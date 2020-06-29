#include "stdafx.h"
#include "Championship.h"
#include "Door.h"
#include "DB.h"

using namespace Championship;

CChampionship g_Championship;


CChampionship::CChampionship() : m_enabled(FALSE), m_loadedDB(FALSE), m_reloadDbTime(0)
{
	SetName(L"Championship");
	memset(m_blockedCrystalType, 0, sizeof(m_blockedCrystalType));
	memset(m_teleportPos, 0, sizeof(m_teleportPos));
};

CChampionship::~CChampionship()
{
}

void CChampionship::Init()
{
	guard;
	LoadINI();
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled", GetName());
		m_reloadDbTime = time(NULL);
		g_DB.RequestLoadChampionshipTeams();
	}
	unguard;
}

void CChampionship::LoadINI()
{
	guard;
	const TCHAR* section = _T("Championship");
	m_enabled = GetPrivateProfileInt(section, _T("Enabled"), FALSE, g_ConfigFile);
	m_teleportTime = GetPrivateProfileInt(section, _T("TeleportTime"), 60, g_ConfigFile);
	TCHAR sTemp[0x8000];
	if(GetPrivateProfileString(section, _T("BlueTeamLocation"), 0, sTemp, 0x8000, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << sTemp;
		tstring pos;
		INT32 idx = 0;
		while(sstr >> pos)
		{
			if(idx > 2)
				break;

			pos = ReplaceChar(pos, _T("/"), _T(" "));
			tstringstream sstr;
			sstr << pos;
			sstr >> m_teleportPos[0][idx].x >> m_teleportPos[0][idx].y >> m_teleportPos[0][idx].z;
			idx++;
		}
	}
	if(GetPrivateProfileString(section, _T("RedTeamLocation"), 0, sTemp, 0x8000, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << sTemp;
		tstring pos;
		INT32 idx = 0;
		while(sstr >> pos)
		{
			if(idx > 2)
				break;

			pos = ReplaceChar(pos, _T("/"), _T(" "));
			tstringstream sstr;
			sstr << pos;
			sstr >> m_teleportPos[1][idx].x >> m_teleportPos[1][idx].y >> m_teleportPos[1][idx].z;
			idx++;
		}
	}

	m_dispelAll = GetPrivateProfileInt(section, _T("DispelAll"), 1, g_ConfigFile);
	m_prepareTime = GetPrivateProfileInt(section, _T("PrepareTime"), 60, g_ConfigFile);
	m_doorTime = GetPrivateProfileInt(section, _T("DoorTime"), 0, g_ConfigFile);

	if(GetPrivateProfileString(section, _T("DoorName"), 0, sTemp, 0x8000, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << sTemp;
		wstring doorName;
		while(sstr >> doorName)
		{
			m_doors.push_back(doorName);
		}
	}

	m_fightTime = GetPrivateProfileInt(section, _T("FightTime"), 300, g_ConfigFile);
	m_rewardId = GetPrivateProfileInt(section, _T("RewardId"), 0, g_ConfigFile);
	m_rewardCount = GetPrivateProfileInt(section, _T("RewardCount"), 0, g_ConfigFile);
	m_teleportBackTime = GetPrivateProfileInt(section, _T("TeleportBackTime"), 15, g_ConfigFile);

	if(GetPrivateProfileString(section, _T("BlockedSkillList"), 0, sTemp, 0x8000, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << sTemp;
		INT32 skillId = 0;
		while(sstr >> skillId)
		{
			m_blockedSkill.push_back(skillId);
		}
		m_blockedSkill.sort();
		m_blockedSkill.unique();
	}
	if(GetPrivateProfileString(section, _T("BlockedItemList"), 0, sTemp, 0x8000, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << sTemp;
		INT32 itemId = 0;
		while(sstr >> itemId)
		{
			m_blockedItem.push_back(itemId);
		}
		m_blockedItem.sort();
		m_blockedItem.unique();
	}
	if(GetPrivateProfileString(section, _T("BlockedCrystalTypeList"), 0, sTemp, 0x8000, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << sTemp;
		UINT crystalType = 0;
		while(sstr >> crystalType)
		{
			if(crystalType < 10)
			{
				m_blockedCrystalType[crystalType] = true;
			}else
			{
				g_Log.Add(CLog::Error, "[%s] CrystalType overflow[%d]!", __FUNCTION__, crystalType);
			}
		}
	}

	m_blockAugmentation = GetPrivateProfileInt(section, _T("BlockAugmentation"), 0, g_ConfigFile);
	m_blockAttribute = GetPrivateProfileInt(section, _T("BlockAttribute"), 0, g_ConfigFile);
	m_maxArmorEnchant = GetPrivateProfileInt(section, _T("MaxArmorEnchant"), 0, g_ConfigFile);
	m_maxWeaponEnchant = GetPrivateProfileInt(section, _T("MaxWeaponEnchant"), 0, g_ConfigFile);
	m_autoReloadDB = GetPrivateProfileInt(section, _T("AutoReloadDB"), 0, g_ConfigFile);
	unguard;
}

bool CChampionship::IsBlockedSkill(INT32 skillId)
{
	for(list<INT32>::iterator Iter = m_blockedSkill.begin(); Iter!=m_blockedSkill.end();Iter++)
	{
		if((*Iter) == skillId)
			return true;
	}
	return false;
}

bool CChampionship::IsBlockedItem(INT32 itemId)
{
	for(list<INT32>::iterator Iter = m_blockedItem.begin(); Iter!=m_blockedItem.end();Iter++)
	{
		if((*Iter) == itemId)
			return true;
	}
	return false;
}

bool CChampionship::IsBlockedCrystalType(UINT crystalType)
{
	if(crystalType < 10)
	{
		return m_blockedCrystalType[crystalType];
	}
	return false;
}

void CChampionship::AddTeam(UINT id, std::wstring name, UINT charId1, UINT charId2, UINT charId3, INT32 points, INT32 winCount, INT32 loseCount, INT32 drawCount, INT32 killCount, INT32 deathCount, UINT teamCount)
{
	guard;
	if(m_enabled)
	{
		SIZE_T size = 0;
		TeamData *pData = new TeamData();
		pData->id = id;
		pData->name = name;
		pData->charId[0] = charId1;
		pData->charId[1] = charId2;
		pData->charId[2] = charId3;
		pData->points = points;
		pData->winCount = winCount;
		pData->loseCount = loseCount;
		pData->drawCount = drawCount;
		pData->killCount = killCount;
		pData->deathCount = deathCount;
		Lock();
		m_team.insert(pair<UINT, TeamData*>(pData->id, pData));
		size = m_team.size();
		Unlock();
		if(size == teamCount)
		{
			g_Log.Add(CLog::Blue, "[%S] Loaded [%d] teams", GetName(), size);
			g_Log.Add(CLog::Blue, "[%S] Loading matches...", GetName());
			g_DB.RequestLoadChampionshipMatches();
		}
	}
	unguard;
}

void CChampionship::AddMatch(UINT id, UINT state, UINT startTime, UINT teamId1, UINT teamId2, UINT winnerId, UINT matchCount)
{
	guard;
	if(m_enabled)
	{
		SIZE_T size = 0;
		CMatch* pData = new CMatch();
		pData->id = id;
		pData->startTime = startTime;

		if(state == Championship::MatchWaiting)
			pData->state = Championship::MatchWaiting;
		else if(state == Championship::MatchTeleporting)
			pData->state = Championship::MatchTeleporting;
		else if(state == Championship::MatchPreparing)
			pData->state = Championship::MatchPreparing;
		else if(state == Championship::MatchFighting)
			pData->state = Championship::MatchFighting;
		else if(state == Championship::MatchDone)
			pData->state = Championship::MatchDone;

		pData->teamId[0] = teamId1;
		pData->teamId[1] = teamId2;
		pData->winnerId = winnerId;
		Lock();
		TeamMap::iterator teamIter = m_team.find(teamId1);
		if(teamIter != m_team.end())
		{
			pData->teamData[0] = teamIter->second;
		}else
		{
			g_Log.Add(CLog::Error, "[%S] Cannot find team data for teamId[%d] matchId[%d]!", GetName(), teamId1, id);
			pData->teamData[0] = 0;
		}
		teamIter = m_team.find(teamId2);
		if(teamIter != m_team.end())
		{
			pData->teamData[1] = teamIter->second;
		}else
		{
			g_Log.Add(CLog::Error, "[%S] Cannot find team data for teamId[%d] matchId[%d]!", GetName(), teamId2, id);
			pData->teamData[1] = 0;
		}

		m_match.insert(pair<UINT, CMatch*>(id, pData));
		size = m_match.size();
		Unlock();

		if(size == matchCount)
		{
			g_Log.Add(CLog::Blue, "[%S] Loaded [%d] matches", GetName(), size);
			m_loadedDB = TRUE;
		}
	}
	unguard;
}

void CChampionship::OnLeaveWorld(User *pUser)
{
	guard;

	if(m_enabled && pUser->ValidUser())
	{
		if(pUser->pEUD->championshipUser.matchId > 0)
		{
			//remove user from match
			Lock();
			if(m_pMatch)
			{
				if(m_pMatch->id == pUser->pEUD->championshipUser.matchId)
				{
					map<UINT, User*>::iterator userIter = m_pMatch->m_users.find(pUser->serverId);
					if(userIter!=m_pMatch->m_users.end())
					{
						m_pMatch->m_users.erase(userIter);
					}
				}
			}
			Unlock();
		}
	}

	unguard;
}

void CChampionship::OnEnterWorld(User *pUser)
{
	guard;
	if(m_enabled && pUser->ValidUser())
	{
		if(m_pMatch)
		{
			if(m_pMatch->state == Championship::MatchTeleporting)
			{
				for(int n=0;n<2;n++)
				{
					//3020	1	a,Your match against Team $s1 will commence in $s2 minute(s). Please make sure your character is ready for battle.\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
					CSystemMessage msg(3020);
					if(n == 0)
						msg.AddText(m_pMatch->teamData[1]->name.c_str());
					else
						msg.AddText(m_pMatch->teamData[0]->name.c_str());
					msg.AddNumber(m_teleportTime / 60);
					for(int m=0;m < 3; m++)
					{
						if(pUser->databaseId == m_pMatch->teamData[n]->charId[m])
						{
							if(User *pUserEx = m_pMatch->AddUser(pUser->databaseId, m_pMatch->teamData[n]->type))
							{
								pUser->pUserSocket->SendSystemMessage(msg);
							}
						}
					}
				}
			}
		}
	}
	unguard;
}

void CChampionship::TimerExpired()
{
	guard;

	if(m_enabled)
	{
		time_t currentTime = time(NULL);
		Lock();
		if(m_pMatch)
		{
			switch(m_pMatch->state)
			{
			case Championship::MatchTeleporting:
				{
					time_t diff = m_pMatch->m_stateTime + m_teleportTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(diff == 60)
						{
							//3021	1	a,You will be moved to the Championship Stadium in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3021);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 40)
						{
							//3021	1	a,You will be moved to the Championship Stadium in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3021);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 20)
						{
							//3021	1	a,You will be moved to the Championship Stadium in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3021);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 10)
						{
							//3021	1	a,You will be moved to the Championship Stadium in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3021);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff <= 5)
						{
							//3021	1	a,You will be moved to the Championship Stadium in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3021);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}
					}else
					{
						m_pMatch->state = Championship::MatchPreparing;
						m_pMatch->m_stateTime = currentTime;

						//check players condition
						vector<UINT> toRemove;
						for(map<UINT, User*>::iterator userIter = m_pMatch->m_users.begin(); userIter != m_pMatch->m_users.end(); userIter++)
						{
							User *pUser = userIter->second;
							if(pUser->ValidUser())
							{
								if(pUser->olympiadUser.status > 0)
								{
									CSystemMessage msg(1983);
									msg.AddText(L"You have been excluded from the championship match because you are currently in the Grand Olympiad Games.");
									pUser->pUserSocket->SendSystemMessage(msg);
									toRemove.push_back(userIter->first);
									continue;
								}
								if(pUser->pSD->karma > 0)
								{
									CSystemMessage msg(1983);
									msg.AddText(L"You have been excluded from the championship match because you are in a state of karma.");
									pUser->pUserSocket->SendSystemMessage(msg);
									toRemove.push_back(userIter->first);
									continue;
								}
								if(pUser->pSD->alive == 0)
								{
									CSystemMessage msg(1983);
									msg.AddText(L"You have been excluded from the championship match because you are currently dead.");
									pUser->pUserSocket->SendSystemMessage(msg);
									toRemove.push_back(userIter->first);
									continue;
								}
							}
						}

						for(UINT n = 0 ; n < toRemove.size(); n++)
						{
							map<UINT, User*>::iterator userIter = m_pMatch->m_users.find(toRemove[n]);
							if(userIter!=m_pMatch->m_users.end())
							{
								m_pMatch->m_users.erase(userIter);
							}
						}



						//check for missing players
						UINT blueCount = 0;
						UINT redCount = 0;
						for(map<UINT, User*>::iterator userIter = m_pMatch->m_users.begin(); userIter != m_pMatch->m_users.end(); userIter++)
						{
							User *pUser = userIter->second;
							if(pUser->ValidUser())
							{
								if(pUser->pEUD->championshipUser.teamType == Championship::TeamBlue)
									blueCount++;
								else if(pUser->pEUD->championshipUser.teamType == Championship::TeamRed)
									redCount++;
							}
						}

						if(blueCount == 3 && redCount == blueCount)
						{
							//port players to locations
							UINT teleportIdx[2] = { 0, 0 };
							for(map<UINT, User*>::iterator userIter = m_pMatch->m_users.begin(); userIter != m_pMatch->m_users.end(); userIter++)
							{
								User *pUser = userIter->second;
								if(pUser->ValidUser())
								{
									int teamIdx = pUser->pEUD->championshipUser.teamType -1;
									if(teamIdx >= 0 && teamIdx < 2)
									{
										if(m_dispelAll)
										{
											pUser->DispelAllByGM();
										}
										pUser->RemoveSkillUsableTimeAll();
										pUser->pEUD->championshipUser.state = Championship::UserPreparing;
										ValidateItems(pUser);
										pUser->Healthy();
										if(teleportIdx[teamIdx] < 3)
										{
											if(m_teleportPos[teamIdx][teleportIdx[teamIdx]].x != 0 || m_teleportPos[teamIdx][teleportIdx[teamIdx]].y != 0)
											{
												pUser->pEUD->championshipUser.orgPos = pUser->pSD->pos;
												//3032	1	a,You are now being moved to the Championship Stadium.\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
												pUser->pUserSocket->SendSystemMessage(3032);
												pUser->TeleportToLocation(m_teleportPos[teamIdx][teleportIdx[teamIdx]].x, m_teleportPos[teamIdx][teleportIdx[teamIdx]].y, m_teleportPos[teamIdx][teleportIdx[teamIdx]].z);
											}
											teleportIdx[teamIdx]++;
										}
									}
								}
							}
							break;
						}else
						{
							m_pMatch->state = Championship::MatchRelease;

							//missing players end match
							if(blueCount < 3 && redCount < 3)
							{
								//both teams are missing players
								//3022	1	a,The match between Team $s1 and Team $s2 has ended in a draw.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg(3022);
								msg.AddText(m_pMatch->teamData[0]->name.c_str());
								msg.AddText(m_pMatch->teamData[1]->name.c_str());
								BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());
								m_pMatch->winnerId = 0; //draw
							}else if(blueCount < 3)
							{
								m_pMatch->winnerId = m_pMatch->teamData[1]->id;
								//3024	1	a,Team $s1 has won the match against Team $s2.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg(3024);
								msg.AddText(m_pMatch->teamData[1]->name.c_str());
								msg.AddText(m_pMatch->teamData[0]->name.c_str());
								BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());

								//3025	1	a,Your team has won the match because your opponent is missing one or more team members.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg2(3025);
								m_pMatch->Broadcast(Championship::TeamRed, "b", msg2.GetSize(), msg2.GetBuff());
								//3026	1	a,Your team has lost the match because you are missing one or more team members.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg3(3026);
								m_pMatch->Broadcast(Championship::TeamBlue, "b", msg3.GetSize(), msg3.GetBuff());
							}else
							{
								m_pMatch->winnerId = m_pMatch->teamData[0]->id;
								//3024	1	a,Team $s1 has won the match against Team $s2.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg(3024);
								msg.AddText(m_pMatch->teamData[0]->name.c_str());
								msg.AddText(m_pMatch->teamData[1]->name.c_str());
								BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());

								//3025	1	a,Your team has won the match because your opponent is missing one or more team members.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg2(3025);
								m_pMatch->Broadcast(Championship::TeamBlue, "b", msg2.GetSize(), msg2.GetBuff());
								//3026	1	a,Your team has lost the match because you are missing one or more team members.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
								CSystemMessage msg3(3026);
								m_pMatch->Broadcast(Championship::TeamRed, "b", msg3.GetSize(), msg3.GetBuff());
							}
						}
					}
					break;
				}
			case Championship::MatchPreparing:
				{
					time_t diff = m_pMatch->m_stateTime + m_prepareTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(diff == 90)
						{
							//3027	1	a,The match will begin in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3027);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 60)
						{
							//3027	1	a,The match will begin in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3027);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 40)
						{
							//3027	1	a,The match will begin in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3027);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 20)
						{
							//3027	1	a,The match will begin in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3027);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 10)
						{
							//3027	1	a,The match will begin in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3027);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff <= 5)
						{
							//3027	1	a,The match will begin in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3027);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}

						if(diff <= m_doorTime && m_pMatch->m_openDoor)
						{
							m_pMatch->m_openDoor = FALSE;
							if(m_doors.size() > 0)
							{
								for(list<wstring>::iterator doorIter = m_doors.begin(); doorIter != m_doors.end(); doorIter++)
								{
									CDoor *pDoor = g_DoorDB.GetDoor(doorIter->c_str());
									if(pDoor)
										pDoor->Open();
								}
							}
						}
					}else
					{
						m_pMatch->m_stateTime = currentTime;
						m_pMatch->state = Championship::MatchFighting;
						//change user state
						for(map<UINT, User*>::iterator userIter = m_pMatch->m_users.begin();userIter!=m_pMatch->m_users.end();userIter++)
						{
							User *pUser = userIter->second;
							if(pUser->ValidUser())
							{
								pUser->pEUD->championshipUser.state = Championship::UserFighting;
								pUser->SendUserInfo();
								pUser->SendCharInfo();
							}
						}
						//broadcast messages
						//3028	1	a,The match between Team $s1 and Team $s2 has begun.\0	0	B2	B0	5A	FF	a,	a,	0	0	0	0	0	a,	a,none\0
						CSystemMessage msg(3028);
						msg.AddText(m_pMatch->teamData[0]->name.c_str());
						msg.AddText(m_pMatch->teamData[1]->name.c_str());
						BroadcastToAllUser("b", msg.GetSize(), msg.GetBuff());

						//3031	1	a,The match has begun.\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,Begin!\0	a,none\0
						CSystemMessage msg2(3031);
						m_pMatch->Broadcast(Championship::TeamNone, "b", msg2.GetSize(), msg2.GetBuff());
					}
					break;
				};
			case Championship::MatchFighting:
				{
					time_t diff = m_pMatch->m_stateTime + m_fightTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(diff <= 15)
						{
							//3029	1	a,The match will end in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	2	0	3	1	0	a,$s1!\0	a,none\0
							CSystemMessage msg(3029);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}

						if(m_pMatch->ValidateWinner())
						{
							//we've got a winner
							m_pMatch->OnFinish(m_rewardId, m_rewardCount);
						}
					}else
					{
						m_pMatch->ValidateWinner(true);
						m_pMatch->OnFinish(m_rewardId, m_rewardCount);
					}
					break;
				}
			case Championship::MatchTeleportBack:
				{
					time_t diff = m_pMatch->m_stateTime + m_teleportBackTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(diff == 15)
						{
							//3030	1	a,You will be moved back in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3030);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 10)
						{
							//3030	1	a,You will be moved back in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3030);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff <= 5)
						{
							//3030	1	a,You will be moved back in $s1 second(s).\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
							CSystemMessage msg(3030);
							msg.AddNumber(static_cast<UINT>(diff));
							m_pMatch->Broadcast(Championship::TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}

					}else
					{
						m_pMatch->state = Championship::MatchRelease;
						m_pMatch->m_stateTime = currentTime;
						//port back to org pos
						for(map<UINT, User*>::iterator userIter = m_pMatch->m_users.begin();userIter!=m_pMatch->m_users.end();userIter++)
						{
							User *pUser = userIter->second;
							if(pUser->ValidUser())
							{
								if(pUser->pEUD->championshipUser.orgPos.x != 0 || pUser->pEUD->championshipUser.orgPos.y != 0)
								{
									pUser->Healthy();
									pUser->TeleportToLocation(pUser->pEUD->championshipUser.orgPos.x, pUser->pEUD->championshipUser.orgPos.y, pUser->pEUD->championshipUser.orgPos.z);
								}
							}
						}
					}
					break;
				}
			case Championship::MatchRelease:
				{
					if(m_doors.size() > 0)
					{
						for(list<wstring>::iterator doorIter = m_doors.begin(); doorIter != m_doors.end(); doorIter++)
						{
							CDoor *pDoor = g_DoorDB.GetDoor(doorIter->c_str());
							if(pDoor)
								pDoor->Close();
						}
					}

					m_pMatch->OnRelease();
					m_pMatch = 0;
					break;
				}
			};
		}else
		{
			if(m_loadedDB)
			{
				if(m_autoReloadDB)
				{
					if( (m_reloadDbTime+m_autoReloadDB) < time(NULL))
					{
						m_loadedDB = FALSE;
						//release matches
						try
						{
							for(MatchMap::iterator matchIter = m_match.begin();matchIter!=m_match.end(); matchIter++)
							{
								delete matchIter->second;
								matchIter->second = 0;
							}
						}catch(...)
						{
							EXCEPTION_LOG_PARAM("releasing CMatch");
						}
						m_match.clear();
						g_DB.RequestLoadChampionshipMatches();
						m_reloadDbTime = time(NULL);
						g_Log.Add(CLog::Blue, "[%S] Reloading matches...", GetName());
					}
				}
				//iterate through matches
				for(MatchMap::iterator matchIter = m_match.begin();matchIter!=m_match.end(); matchIter++)
				{
					if(matchIter->second->state == Championship::MatchWaiting)
					{
						if(matchIter->second->startTime < currentTime)
						{
							//start match
							m_pMatch = matchIter->second;
							m_pMatch->Init(m_teleportTime);
							break;
						}
					}
				}
			}
		}
		Unlock();
	}
	unguard;
}

void CChampionship::OnDie(User *pUser)
{
	guard;
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			Lock();
			if(m_pMatch && pUser->pEUD->championshipUser.matchId == m_pMatch->id)
			{
				if(pUser->pEUD->championshipUser.state = Championship::UserFighting)
				{
					if(pUser->pEUD->championshipUser.teamType == Championship::TeamBlue)
					{
						m_pMatch->m_deathCount[0]++;
						m_pMatch->m_killCount[1]++;
					}else if(pUser->pEUD->championshipUser.teamType == Championship::TeamRed)
					{
						m_pMatch->m_deathCount[1]++;
						m_pMatch->m_killCount[0]++;
					}
				}
			}
			Unlock();
		}
	}
	unguard;
}

void CChampionship::ValidateItems(User *pUser)
{
	guard;

	if(pUser->ValidUser())
	{
		for(int n=0;n<29;n++)
		{
			INT32 index = pUser->pSD->equipment[n];
			if(index > 0)
			{
				CItem *pItem = pUser->inventory.GetByIndex(index);
				if(pItem)
				{
					if(IsBlockedItem(pItem->pSID->itemId))
					{
						pUser->UnequipItem(pItem);
						continue;
					}
					if(IsBlockedCrystalType(pItem->pII->crystalType))
					{
						pUser->UnequipItem(pItem);
						continue;
					}

					if(m_blockAugmentation && pItem->pSID->isAugmented)
					{
						pUser->UnequipItem(pItem);
						continue;
					}


					if(pItem->IsWeapon())
					{
						if(pItem->pII->weaponType == WeaponDual)
						{
							if( pItem->pSID->enchantLevel > (m_maxWeaponEnchant + 1) )
							{
								pUser->UnequipItem(pItem);
								continue;
							}
						}else
						{
							if(pItem->pSID->enchantLevel > m_maxWeaponEnchant)
							{
								pUser->UnequipItem(pItem);
								continue;
							}
						}
					}else
					{
						if(pItem->pSID->slotType == SlotOnePiece)
						{
							if( pItem->pSID->enchantLevel > (m_maxWeaponEnchant + 1) )
							{
								pUser->UnequipItem(pItem);
								continue;
							}
						}else
						{
							if( pItem->pSID->enchantLevel > m_maxWeaponEnchant )
							{
								pUser->UnequipItem(pItem);
								continue;
							}
						}
					}

					if(m_blockAttribute)
					{

						if(pItem->attribute.attackAttribute != static_cast<USHORT>(-2) || pItem->attribute.defAttribute[0] != 0 || pItem->attribute.defAttribute[1] != 0 || pItem->attribute.defAttribute[2] != 0 
							|| pItem->attribute.defAttribute[3] != 0 || pItem->attribute.defAttribute[4] != 0 || pItem->attribute.defAttribute[5] != 0 )
						{
							pUser->UnequipItem(pItem);
							continue;
						}						
					}
				}
			}
		}
	}
	unguard;
}

bool CChampionship::CanUse(CItem *pItem)
{
	if(IsBlockedItem(pItem->pSID->itemId))
		return false;
	if(IsBlockedCrystalType(pItem->pII->crystalType))
		return false;

	if(m_blockAugmentation && pItem->pSID->isAugmented)
		return false;

	if(m_blockAttribute)
	{
		if(pItem->attribute.attackAttribute != static_cast<USHORT>(-2) || pItem->attribute.defAttribute[0] != 0 || pItem->attribute.defAttribute[1] != 0 || pItem->attribute.defAttribute[2] != 0 
			|| pItem->attribute.defAttribute[3] != 0 || pItem->attribute.defAttribute[4] != 0 || pItem->attribute.defAttribute[5] != 0 )
		{
			return false;
		}						
	}

	if(pItem->IsWeapon() && m_maxWeaponEnchant > 0)
	{
		if(pItem->pII->weaponType == WeaponDual)
		{
			if( pItem->pSID->enchantLevel > (m_maxWeaponEnchant + 1) )
			{
				return false;
			}
		}else
		{
			if(pItem->pSID->enchantLevel > m_maxWeaponEnchant)
			{
				return false;
			}
		}
	}else if(m_maxArmorEnchant > 0)
	{
		if(pItem->pSID->slotType == SlotOnePiece)
		{
			if( pItem->pSID->enchantLevel > (m_maxWeaponEnchant + 1) )
			{
				return false;
			}
		}else
		{
			if( pItem->pSID->enchantLevel > m_maxWeaponEnchant )
			{
				return false;
			}
		}
	}

	return true;
}