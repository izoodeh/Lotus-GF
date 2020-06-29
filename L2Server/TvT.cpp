#include "stdafx.h"
#include "TvT.h"
#include "UserDb.h"
#include "Door.h"
#include "Npc.h"
#include "UserDb.h"

using namespace TvT;

CTvT g_TvT;

CTvT::CTvT() : m_lpEvent(0), m_enabled(FALSE)
{
	SetName(L"TvT");
}

CTvT::~CTvT()
{

}

void CTvT::Init()
{
	m_tvtInfo.clear();
	m_enabled = FALSE;
	LoadINI();
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled, loaded [%d] events", GetName(), m_tvtInfo.size());
	}
}

void CTvT::LoadINI()
{
	TCHAR g_TvTFile[260];
	GetCurrentDirectory(MAX_PATH,g_TvTFile);
	lstrcat(g_TvTFile, TEXT("\\TvT.ini"));
	const TCHAR* section = _T("TvTSystem");
	m_enabled = GetPrivateProfileInt(section, _T("Enabled"), FALSE, g_TvTFile);
	INT32 count = GetPrivateProfileInt(section, _T("TvTEventCount"), 0, g_TvTFile);
	if(count > 0)
	{
		for(int n=1;n<=count;n++)
		{
			tstringstream sectionEx;
			sectionEx << "TvT_" << n ;
			TvT::TvTInfo* pInfo = new TvT::TvTInfo();
			pInfo->enabled = GetPrivateProfileInt(sectionEx.str().c_str(), _T("Enabled"), FALSE, g_TvTFile);
			TCHAR sTemp[0x8000];
			memset(sTemp, 0, sizeof(sTemp));
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("Type"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring type = sTemp;
				if(type == _T("LTS"))
				{
					pInfo->type = TvT::TvTLastTeamStanding;
				}else if(type == _T("DM"))
				{
					pInfo->type = TvT::TvTDeathMatch;
				}else
				{
					pInfo->type = TvT::TvTNone;
				}
			}
			UINT dayFlag = GetPrivateProfileInt(sectionEx.str().c_str(), _T("DayFlag"), 0, g_TvTFile);
			for(UINT m = 1; m < 8; m++)
			{
				if((dayFlag % 10) == 1)
				{
					pInfo->dayFlag[m-1] = true;
				}else
				{
					pInfo->dayFlag[m-1] = false;
				}
				dayFlag /= 10;
			}
			pInfo->startHour = GetPrivateProfileInt(sectionEx.str().c_str(), _T("StartHour"), 14, g_TvTFile);
			pInfo->startMinute = GetPrivateProfileInt(sectionEx.str().c_str(), _T("StartMinute"), 30, g_TvTFile);
			pInfo->registerTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RegisterTime"), 300, g_TvTFile);
			pInfo->prepareTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("PrepareTime"), 50, g_TvTFile);
			pInfo->countDownTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("CountDownTime"), 30, g_TvTFile);
			pInfo->fightTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("FightTime"), 300, g_TvTFile);
			pInfo->finishTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("FinishTime"), 15, g_TvTFile);
			pInfo->respawnTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RespawnTime"), 15, g_TvTFile);
			pInfo->backToFightTime = GetPrivateProfileInt(sectionEx.str().c_str(), _T("BackToFightTime"), 30, g_TvTFile);
			pInfo->dispelAbnormal = GetPrivateProfileInt(sectionEx.str().c_str(), _T("DispelAbnormal"), 0, g_TvTFile);
			pInfo->rewardId = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RewardId"), 0, g_TvTFile);
			pInfo->rewardCount = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RewardCount"), 0, g_TvTFile);
			pInfo->friendlyFire = GetPrivateProfileInt(sectionEx.str().c_str(), _T("FriendlyFire"), 0, g_TvTFile);
			pInfo->hardwareIdLimit = GetPrivateProfileInt(sectionEx.str().c_str(), _T("HardwareIdLimit"), 0, g_TvTFile);

			pInfo->requiredItemId = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RequiredItemId"), 0, g_TvTFile);
			pInfo->requiredItemCount = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RequiredItemCount"), 0, g_TvTFile);

			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("BlueTeamLocation"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				pos = Utils::ReplaceString(pos, _T("/"), _T(" "), true);
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->bluePos.x >> pInfo->bluePos.y >> pInfo->bluePos.z;
			}else
			{
				pInfo->bluePos.x = 0;
				pInfo->bluePos.y = 0;
				pInfo->bluePos.z = 0;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("BlueTeamRespawnLocation"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				pos = Utils::ReplaceString(pos, _T("/"), _T(" "), true);
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->blueRespawnPos.x >> pInfo->blueRespawnPos.y >> pInfo->blueRespawnPos.z;
			}else
			{
				pInfo->blueRespawnPos.x = 0;
				pInfo->blueRespawnPos.y = 0;
				pInfo->blueRespawnPos.z = 0;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RedTeamLocation"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				pos = Utils::ReplaceString(pos, _T("/"), _T(" "), true);
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->redPos.x >> pInfo->redPos.y >> pInfo->redPos.z;
			}else
			{
				pInfo->redPos.x = 0;
				pInfo->redPos.y = 0;
				pInfo->redPos.z = 0;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RedTeamRespawnLocation"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				pos = Utils::ReplaceString(pos, _T("/"), _T(" "), true);
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->redRespawnPos.x >> pInfo->redRespawnPos.y >> pInfo->redRespawnPos.z;
			}else
			{
				pInfo->redRespawnPos.x = 0;
				pInfo->redRespawnPos.y = 0;
				pInfo->redRespawnPos.z = 0;
			}
			pInfo->randomizeTeam = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RandomizeTeam"), 0, g_TvTFile);
			pInfo->minPlayers = GetPrivateProfileInt(sectionEx.str().c_str(), _T("MinPlayers"), 0, g_TvTFile);
			pInfo->maxPlayers = GetPrivateProfileInt(sectionEx.str().c_str(), _T("MaxPlayers"), 0, g_TvTFile);
			pInfo->minLevel = GetPrivateProfileInt(sectionEx.str().c_str(), _T("MinLevel"), 0, g_TvTFile);
			pInfo->skillPchId = GetPrivateProfileInt(sectionEx.str().c_str(), _T("SkillPchId"), 0, g_TvTFile);
			pInfo->registerNpcClassId = GetPrivateProfileInt(sectionEx.str().c_str(), _T("RegisterNpcId"), 0, g_TvTFile);
			pInfo->inviteMessageId = GetPrivateProfileInt(sectionEx.str().c_str(), _T("InviteMessageId"), 0, g_TvTFile);
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RegisterNpcPos"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				pos = Utils::ReplaceString(pos, _T("/"), _T(" "), true);
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->registerNpcPos.x >> pInfo->registerNpcPos.y >> pInfo->registerNpcPos.z;
			}else
			{
				pInfo->kickOutPos.x = 0;
				pInfo->kickOutPos.y = 0;
				pInfo->kickOutPos.z = 0;
			}


			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("KickOutPos"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				pos = Utils::ReplaceString(pos, _T("/"), _T(" "), true);
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->kickOutPos.x >> pInfo->kickOutPos.y >> pInfo->kickOutPos.z;
			}else
			{
				pInfo->kickOutPos.x = 0;
				pInfo->kickOutPos.y = 0;
				pInfo->kickOutPos.z = 0;
			}

			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("FightZoneXRange"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->fightZoneX[0] >> pInfo->fightZoneX[1];
			}else
			{
				pInfo->fightZoneX[0] = 0;
				pInfo->fightZoneX[1] = 0;
			}

			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("FightZoneYRange"), 0, sTemp, 8192, g_TvTFile))
			{
				tstring pos = sTemp;
				tstringstream sstr;
				sstr << pos;
				sstr >> pInfo->fightZoneY[0] >> pInfo->fightZoneY[1];
			}else
			{
				pInfo->fightZoneY[0] = 0;
				pInfo->fightZoneY[1] = 0;
			}

			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("DoorName"), 0, sTemp, 8192, g_TvTFile))
			{
				tstringstream sstr;
				sstr << sTemp;
				wstring doorName;
				while(sstr >> doorName)
				{
					pInfo->doorList.push_back(doorName);
				}
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RegistrationStartMessage1"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->registrationStartMsg1 = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RegistrationStartMessage2"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->registrationStartMsg2 = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RegistrationRemindMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->registrationRemindMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RegistrationEndMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->registrationEndMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RegistratrionNontEnoughParticipants"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->registrationNotEnoughParticipantsMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("PreparationStartMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->preparationStartMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("PreparationRemindMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->preparationRemindMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("FightStartRemindMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->fightStartRemindMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("FightEndRemindMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->fightEndRemindMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("DrawMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->endMsg[0] = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("BlueWinMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->endMsg[1] = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("RedWinMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->endMsg[2] = sTemp;
			}if(GetPrivateProfileString(sectionEx.str().c_str(), _T("TeleportBackRemindMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->teleportBackRemindMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("EndMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->endEventMsg = sTemp;
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("FightStartMessage"), 0, sTemp, 8192, g_TvTFile))
			{
				pInfo->fightStartMessage = sTemp;
			}

			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("BlockedSkillList"), 0, sTemp, 0x8000, g_TvTFile))
			{
				tstringstream sstr;
				sstr << sTemp;
				INT32 skillId = 0;
				while(sstr >> skillId)
				{
					pInfo->blockedSkill.push_back(skillId);
				}
				pInfo->blockedSkill.sort();
				pInfo->blockedSkill.unique();
			}
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("BlockedItemList"), 0, sTemp, 0x8000, g_TvTFile))
			{
				tstringstream sstr;
				sstr << sTemp;
				INT32 itemId = 0;
				while(sstr >> itemId)
				{
					pInfo->blockedItem.push_back(itemId);
				}
				pInfo->blockedItem.sort();
				pInfo->blockedItem.unique();
			}
			memset(pInfo->blockedCrystalType, 0, sizeof(pInfo->blockedCrystalType));
			if(GetPrivateProfileString(sectionEx.str().c_str(), _T("BlockedCrystalTypeList"), 0, sTemp, 0x8000, g_TvTFile))
			{
				tstringstream sstr;
				sstr << sTemp;
				UINT crystalType = 0;
				while(sstr >> crystalType)
				{
					if(crystalType < 10)
					{
						pInfo->blockedCrystalType[crystalType] = true;
					}else
					{
						g_Log.Add(CLog::Error, "[%s] CrystalType overflow[%d]!", __FUNCTION__, crystalType);
					}
				}
			}

			pInfo->blockAugmentation = GetPrivateProfileInt(sectionEx.str().c_str(), _T("BlockAugmentation"), 0, g_TvTFile);
			pInfo->blockAttribute = GetPrivateProfileInt(sectionEx.str().c_str(), _T("BlockAttribute"), 0, g_TvTFile);
			pInfo->maxArmorEnchant = GetPrivateProfileInt(sectionEx.str().c_str(), _T("MaxArmorEnchant"), 0, g_TvTFile);
			pInfo->maxWeaponEnchant = GetPrivateProfileInt(sectionEx.str().c_str(), _T("MaxWeaponEnchant"), 0, g_TvTFile);

			m_tvtInfo.push_back(pInfo);
		}
	}
}

void CTvT::TimerExpired()
{
	guard;

	if(m_enabled)
	{
		Lock();
		if(m_lpEvent)
		{
			time_t currentTime = time(NULL);

			switch(m_lpEvent->m_state)
			{
			case TvT::StateNone:
				{
					m_lpEvent->Init();
				};
			case TvT::StateRegistration:
				{
					time_t diff = m_lpEvent->m_stateTime + m_lpEvent->m_lpInfo->registerTime;
					diff -= currentTime;
					if(diff > 0)
					{
						//broadcast message when it ends
						if(m_lpEvent->m_lpInfo->registrationRemindMsg.size() > 0)
						{
							if(diff == 60)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->registrationRemindMsg, _T("[param]"), remainSec);
								Utils::BroadcastToAllUser_Announce(announce.c_str());
							}else if(diff == 30)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->registrationRemindMsg, _T("[param]"), remainSec);
								Utils::BroadcastToAllUser_Announce(announce.c_str());
							}else if(diff == 10)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->registrationRemindMsg, _T("[param]"), remainSec);
								Utils::BroadcastToAllUser_Announce(announce.c_str());
							}
						}

					}else
					{
						m_lpEvent->m_stateTime = time(NULL);
						if(m_lpEvent->m_lpInfo->registrationEndMsg.size() > 0)
						{
							Utils::BroadcastToAllUser_Announce(m_lpEvent->m_lpInfo->registrationEndMsg.c_str());
						}

						//switch to next state if 
						if(m_lpEvent->m_lpInfo->minPlayers > 0 && m_lpEvent->m_lpInfo->minPlayers > m_lpEvent->m_users.size())
						{
							if(m_lpEvent->m_lpInfo->registrationNotEnoughParticipantsMsg.size() > 0)
							{
								Utils::BroadcastToAllUser_Announce(m_lpEvent->m_lpInfo->registrationNotEnoughParticipantsMsg.c_str());
							}
							m_lpEvent->m_state = TvT::StateDelete;
							break;
						}

						m_lpEvent->m_state = TvT::StatePreparation;
						if(m_lpEvent->m_lpInfo->preparationStartMsg.size() > 0)
						{
							Utils::BroadcastToAllUser_Announce(m_lpEvent->m_lpInfo->preparationStartMsg.c_str());
						}
						if(m_lpEvent->m_lpInfo->doorList.size() > 0)
						{
							//close doors
							for(list<wstring>::iterator Iter = m_lpEvent->m_lpInfo->doorList.begin(); Iter!= m_lpEvent->m_lpInfo->doorList.end();Iter++)
							{
								CDoor *pDoor = g_DoorDB.GetDoor(Iter->c_str());
								if(pDoor)
								{
									pDoor->Close();
								}
							}
						}

						if(m_lpEvent->m_lpInfo->kickOutPos.x != 0 || m_lpEvent->m_lpInfo->kickOutPos.y != 0)
						{
							g_UserDb.KickOutFromRange(m_lpEvent->m_lpInfo->kickOutPos, m_lpEvent->m_lpInfo->fightZoneX[0], m_lpEvent->m_lpInfo->fightZoneX[1], m_lpEvent->m_lpInfo->fightZoneY[0], m_lpEvent->m_lpInfo->fightZoneY[1] );
						}
					}
					break;
				}
			case TvT::StatePreparation:
				{
					time_t diff = m_lpEvent->m_stateTime + m_lpEvent->m_lpInfo->prepareTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(m_lpEvent->m_lpInfo->preparationRemindMsg.size() > 0)
						{
							if(diff == 30)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->preparationRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}else if(diff == 10)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->preparationRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
							else if(diff == 5)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->preparationRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
						}
					}else
					{
						m_lpEvent->m_stateTime = time(NULL);
						m_lpEvent->m_state = TvT::StateCountDown;

						//remove invalid players
						
						vector<UINT> toRemove;
						for(map<UINT, User*>::iterator Iter = m_lpEvent->m_users.begin(); Iter!=m_lpEvent->m_users.end();Iter++)
						{
							User *pUser = Iter->second;
							if(pUser->ValidUser())
							{
								if(pUser->GetInstantZone() == 0)
								{
									if(pUser->pSD->alive)
									{
										if(pUser->olympiadUser.status == 0)
										{
											ValidateItems(pUser);
											pUser->pEUD->tvtUser.status = TvT::UserPreparing;
											pUser->SendUserInfo();
											pUser->SendCharInfo();
										}else
										{
											pUser->pUserSocket->SendSystemMessage(L"You cannot participe in TvT when participing in Olympiad Games!");
											toRemove.push_back(Iter->first);
										}
									}else
									{
										pUser->pUserSocket->SendSystemMessage(L"Only alive players can participe in TvT event!");
										toRemove.push_back(Iter->first);
									}
								}else
								{
									pUser->pUserSocket->SendSystemMessage(L"You cannot participe in TvT event while being in instance zone!");
									toRemove.push_back(Iter->first);
								}
							}else
							{
								toRemove.push_back(Iter->first);
							}
						}

						for(int j=0;j<toRemove.size();j++)
						{
							map<UINT, User*>::iterator Iter = m_lpEvent->m_users.find(toRemove[j]);
							if(Iter != m_lpEvent->m_users.end())
							{
								if(User *pUser = Iter->second->CastUser())
								{
									OnExit(pUser, false);
								}else
								{
									m_lpEvent->m_users.erase(Iter);
								}
							}
						}
						//randomize team if needed
						if(m_lpEvent->m_lpInfo->randomizeTeam)
						{
							m_lpEvent->RandomizeTeams();
						}

						if(m_lpEvent->m_lpInfo->bluePos.x != 0 || m_lpEvent->m_lpInfo->bluePos.y != 0)
						{
							//port blue
							for(map<UINT, User*>::iterator Iter = m_lpEvent->m_users.begin(); Iter!=m_lpEvent->m_users.end();Iter++)
							{
								User *pUser = Iter->second;
								if(pUser->ValidUser() && pUser->pEUD->tvtUser.team == TvT::TeamBlue)
								{
									if(pUser->GetInstantZone() == 0)
									{
										pUser->pEUD->tvtUser.orgPos = pUser->pSD->pos;
										pUser->pEUD->tvtUser.fightPos = m_lpEvent->m_lpInfo->bluePos;
										if(m_lpEvent->m_lpInfo->dispelAbnormal)
										{
											pUser->DispelAllByGM();
											if(CSummon *pSummon = pUser->GetSummonOrPet())
											{
												pSummon->DispelAllByGM();
											}
										}
										if(m_lpEvent->m_lpInfo->blueRespawnPos.x != 0 || m_lpEvent->m_lpInfo->blueRespawnPos.y != 0)
										{
											pUser->pEUD->tvtUser.respawnPos = m_lpEvent->m_lpInfo->blueRespawnPos;
										}else
										{
											pUser->pEUD->tvtUser.respawnPos = m_lpEvent->m_lpInfo->bluePos;
										}
										pUser->TeleportToLocation(pUser->pEUD->tvtUser.respawnPos.x + g_Random.RandInt(5), pUser->pEUD->tvtUser.respawnPos.y + g_Random.RandInt(5), pUser->pEUD->tvtUser.respawnPos.z);
									}
								}
							}
						}
						if(m_lpEvent->m_lpInfo->redPos.x != 0 || m_lpEvent->m_lpInfo->redPos.y != 0)
						{
							//port red
							for(map<UINT, User*>::iterator Iter = m_lpEvent->m_users.begin(); Iter!=m_lpEvent->m_users.end();Iter++)
							{
								User *pUser = Iter->second;
								if(pUser->ValidUser() && pUser->pEUD->tvtUser.team == TvT::TeamRed)
								{
									if(pUser->GetInstantZone() == 0)
									{
										pUser->pEUD->tvtUser.orgPos = pUser->pSD->pos;
										pUser->pEUD->tvtUser.fightPos = m_lpEvent->m_lpInfo->redPos;
										if(m_lpEvent->m_lpInfo->dispelAbnormal)
										{
											pUser->DispelAllByGM();
											if(CSummon *pSummon = pUser->GetSummonOrPet())
											{
												pSummon->DispelAllByGM();
											}
										}
										if(m_lpEvent->m_lpInfo->redRespawnPos.x != 0 || m_lpEvent->m_lpInfo->redRespawnPos.y != 0)
										{
											pUser->pEUD->tvtUser.respawnPos = m_lpEvent->m_lpInfo->redRespawnPos;
										}else
										{
											pUser->pEUD->tvtUser.respawnPos = m_lpEvent->m_lpInfo->redPos;
										}
										pUser->TeleportToLocation(pUser->pEUD->tvtUser.respawnPos.x + g_Random.RandInt(5), pUser->pEUD->tvtUser.respawnPos.y + g_Random.RandInt(5), pUser->pEUD->tvtUser.respawnPos.z);
									}
								}
							}
						}

					}
					break;
				}
			case TvT::StateCountDown:
				{
					time_t diff = m_lpEvent->m_stateTime + m_lpEvent->m_lpInfo->countDownTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(m_lpEvent->m_lpInfo->fightStartRemindMsg.size() > 0)
						{
							if(diff == 30)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->fightStartRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}else if(diff == 10)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->fightStartRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
							else if(diff < 6)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->fightStartRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
						}
					}else
					{
						m_lpEvent->m_stateTime = time(NULL);
						m_lpEvent->m_state = TvT::StateFight;
						for(map<UINT, User*>::iterator Iter = m_lpEvent->m_users.begin(); Iter!=m_lpEvent->m_users.end();Iter++)
						{
							User *pUser = Iter->second;
							if(pUser->ValidUser() && pUser->pSD->alive)
							{
								pUser->pEUD->tvtUser.status = TvT::UserFighting;
								if(pUser->pEUD->tvtUser.fightPos != pUser->pEUD->tvtUser.respawnPos)
								{
									pUser->TeleportToLocation(pUser->pEUD->tvtUser.fightPos.x + g_Random.RandInt(5), pUser->pEUD->tvtUser.fightPos.y + g_Random.RandInt(5), pUser->pEUD->tvtUser.fightPos.z);
								}
							}
						}
						m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", m_lpEvent->m_lpInfo->fightStartMessage.c_str());
					}
					break;
				}
			case TvT::StateFight:
				{
					time_t diff = m_lpEvent->m_stateTime + m_lpEvent->m_lpInfo->fightTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(m_lpEvent->m_lpInfo->fightEndRemindMsg.size() > 0)
						{
							if(diff == 30)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->fightEndRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}else if(diff == 10)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->fightEndRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
							else if(diff < 6)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->fightEndRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
							if(m_lpEvent->ValidateWinner())
							{
								m_lpEvent->OnFinish();
							}
						}
					}else
					{
						m_lpEvent->ValidateWinner(true);
						m_lpEvent->OnFinish();
					}
					break;
				}
			case TvT::StateFinish:
				{
					time_t diff = m_lpEvent->m_stateTime + m_lpEvent->m_lpInfo->finishTime;
					diff -= currentTime;
					if(diff > 0)
					{
						if(m_lpEvent->m_lpInfo->teleportBackRemindMsg.size() > 0)
						{
							if(diff == 10)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->teleportBackRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}else if(diff < 6)
							{
								TCHAR remainSec[16];
								_itot_s(static_cast<int>(diff), remainSec, 16, 10);
								wstring announce = Utils::ReplaceString(m_lpEvent->m_lpInfo->teleportBackRemindMsg, _T("[param]"), remainSec);
								m_lpEvent->Broadcast("cddSS", 0x4A, 0, 18, L"", announce.c_str());
							}
						}
					}else
					{
						m_lpEvent->m_stateTime = time(NULL);
						m_lpEvent->m_state = TvT::StateDelete;
					}
					break;
				}
			case TvT::StateDelete:
				{
					m_lpEvent->OnDelete();
					delete m_lpEvent;
					m_lpEvent = 0;
					break;
				};
			};
		}else
		{
			tm ti; time_t rawtime;
			time ( &rawtime );
			localtime_s(&ti, &rawtime );

			for(list<TvT::TvTInfo*>::iterator Iter = m_tvtInfo.begin();Iter!=m_tvtInfo.end();Iter++)
			{
				TvT::TvTInfo *pInfo = (*Iter);
				if(pInfo)
				{
					if(pInfo->enabled)
					{
						if(pInfo->dayFlag[ti.tm_wday])
						{
							if(pInfo->startHour == ti.tm_hour && pInfo->startMinute == ti.tm_min)
							{
								m_lpEvent = new CMatch(pInfo);
								break;
							}
						}
					}
				}
			}
		}
		Unlock();
	}
	unguard;
}

void CTvT::Unregister(User *pUser)
{
	guard;

	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pEUD->tvtUser.status != TvT::UserNone)
			{
				Lock();
				if(m_lpEvent)
				{
					if(m_lpEvent->m_state == TvT::StateRegistration)
					{
						map<UINT, User*>::iterator Iter = m_lpEvent->m_users.find(pUser->serverId);
						if(Iter!= m_lpEvent->m_users.end())
						{
							m_lpEvent->m_users.erase(Iter);
							pUser->pUserSocket->SendSystemMessage(L"You've been unregistered from TvT event.");
						}
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot unregister from TvT event at the moment.");
					}
				}
				Unlock();
				pUser->pEUD->tvtUser.status = TvT::UserNone;
				pUser->pEUD->tvtUser.team = TvT::TeamNone;
				pUser->pEUD->tvtUser.orgPos.x = 0;
				pUser->pEUD->tvtUser.orgPos.y = 0;
				pUser->pEUD->tvtUser.orgPos.z = 0;
			}
		}
	}
	unguard;
}

void CTvT::Register(User *pUser, int team)
{
	guard;

	if(m_enabled)
	{
		if(pUser->pEUD->tvtUser.status == TvT::UserNone)
		{
			Lock();
			if(m_lpEvent)
			{
				if(m_lpEvent->m_state == TvT::StateRegistration)
				{
					if(pUser->pSD->level >= m_lpEvent->m_lpInfo->minLevel)
					{
						if(m_lpEvent->m_users.size() < m_lpEvent->m_lpInfo->maxPlayers)
						{
							if(pUser->olympiadUser.status == 0 )
							{
								if(pUser->GetInstantZone() == 0)
								{
									if(pUser->pSD->alive)
									{
										if(m_lpEvent->m_lpInfo->hardwareIdLimit == 0 || (m_lpEvent->m_lpInfo->hardwareIdLimit > g_UserDb.GetTvTUsersByHWID(pUser->pUserSocket->hardwareId.hash)) )
										{
											if(pUser->olympiadUser.status == 0)
											{
												if(m_lpEvent->m_lpInfo->requiredItemId == 0 || pUser->inventory.HaveItemByClassId(m_lpEvent->m_lpInfo->requiredItemId, m_lpEvent->m_lpInfo->requiredItemCount))
												{	
													if(m_lpEvent->m_lpInfo->requiredItemId > 0)
													{
														pUser->DeleteItemInInventory(m_lpEvent->m_lpInfo->requiredItemId, m_lpEvent->m_lpInfo->requiredItemCount);
													}

													if(team == 1 || team == 2)
													{
														pUser->pEUD->tvtUser.status = TvT::UserRegistered;
														if(team == 1)
															pUser->pEUD->tvtUser.team = TvT::TeamBlue;
														else
															pUser->pEUD->tvtUser.team = TvT::TeamRed;
														pUser->pEUD->tvtUser.friendlyFire = m_lpEvent->m_lpInfo->friendlyFire;

														m_lpEvent->m_users.insert(pair<UINT, User*>(pUser->serverId, pUser));
														pUser->pUserSocket->SendSystemMessage(L"You've been registered in TvT event.");
													}else
													{
														g_Log.Add(CLog::Error, "[%s] Invalid team[%d] - user[%S]", __FUNCTION__, team, pUser->pSD->wName);
													}
												}else
												{
													pUser->pUserSocket->SendSystemMessage(L"You don't have enough required item(s) to participe in TvT event.");
												}
											}else
											{
												pUser->pUserSocket->SendSystemMessage(L"You cannot register in TvT when participing in Olympiad Games!");
											}
										}else
										{
											pUser->pUserSocket->SendSystemMessage(L"You cannot register any more characters in TvT - reached the limit.");
										}
									}else
									{
										pUser->pUserSocket->SendSystemMessage(L"Only alive players can participe in TvT event.");
									}
								}else
								{
									pUser->pUserSocket->SendSystemMessage(L"You cannot participe in TvT event from instance zone.");
								}
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"You cannot participe in TvT event when participing in olympiad fights.");
							}
						}else
						{
							pUser->pUserSocket->SendSystemMessage(L"There are no empty slots in TvT event, please come back later.");
						}
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"Your level is too low to participe in TvT event.");
					}
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot participe in TvT event at the moment.");
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot participe in TvT event at the moment.");
			}
			Unlock();
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"You already are participing in TvT event.");
		}
	}
	unguard;
}

void CTvT::OnDie(User *pUser)
{
	guard;
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pEUD->tvtUser.status == TvT::UserFighting)
			{
				Lock();
				if(m_lpEvent->m_lpInfo->type == TvT::TvTLastTeamStanding)
				{
					pUser->pEUD->tvtUser.status = TvT::UserFinishing;
					if(m_lpEvent->ValidateWinner())
					{
						m_lpEvent->OnFinish();
					}
				}else if(m_lpEvent->m_lpInfo->type == TvT::TvTDeathMatch)
				{
					if(pUser->pEUD->tvtUser.team == TvT::TeamBlue)
					{
						m_lpEvent->m_killCountRed++;
					}else if(pUser->pEUD->tvtUser.team == TvT::TeamRed)
					{
						m_lpEvent->m_killCountBlue++;
					}
					pUser->pEUD->tvtUser.respawnTick = m_lpEvent->m_lpInfo->respawnTime;
				}
				Unlock();
			}
		}
	}
	unguard;
}

void CTvT::OnExit(User *pUser, bool useLock)
{
	guard;

	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pEUD->tvtUser.status != TvT::UserNone)
			{
				if(useLock)
					Lock();

				if(m_lpEvent)
				{
					map<UINT, User*>::iterator Iter = m_lpEvent->m_users.find(pUser->serverId);
					if(Iter!=m_lpEvent->m_users.end())
					{
						m_lpEvent->m_users.erase(Iter);
					}
					if(m_lpEvent->m_lpInfo->type == TvT::TvTLastTeamStanding)
					{
						if(m_lpEvent->ValidateWinner())
						{
							m_lpEvent->OnFinish();
						}
					}
				}
				if(useLock)
					Unlock();

				pUser->pEUD->tvtUser.Clear();
			}
		}
	}
	unguard;
}

void CTvT::OnNpcSpawn(CNpc *pNpc)
{
	guard;

	if(m_enabled)
	{
		if(pNpc)
		{
			Lock();
			if(m_lpEvent)
			{
				if(m_lpEvent->m_lpInfo->registerNpcClassId)
				{
					if(pNpc->pSD->npc_class_id == m_lpEvent->m_lpInfo->registerNpcClassId)
					{
						if(m_lpEvent->m_managerServerId == 0)
						{
							m_lpEvent->m_managerServerId = pNpc->serverId;
						}
					}
				}
			}
			Unlock();
		}
	}
	unguard;
}

void CTvT::TimerExpired(User *pUser)
{
	guard;

	if(m_enabled && m_lpEvent)
	{
		if((pUser->pEUD->tvtUser.status == TvT::UserPreparing || pUser->pEUD->tvtUser.status == TvT::UserFinishing) && pUser->pSD->karma == 0)
		{
			if(m_lpEvent->m_lpInfo->fightZoneX[0] <= (INT32)pUser->pSD->pos.x && m_lpEvent->m_lpInfo->fightZoneX[1] >= (INT32)pUser->pSD->pos.x
				&& m_lpEvent->m_lpInfo->fightZoneY[0] <= (INT32)pUser->pSD->pos.y && m_lpEvent->m_lpInfo->fightZoneY[1] >= (INT32)pUser->pSD->pos.y)
			{
				pUser->inPeaceZone = true;
			}
		}
		if(pUser->pEUD->tvtUser.status == TvT::UserFighting)
		{
			//check loc
			if(m_lpEvent->m_lpInfo->fightZoneX[0] > (INT32)pUser->pSD->pos.x || m_lpEvent->m_lpInfo->fightZoneX[1] < (INT32)pUser->pSD->pos.x
				|| m_lpEvent->m_lpInfo->fightZoneY[0] > (INT32)pUser->pSD->pos.y || m_lpEvent->m_lpInfo->fightZoneY[1] < (INT32)pUser->pSD->pos.y)
			{
				if(pUser->pEUD->tvtUser.resetTimer == 0)
				{
					pUser->pUserSocket->SendSystemMessage(L"You're outside combat zone, you'll be expelled from TvT event in 15 seconds.");
					pUser->pEUD->tvtUser.resetTimer = 23;
				}

				pUser->pEUD->tvtUser.resetTimer--;
				if(pUser->pEUD->tvtUser.resetTimer == 0)
				{
					pUser->pUserSocket->SendSystemMessage(L"You're outside combat zone, you'll be expelled from TvT event.");
					OnExit(pUser);
					unguard;
					return;
				}
			}else if(pUser->pEUD->tvtUser.resetTimer != 0)
			{
				pUser->pEUD->tvtUser.resetTimer = 0;
			}

			if(pUser->pEUD->tvtUser.respawnTick > 0)
			{
				if(!pUser->pSD->alive)
				{
					pUser->pEUD->tvtUser.respawnTick--;
					if(pUser->pEUD->tvtUser.respawnTick != 0)
					{
						CSystemMessage msg(3034);	//You'll be respawned in $s1 second(s)!
						msg.AddNumber(pUser->pEUD->tvtUser.respawnTick);
						pUser->pUserSocket->SendSystemMessage(msg);
					}else
					{
						pUser->ResurrectedBy(pUser, 100);
						if(pUser->pEUD->tvtUser.respawnPos.x != 0 || pUser->pEUD->tvtUser.respawnPos.y != 0)
						{
							pUser->TeleportToLocation(pUser->pEUD->tvtUser.respawnPos.x, pUser->pEUD->tvtUser.respawnPos.y, pUser->pEUD->tvtUser.respawnPos.z);
							pUser->pEUD->tvtUser.backToFightTick = m_lpEvent->m_lpInfo->backToFightTime;
						}
						//add custom buffv
					}
				}else
				{
					pUser->pEUD->tvtUser.respawnTick = 0;
				}
			}else if(pUser->pEUD->tvtUser.backToFightTick > 0)
			{
				pUser->pEUD->tvtUser.backToFightTick--;
				if(pUser->pEUD->tvtUser.backToFightTick != 0)
				{
					//3036	1	a,You'll be ported back in $s1 second(s)!\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
					CSystemMessage msg(3036);
					msg.AddNumber(pUser->pEUD->tvtUser.backToFightTick);
					pUser->pUserSocket->SendSystemMessage(msg);
				}else
				{
					if(pUser->pEUD->tvtUser.fightPos.x != 0 || pUser->pEUD->tvtUser.fightPos.y != 0)
					{
						pUser->TeleportToLocation(pUser->pEUD->tvtUser.fightPos.x, pUser->pEUD->tvtUser.fightPos.y, pUser->pEUD->tvtUser.fightPos.z);
					}
				}
			}
		}
	}
	unguard;
}

bool CTvT::CanUse(CItem *pItem)
{
	guard;

	if(m_enabled && m_lpEvent)
	{
		if(m_lpEvent->m_state == TvT::StateCountDown || m_lpEvent->m_state == TvT::StateFight)
		{
			for(list<INT32>::iterator Iter = m_lpEvent->m_lpInfo->blockedItem.begin(); Iter!=m_lpEvent->m_lpInfo->blockedItem.end();Iter++)
			{
				if((*Iter) == pItem->pSID->itemId)
				{
					unguard;
					return false;
				}
			}

			if(pItem->pII->crystalType < 10)
			{
				if(m_lpEvent->m_lpInfo->blockedCrystalType[pItem->pII->crystalType])
				{
					unguard;
					return false;
				}
			}

			if(m_lpEvent->m_lpInfo->blockAugmentation && pItem->pSID->isAugmented)
			{
				unguard;
				return false;
			}

			if(m_lpEvent->m_lpInfo->blockAttribute)
			{
				if(pItem->attribute.attackAttribute != static_cast<USHORT>(-2) || pItem->attribute.defAttribute[0] != 0 || pItem->attribute.defAttribute[1] != 0 || pItem->attribute.defAttribute[2] != 0 
					|| pItem->attribute.defAttribute[3] != 0 || pItem->attribute.defAttribute[4] != 0 || pItem->attribute.defAttribute[5] != 0 )
				{
					unguard;
					return false;
				}						
			}

			if(pItem->IsWeapon() && m_lpEvent->m_lpInfo->maxWeaponEnchant > 0)
			{
				if(pItem->pII->weaponType == WeaponDual)
				{
					if( pItem->pSID->enchantLevel > (m_lpEvent->m_lpInfo->maxWeaponEnchant + 1) )
					{
						unguard;
						return false;
					}
				}else
				{
					if(pItem->pSID->enchantLevel > m_lpEvent->m_lpInfo->maxWeaponEnchant)
					{
						unguard;
						return false;
					}
				}
			}else if(m_lpEvent->m_lpInfo->maxArmorEnchant > 0)
			{
				if(pItem->pSID->slotType == SlotOnePiece)
				{
					if( pItem->pSID->enchantLevel > (m_lpEvent->m_lpInfo->maxWeaponEnchant + 1) )
					{
						unguard;
						return false;
					}
				}else
				{
					if( pItem->pSID->enchantLevel > m_lpEvent->m_lpInfo->maxWeaponEnchant )
					{
						unguard;
						return false;
					}
				}
			}
		}
	}

	unguard;

	return true;
}

void CTvT::ValidateItems(User *pUser)
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
					if(!CanUse(pItem))
					{
						pUser->UnequipItem(pItem);
						continue;
					}
				}
			}
		}
	}
	unguard;
}

bool CTvT::IsSkillBlocked(INT32 skillId)
{
	guard;

	if(m_enabled && m_lpEvent)
	{
		if(m_lpEvent->m_state == TvT::StateCountDown || m_lpEvent->m_state == TvT::StateFight)
		{
			for(list<INT32>::iterator Iter = m_lpEvent->m_lpInfo->blockedSkill.begin(); Iter!=m_lpEvent->m_lpInfo->blockedSkill.end();Iter++)
			{
				if((*Iter) == skillId)
				{
					unguard;
					return true;
				}
			}
		}
	}
	unguard;
	return false;
}

bool CTvT::HandleReply(User *pUser, UINT msgId, UINT reply)
{
	guard;
	bool ret = false;

	if(m_enabled)
	{
		for(UINT n=0;n<m_inviteMessageId.size();n++)
		{
			if(m_inviteMessageId[n] == msgId)
			{
				if(reply)
				{
					Register(pUser, 0);	//randomize
				}
				ret = true;
				break;
			}
		}

	}

	unguard;
	return ret;
}