#include "stdafx.h"
#include "FactionSystem.h"

CFactionSystem g_FactionSystem;

CFactionSystem::CFactionSystem() : enabled(FALSE)
{
	SetName(L"Faction System");
}

CFactionSystem::~CFactionSystem()
{

}

void CFactionSystem::Init()
{
	LoadData();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Loaded [%d] faction(s).", GetName(), lConfig.size());
	}
}

void CFactionSystem::LoadData()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//FactionData.txt", lpData);
	if(lpData && loaded > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		wstring line;
		while(getline(file, line))
		{
			if(line.size() > 4)
			{
				if( line[0] == L'/' || line[0] == L';' )
					continue;

				if( line.find(L"setting_begin") == 0 )
				{
					enabled = ParseOptionInt(line, L"enabled", 0);
					
					continue;
				}
				if( line.find(L"faction_begin") == 0 )
				{
					FactionConfig fc;
					//faction_begin type=FactionBoss	npc_class_id=1029014	messageId=3018	min_player_count=2	max_player_count=100	teleport_delay=300	WaitPosBlue=0/0/0	WaitPosRed=0/0/0	StartPosBlue=0/0/0	StartPosRed=0/0/0	faction_end
					wstring sType = ParseOptionString(line, L"type", L"FactionNone");
					if(sType == L"FactionBoss")
						fc.type = FactionBoss;
					else
						fc.type = FactionNone;
					fc.npcClassId = ParseOptionInt(line, L"npc_class_id", 0);
					fc.messageId = ParseOptionInt(line, L"messageId", 0);
					fc.minPlayerCount = ParseOptionInt(line, L"min_player_count", 0);
					fc.maxPlayerCount = ParseOptionInt(line, L"max_player_count", 0);
					fc.minPlayerLevel = ParseOptionInt(line, L"min_player_level", 0);
					fc.waitTime = ParseOptionInt(line, L"wait_time", 0);
					fc.maxTime = ParseOptionInt(line, L"max_time", 0);
					fc.rewardId = ParseOptionInt(line, L"reward_item_id", 0);
					fc.rewardCount = ParseOptionInt(line, L"reward_item_count", 0);
					fc.closeColiseum = ParseOptionInt(line, L"close_coliseum", 0);

					wstring sPos = ParseOptionString(line, L"WaitPosBlue", L"0/0/0");
					sPos = ReplaceChar(sPos, L"/", L" ");
					{
						wstringstream sstr;
						sstr << sPos;
						sstr >> fc.waitPosBlue.x >> fc.waitPosBlue.y >> fc.waitPosBlue.z;
					}
					sPos = ParseOptionString(line, L"WaitPosRed", L"0/0/0");
					sPos = ReplaceChar(sPos, L"/", L" ");
					{
						wstringstream sstr;
						sstr << sPos;
						sstr >> fc.waitPosRed.x >> fc.waitPosRed.y >> fc.waitPosRed.z;
					}
					sPos = ParseOptionString(line, L"StartPosBlue", L"0/0/0");
					sPos = ReplaceChar(sPos, L"/", L" ");
					{
						wstringstream sstr;
						sstr << sPos;
						sstr >> fc.startPosBlue.x >> fc.startPosBlue.y >> fc.startPosBlue.z;
					}
					sPos = ParseOptionString(line, L"StartPosRed", L"0/0/0");
					sPos = ReplaceChar(sPos, L"/", L" ");
					{
						wstringstream sstr;
						sstr << sPos;
						sstr >> fc.startPosRed.x >> fc.startPosRed.y >> fc.startPosRed.z;
					}
					if(fc.type != FactionNone)
					{
						lConfig.push_back(fc);
					}
				}
			}
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//FactionData.txt !", GetName());
	}
}

void CFactionSystem::OnNpcDie(CNpc *pNpc, CCreature *pEnemy)
{
	guard;
	if(enabled)
	{
		if(pNpc->ValidCreature())
		{
			Lock();
			for(map<UINT, CFaction*>::iterator Iter = mFaction.begin();Iter!=mFaction.end();Iter++)
			{
				if(CBossFaction *pFaction = Iter->second->CastBossFaction())
				{
					if(pNpc->pSD->npc_class_id == pFaction->bossClassId && pNpc->serverId == pFaction->bossServerId)
					{
						pFaction->OnNpcDie(pNpc, pEnemy);
					}
				}
			}
			Unlock();
		}
	}
	unguard;
}

void CFactionSystem::OnEnterWorld(CNpc *pNpc)
{
	guard;

	if(enabled)
	{
		if(pNpc->ValidCreature())
		{
			//check if there's any boss faction with this npc class id
			for(list<FactionConfig>::iterator Iter = lConfig.begin();Iter!=lConfig.end();Iter++)
			{
				if(Iter->type == FactionBoss)
				{
					if(Iter->npcClassId == pNpc->pSD->npc_class_id)
					{
						//create boss faction
						CBossFaction *pFaction = new CBossFaction();
						pFaction->minPlayerLevel = Iter->minPlayerLevel;
						pFaction->minPlayerCount = Iter->minPlayerCount;
						pFaction->maxPlayerCount = Iter->maxPlayerCount;
						pFaction->Init(Iter->npcClassId, pNpc->serverId, Iter->messageId, Iter->waitTime, Iter->maxTime, Iter->waitPosBlue, Iter->waitPosRed, Iter->startPosBlue, Iter->startPosRed, Iter->rewardId, Iter->rewardCount, Iter->closeColiseum);
						Lock();
						mFaction.insert(pair<UINT, CFaction*>(pFaction->GetId(), pFaction));
						Unlock();
					}
				}
			}
		}
	}
	unguard;
}

void CFactionSystem::OnEnterWorld(User *pUser)
{
	guard;

	if(enabled)
	{
		if(pUser->ValidUser())
		{
			Lock();
			for(map<UINT, CFaction*>::iterator Iter = mFaction.begin();Iter!=mFaction.end();Iter++)
			{
				if(CBossFaction *pFaction = Iter->second->CastBossFaction())
				{
					pFaction->EnterWorld(pUser);
				}
			}
			Unlock();
		}
	}
	unguard;
}

void CFactionSystem::OnLeaveWorld(User *pUser)
{
	guard;

	if(enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pEUD->faction.factionId > 0)
			{
				Lock();
				for(map<UINT, CFaction*>::iterator Iter = mFaction.begin();Iter!=mFaction.end();Iter++)
				{
					if(pUser->pEUD->faction.factionId == Iter->second->GetId())
					{
						if(CBossFaction *pFaction = Iter->second->CastBossFaction())
						{
							pFaction->LeaveWorld(pUser);
						}
					}
				}
				Unlock();
			}
		}
	}
	unguard;
}

bool CFactionSystem::HandleReply(User *pUser, UINT messageId, UINT answer)
{
	guard;

	if(enabled)
	{
		if(pUser->ValidUser())
		{
			for(list<FactionConfig>::iterator Iter = lConfig.begin();Iter!=lConfig.end();Iter++)
			{
				if(Iter->messageId == messageId)
				{
					if(answer)
					{
						if(pUser->olympiadUser.status == 0)
						{
							if(pUser->pSD->alive)
							{
								if(pUser->pEUD->faction.factionId == 0)
								{
									Lock();
									for(map<UINT, CFaction*>::iterator factionIter = mFaction.begin(); factionIter!=mFaction.end();factionIter++)
									{
										if(factionIter->second->askMessageId == messageId)
										{
											if(!factionIter->second->started && !factionIter->second->deleteWaiting)
											{
												if(factionIter->second->AddUser(pUser, TeamNone))
												{
													if(CBossFaction *pBossFaction = factionIter->second->CastBossFaction())
													{
														pBossFaction->HandleNewUser(pUser);
													}
												}
											}else
											{
												pUser->pUserSocket->SendSystemMessage(GetName(), L"You cannot participe in this faction couse the fight already started.");
											}
										}
									}
									Unlock();
								}else
								{
									pUser->pUserSocket->SendSystemMessage(GetName(), L"You already are assigned to a faction.");
								}
							}else
							{
								pUser->pUserSocket->SendSystemMessage(GetName(), L"You have to be alive to participe in a faction event.");
							}
						}else
						{
							pUser->pUserSocket->SendSystemMessage(GetName(), L"You cannot participe in a faction event while participing in Olympiad Fights.");
						}
					}
					unguard;
					return true;
				}
			}
		}
	}
	unguard;
	return false;
}

void CFactionSystem::TimerExpired()
{
	guard;

	if(enabled)
	{
		vector<UINT> toDelete;
		Lock();
		for(map<UINT, CFaction*>::iterator Iter = mFaction.begin();Iter!=mFaction.end();Iter++)
		{
			if(Iter->second->deleteWaiting)
			{
				toDelete.push_back(Iter->first);
				continue;
			}else
			{
				if(CBossFaction *pFaction = Iter->second->CastBossFaction())
				{
					pFaction->TimerExpired();
				}
			}
		}
		if(toDelete.size() > 0)
		{
			for(vector<UINT>::iterator Iter = toDelete.begin();Iter!=toDelete.end();Iter++)
			{
				map<UINT, CFaction*>::iterator mIter = mFaction.find((*Iter));
				if(mIter!=mFaction.end())
				{
					CFaction *pFaction = mIter->second;
					mIter->second = 0;
					mFaction.erase(mIter);
					if(CBossFaction *pBossFaction = pFaction->CastBossFaction())
					{
						pBossFaction->End();
						delete pBossFaction;
					}
				}
			}
		}
		Unlock();
	}
	unguard;
}