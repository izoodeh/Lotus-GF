#include "stdafx.h"
#include "TvTMatch.h"
#include "NpcServer.h"
#include "Door.h"

using namespace TvT;

CMatch::CMatch(TvT::TvTInfo *pInfo) : m_state(TvT::StateNone), m_startTime(0), m_stateTime(0), m_winnerTeam(TvT::TeamNone), m_managerServerId(0), m_killCountBlue(0), m_killCountRed(0)
{
	InitializeCriticalSection(&m_lock);
	m_lpInfo = pInfo;
}

CMatch::~CMatch()
{
	DeleteCriticalSection(&m_lock);
}

void CMatch::Init()
{
	m_startTime = time(NULL);
	m_stateTime = time(NULL);
	m_state = TvT::StateRegistration;
	if(m_lpInfo && m_lpInfo->enabled)
	{
		if(m_lpInfo->registerNpcClassId)
		{
			//spawn npc
			g_NpcServer.SpawnNpc(m_lpInfo->registerNpcClassId, m_lpInfo->registerNpcPos.x, m_lpInfo->registerNpcPos.y, m_lpInfo->registerNpcPos.z);
		}
		if(m_lpInfo->registrationStartMsg1.size() > 0)
		{
			Utils::BroadcastToAllUser_Announce(m_lpInfo->registrationStartMsg1.c_str());
		}
		if(m_lpInfo->registrationStartMsg2.size() > 0)
		{
			Utils::BroadcastToAllUser_Announce(m_lpInfo->registrationStartMsg2.c_str());
		}
		if(m_lpInfo->inviteMessageId != 0)
		{
			SendAskMessage();
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Invalid TvT info!", __FUNCTION__);
		m_state = TvT::StateDelete;
	}
}

void CMatch::SendAskMessage( User *pUser )
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->level >= m_lpInfo->minLevel && pUser->pSD->alive)
		{
			pUser->pUserSocket->Send("cdddd", 0xF3, m_lpInfo->inviteMessageId, 0, 18000, 0);
		}
	}else
	{
		char buff[8190];
		int len = Assemble(buff, 8190, "cdddd", 0xF3, m_lpInfo->inviteMessageId, 0, 9000, 0);
		BroadcastToAllUser("b", len, buff);
	}
}

void CMatch::OnDelete()
{
	if(m_lpInfo->finishTime > 0)
	{
		for(map<UINT, User*>::iterator Iter = m_users.begin();Iter!=m_users.end();Iter++)
		{
			User *pUser = Iter->second;
			if(pUser->ValidUser())
			{
				if(pUser->pEUD->tvtUser.orgPos.x != 0 || pUser->pEUD->tvtUser.orgPos.y != 0)
				{
					pUser->TeleportToLocation(pUser->pEUD->tvtUser.orgPos.x, pUser->pEUD->tvtUser.orgPos.y, pUser->pEUD->tvtUser.orgPos.z, false);
				}
			}
		}
	}
	if(m_lpInfo->doorList.size() > 0)
	{
		//open doors
		for(list<wstring>::iterator Iter = m_lpInfo->doorList.begin(); Iter!= m_lpInfo->doorList.end();Iter++)
		{
			CDoor *pDoor = g_DoorDB.GetDoor(Iter->c_str());
			if(pDoor)
			{
				pDoor->Open();
			}
		}
	}

	//despawn npc
	if(m_lpInfo->endEventMsg.size() > 0)
	{
		Utils::BroadcastToAllUser_Announce(m_lpInfo->endEventMsg.c_str());
	}

	for(map<UINT, User*>::iterator Iter = m_users.begin();Iter!=m_users.end();Iter++)
	{
		User *pUser = Iter->second;
		if(pUser->ValidUser())
		{
			pUser->pEUD->tvtUser.orgPos.x = 0;
			pUser->pEUD->tvtUser.orgPos.y = 0;
			pUser->pEUD->tvtUser.orgPos.z = 0;
			pUser->pEUD->tvtUser.respawnTick = 0;
			pUser->pEUD->tvtUser.resetTimer = 0;
			pUser->pEUD->tvtUser.status = TvT::UserNone;
			pUser->pEUD->tvtUser.team = TvT::TeamNone;
			pUser->SendUserInfo();
			pUser->SendCharInfo();
		}
	}
	m_users.clear();
	//despawn npc
	if(m_managerServerId)
	{
		CNpc *pNpc = CNpc::GetNpc(&m_managerServerId);
		if(pNpc)
		{
			if(pNpc->pSD->alive)
			{
				pNpc->KillNpc();
			}
		}
		m_managerServerId = 0;
	}
}

void CMatch::OnFinish()
{
	m_stateTime = time(NULL);
	if(m_lpInfo->finishTime > 0)
	{
		m_state = TvT::StateFinish;
	}else
	{
		//just end
		m_state = TvT::StateDelete;
	}

	for(map<UINT, User*>::iterator Iter = m_users.begin(); Iter!=m_users.end();Iter++)
	{
		User *pUser = Iter->second;
		if(pUser->ValidUser())
		{
			if(m_lpInfo->rewardId > 0 && m_lpInfo->rewardCount > 0)
			{
				if(m_winnerTeam != TvT::TeamNone && m_winnerTeam == pUser->pEUD->tvtUser.team)
				{
					if(pUser->IsNowTrade())
						pUser->TradeCancel();

					pUser->AddItemToInventory(m_lpInfo->rewardId, m_lpInfo->rewardCount);
				}
			}
			pUser->pEUD->tvtUser.status = TvT::UserFinishing;
		}
	}
	//broadcast who the winner is
	if(m_winnerTeam == TvT::TeamNone)
	{
		if(m_lpInfo->endMsg[0].size() > 0)
		{
			Utils::BroadcastToAllUser_Announce(m_lpInfo->endMsg[0].c_str());
		}
	}else if(m_winnerTeam == TvT::TeamBlue)
	{
		if(m_lpInfo->endMsg[1].size() > 0)
		{
			Utils::BroadcastToAllUser_Announce(m_lpInfo->endMsg[1].c_str());
		}
	}if(m_winnerTeam == TvT::TeamRed)
	{
		if(m_lpInfo->endMsg[2].size() > 0)
		{
			Utils::BroadcastToAllUser_Announce(m_lpInfo->endMsg[2].c_str());
		}
	}
}

void CMatch::Broadcast(const char *format, ...)
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
			pUser->pUserSocket->Send("b", len, buff);
		}
	}
}

bool CMatch::ValidateWinner( bool timeout )
{
	if(m_state == TvT::StateFight)
	{
		UINT blueAlive = 0;
		UINT redAlive = 0;
		for(map<UINT, User*>::iterator Iter = m_users.begin();Iter!=m_users.end();Iter++)
		{
			User *pUser = Iter->second;
			if(pUser->ValidUser())
			{
				if(pUser->pEUD->tvtUser.status == TvT::UserFighting)
				{
					if(pUser->pEUD->tvtUser.team == TvT::TeamBlue)
						blueAlive++;
					else if(pUser->pEUD->tvtUser.team == TvT::TeamRed)
						redAlive++;
				}
			}
		}
		
		if(blueAlive == 0 || redAlive == 0)
		{
			if(blueAlive == 0)
				m_winnerTeam = TvT::TeamRed;
			if(redAlive == 0)
				m_winnerTeam = TvT::TeamBlue;
			if(blueAlive == redAlive && blueAlive == 0)
				m_winnerTeam = TvT::TeamNone;

			return true;
		}
		if(timeout)
		{
			if(m_lpInfo->type == TvT::TvTLastTeamStanding)
			{
				if(blueAlive > redAlive)
				{
					m_winnerTeam = TvT::TeamBlue;
				}else if(redAlive > blueAlive)
				{
					m_winnerTeam = TvT::TeamRed;
				}
			}else if(m_lpInfo->type == TvT::TvTDeathMatch)
			{
				if(m_killCountBlue > m_killCountRed)
				{
					m_winnerTeam = TvT::TeamBlue;
				}else if(m_killCountRed > m_killCountBlue)
				{
					m_winnerTeam = TvT::TeamRed;
				}else
				{
					m_winnerTeam = TvT::TeamNone;
				}
			}
		}

	}
	return false;
}

void CMatch::RandomizeTeams()
{
	guard;

	UINT total = static_cast<UINT>(m_users.size());
	UINT red = 0;
	UINT blue = 0;

	for(map<UINT, User*>::iterator it = m_users.begin();it!=m_users.end();it++)
	{
		if(User *pUser = it->second->CastUser())
		{
			if(red == blue)
			{
				UINT team = g_Random.RandInt(2);
				if(team == 0)
				{
					//blue
					pUser->pEUD->tvtUser.team = TvT::TeamBlue;
					blue++;
				}else
				{
					//red
					pUser->pEUD->tvtUser.team = TvT::TeamRed;
					red++;
				}
			}else if(blue > red)
			{
				pUser->pEUD->tvtUser.team = TvT::TeamRed;
				red++;
			}else
			{
				pUser->pEUD->tvtUser.team = TvT::TeamBlue;
				blue++;
			}
		}
	}

	unguard;
}