#include "stdafx.h"
#include "ChampionshipMatch.h"
#include "UserDb.h"
#include "DB.h"

using namespace Championship;

CMatch::CMatch() : m_stateTime(0), m_openDoor(TRUE)
{
	m_killCount[0] = 0;
	m_killCount[1] = 0;
	m_deathCount[0] = 0;
	m_deathCount[1] = 0;
};

CMatch::~CMatch()
{
}

void CMatch::Broadcast(Championship::TeamType team, const char *format, ...)
{
	va_list va;
	va_start(va, format);

	char buff[8190];
	int len = VAssemble(buff, 8190, format, va);
	va_end(va);

	for(map<UINT, User*>::iterator Iter = m_users.begin();Iter!=m_users.end();Iter++)
	{
		User *pUser = Iter->second;
		if(pUser->ValidUser())
		{
			if(team == Championship::TeamNone || pUser->pEUD->championshipUser.teamType == team)
			{
				pUser->pUserSocket->Send("b", len, buff);
			}
		}
	}
}

void CMatch::Init(INT32 teleportTime)
{
	m_stateTime = time(NULL);
	state = Championship::MatchTeleporting;

	//Check for required data
	if(teamData[0] && teamData[1])
	{
		//3019	1	a,The match between Team $s1 and Team $s2 is about to commence. Use the Broadcasting Towers located in town to spectate.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
		CSystemMessage msg(3019);
		msg.AddText(teamData[0]->name.c_str());
		msg.AddText(teamData[1]->name.c_str());
		BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());

		//Fill up users
		for(int n=0; n < 2; n++)
		{
			TeamType team;
			if(n == 0)
				team = Championship::TeamBlue;
			else
				team = Championship::TeamRed;
			//set team type
			teamData[n]->type = team;
			//3020	1	a,Your match against Team $s1 will commence in $s2 minute(s). Please make sure your character is ready for battle.\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
			CSystemMessage msg(3020);
			if(n == 0)
				msg.AddText(teamData[1]->name.c_str());
			else
				msg.AddText(teamData[0]->name.c_str());
			msg.AddNumber(teleportTime / 60);
			for(int m=0;m < 3; m++)
			{
				if(User *pUser = AddUser(teamData[n]->charId[m], team))
				{
					pUser->pUserSocket->SendSystemMessage(msg);
				}else
				{
					g_Log.Add(CLog::Error, "[%s] User[%d] is not online! [%d][%d]", __FUNCTION__, teamData[n]->charId[m], n, m);
				}
			}
		}

	}else
	{
		g_Log.Add(CLog::Error, "[%s] Cannot initialize match without team data matchId[%d] team[%d][%d] !", __FUNCTION__, id, teamId[0], teamId[1]);
		state = Championship::MatchRelease;
	}
}

User* CMatch::AddUser(UINT charId, Championship::TeamType team)
{
	User *pUser = g_UserDb.FindByDatabaseId(charId);
	if(pUser->ValidUser())
	{
		pUser->pEUD->championshipUser.matchId = id;
		pUser->pEUD->championshipUser.teamType = team;
		pUser->pEUD->championshipUser.state = Championship::UserTeleporting;
		m_users.insert(pair<UINT, User*>(pUser->serverId, pUser));
		return pUser;
	}
	return 0;
}

void CMatch::OnRelease()
{
	guard;

	INT32 newPoints[2] = { 0, 0 };
	//save resoults
	if(winnerId == 0)
	{
		newPoints[0] = 1;
		newPoints[1] = 1;
		//draw
		teamData[0]->drawCount++;
		teamData[0]->points += newPoints[0];
		teamData[1]->drawCount++;
		teamData[1]->points += newPoints[1];
	}else if(winnerId == teamData[0]->id)
	{
		newPoints[0] = 3;
		//blue team won
		teamData[0]->winCount++;
		teamData[0]->points += newPoints[0];
		teamData[1]->loseCount++;
	}else if(winnerId == teamData[1]->id)
	{
		newPoints[1] = 3;
		//red team won
		teamData[1]->winCount++;
		teamData[1]->points += newPoints[1];
		teamData[0]->loseCount++;
	}

	teamData[0]->killCount += m_killCount[0];
	teamData[1]->killCount += m_killCount[1];
	teamData[0]->deathCount += m_deathCount[0];
	teamData[1]->deathCount += m_deathCount[1];

	state = Championship::MatchDone;

	g_DB.RequestSaveChampionshipMatch(id, state, winnerId);
	g_DB.RequestSaveChampionshipTeam(teamData[0]->id, teamData[0]->points, teamData[0]->winCount, teamData[0]->loseCount, teamData[0]->drawCount, teamData[0]->killCount, teamData[0]->deathCount);
	g_DB.RequestSaveChampionshipTeam(teamData[1]->id, teamData[1]->points, teamData[1]->winCount, teamData[1]->loseCount, teamData[1]->drawCount, teamData[1]->killCount, teamData[1]->deathCount);

	//3023	1	a,Your team has gained $s1 point(s) in the championship ranking. Your team currently has a total of $s2 points.\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
	CSystemMessage msg(3023);
	msg.AddNumber(newPoints[0]);
	msg.AddNumber(teamData[0]->points);
	CSystemMessage msg2(3023);
	msg2.AddNumber(newPoints[1]);
	msg2.AddNumber(teamData[1]->points);
	Broadcast(Championship::TeamBlue, "b", msg.GetSize(), msg.GetBuff());
	Broadcast(Championship::TeamRed, "b", msg2.GetSize(), msg2.GetBuff());

	for(map<UINT, User*>::iterator Iter = m_users.begin(); Iter!=m_users.end();Iter++)
	{
		User *pUser = Iter->second;
		if(pUser->ValidUser())
		{
			if(pUser->pEUD->championshipUser.orgPos.x != 0 || pUser->pEUD->championshipUser.orgPos.y != 0)
			{
				//teleport back
				pUser->TeleportToLocation(pUser->pEUD->championshipUser.orgPos.x, pUser->pEUD->championshipUser.orgPos.y, pUser->pEUD->championshipUser.orgPos.z);
			}
			pUser->pEUD->championshipUser.Clear();
		}
	}
	m_users.clear();
	unguard;
}

bool CMatch::ValidateWinner( bool timeout )
{
	UINT blueAlive = 0;
	UINT redAlive = 0;
	for(map<UINT, User*>::iterator userIter = m_users.begin(); userIter!=m_users.end(); userIter++)
	{
		User *pUser = userIter->second;
		if(pUser->ValidUser())
		{
			if(pUser->pSD->alive)
			{
				if(pUser->pEUD->championshipUser.teamType == Championship::TeamBlue)
					blueAlive++;
				else if(pUser->pEUD->championshipUser.teamType == Championship::TeamRed)
					redAlive++;

			}
		}
	}
	if(blueAlive == 0 || redAlive == 0)
	{
		if(blueAlive == redAlive)
		{
			//draw
			winnerId = 0;
		}else if(blueAlive == 0)
		{
			//red winner
			winnerId = teamData[1]->id;
		}else
		{
			//blue winner
			winnerId = teamData[0]->id;
		}
		return true;
	}

	if(timeout)
	{
		if(m_killCount[0] > m_killCount[1])
		{
			winnerId = teamData[0]->id;
		}else if(m_killCount[0] < m_killCount[1])
		{
			winnerId = teamData[1]->id;
		}else
		{
			winnerId = 0;
		}
		return true;
	}
	return false;
}

void CMatch::OnFinish(INT32 rewardId, INT32 rewardCount)
{
	m_stateTime = time(NULL);
	state = Championship::MatchTeleportBack;

	if(winnerId == 0)
	{
		//3022	1	a,The match between Team $s1 and Team $s2 has ended in a draw.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
		CSystemMessage msg(3022);
		msg.AddText(teamData[0]->name.c_str());
		msg.AddText(teamData[1]->name.c_str());
		BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());
	}else
	{
		//3024	1	a,Team $s1 has won the match against Team $s2.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
		CSystemMessage msg(3024);
		Championship::TeamType winType = Championship::TeamNone;
		if(winnerId == teamData[0]->id)
		{
			winType = Championship::TeamBlue;
			msg.AddText(teamData[0]->name.c_str());
			msg.AddText(teamData[1]->name.c_str());
		}else
		{
			winType = Championship::TeamRed;
			msg.AddText(teamData[1]->name.c_str());
			msg.AddText(teamData[0]->name.c_str());
		}
		BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());
				
		if(winType != Championship::TeamNone)
		{
			//3033	1	a,Congratulations. You have won the match!\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
			CSystemMessage msg2(3033);
			Broadcast(winType, "b", msg2.GetSize(), msg2.GetBuff());
		}

	}

	for(map<UINT, User*>::iterator userIter = m_users.begin(); userIter != m_users.end(); userIter++)
	{
		User *pUser = userIter->second;
		if(pUser->ValidUser())
		{
			pUser->pEUD->championshipUser.state = Championship::UserTeleportingBack;
			pUser->SendUserInfo();
			pUser->SendCharInfo();
			UINT teamType = pUser->pEUD->championshipUser.teamType;
			if(winnerId > 0 && teamType < 2 && rewardId > 0 && rewardCount > 0)
			{
				if(winnerId == teamData[teamType]->id)
				{
					if(pUser->IsNowTrade())
						pUser->TradeCancel();

					if(pUser->inventory.CheckAddable(rewardId, rewardCount))
					{
						pUser->AddItemToInventory(rewardId, rewardCount);
					}
				}
			}
		}
	}
}