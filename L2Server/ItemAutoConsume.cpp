#include "stdafx.h"
#include "ItemAutoConsume.h"
#include "Championship.h"

CItemAutoConsume g_ItemAutoConsume;

CItemAutoConsume::CItemAutoConsume() : m_enabled(FALSE)
{
	SetName(L"Item Auto Consume");
}

CItemAutoConsume::~CItemAutoConsume()
{
}

void CItemAutoConsume::Init()
{
	m_enabled = FALSE;
	m_items.clear();
	LoadData();
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Loaded [%d] items.", GetName(), m_items.size());
	}
}

void CItemAutoConsume::LoadData()
{	
	guard;
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//ItemAutoConsume.txt", lpData);
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
					m_enabled = ParseOptionInt(line, L"enabled", 0);
					
					continue;
				}
				if( line.find(L"item_begin") == 0 )
				{
					AutoConsumeInfo* pAci = new AutoConsumeInfo();
//item_begin	item_id=20424	consume_type=hp	activate_on_missing_percent=30	activate_on_missing_fixed=500	reuse_delay=800	required_level=60	can_use_in_olympiad=0	item_end					
					pAci->itemId = ParseOptionInt(line, L"item_id");
					wstring sType = ParseOptionString(line, L"consume_type", L"None");
					if(sType == L"hp")
						pAci->type = AutoConsumeHp;
					else if(sType == L"mp")
						pAci->type = AutoConsumeMp;
					else if(sType == L"cp")
						pAci->type = AutoConsumeCp;
					else
						pAci->type = AutoConsumeNone;
					pAci->activateOnMissingPercent = static_cast<double>(ParseOptionInt(line, L"activate_on_missing_percent", INT_MAX));
					pAci->activateOnMissingFixed = static_cast<double>(ParseOptionInt(line, L"activate_on_missing_fixed", INT_MAX));
					pAci->reuseDelay = ParseOptionInt(line, L"reuse_delay", 0);
					pAci->requiredLevel = ParseOptionInt(line, L"required_level", 0);
					if(pAci->itemId > 0 && pAci->type != AutoConsumeNone)
					{
						m_items.insert(pair<INT32, AutoConsumeInfo*>(pAci->itemId, pAci));
					}
				}
			}
		}
		delete [] lpData;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//ItemAutoConsume.txt !", GetName());
	}
	unguard;
}

BOOL CItemAutoConsume::IsAutoConsume(INT32 itemId)
{
	guard;
	if(m_enabled)
	{
		map<INT32, AutoConsumeInfo*>::iterator Iter = m_items.find(itemId);
		if(Iter!=m_items.end())
		{
			unguard;
			return true;
		}
	}
	unguard;
	return false;
}

void CItemAutoConsume::TurnOn(User *pUser, INT32 itemId)
{
	guard;
	if(m_enabled)
	{
		if(pUser->ValidUser() && pUser->pSD->alive)
		{
			if(pUser->IsNowTrade())
				pUser->TradeCancel();

			map<INT32, AutoConsumeInfo*>::iterator Iter = m_items.find(itemId);
			if(Iter!=m_items.end())
			{
				if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(itemId))
				{
					AutoConsumeInfo *pInfo = Iter->second;
					if(pInfo->canUseInOlympiad)
					{
						if(pUser->olympiadUser.IsInside())
						{
							pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action while fighting in olympiad.");
							unguard;
							return;
						}
					}
					if(!pUser->CanUseItem(pItem))
					{
						unguard;
						return;
					}
					
					if(pInfo->requiredLevel <= pUser->pSD->level)
					{
						pUser->pEUD->Lock();
						if(pInfo->type == AutoConsumeHp)
						{
							if(pUser->pEUD->autoConsumeHp.itemId)
							{
								TurnOff(pUser, pUser->pEUD->autoConsumeHp.itemId, FALSE);
							}
							pUser->pEUD->autoConsumeHp.itemId = itemId;
							pUser->pEUD->autoConsumeHp.lastUseTick = 0;
							pUser->pEUD->autoConsumeHp.pInfo = pInfo;
							
						}else if(pInfo->type == AutoConsumeMp)
						{
							if(pUser->pEUD->autoConsumeMp.itemId)
							{
								TurnOff(pUser, pUser->pEUD->autoConsumeMp.itemId, FALSE);
							}
							pUser->pEUD->autoConsumeMp.itemId = itemId;
							pUser->pEUD->autoConsumeMp.lastUseTick = 0;
							pUser->pEUD->autoConsumeMp.pInfo = pInfo;
							
						}else if(pInfo->type == AutoConsumeCp)
						{
							if(pUser->pEUD->autoConsumeCp.itemId)
							{
								TurnOff(pUser, pUser->pEUD->autoConsumeCp.itemId, FALSE);
							}
							pUser->pEUD->autoConsumeCp.itemId = itemId;
							pUser->pEUD->autoConsumeCp.lastUseTick = 0;
							pUser->pEUD->autoConsumeCp.pInfo = pInfo;
							
						}
						pUser->pEUD->Unlock();
						pUser->pUserSocket->Send("chdd", 0xFE, 0xC, itemId, 1);
						CSystemMessage msg(1433);
						msg.AddItem(itemId);
						pUser->pUserSocket->SendSystemMessage(msg);
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"Your level is too low to perform this action.");
					}
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Not registered item[%d] - user[%S] !", __FUNCTION__, itemId, pUser->pSD->wName);
			}
		}
	}
	unguard;
}

void CItemAutoConsume::TurnOff(User *pUser, INT32 itemId, BOOL useLock, BOOL outOfPots)
{
	guard;

	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(itemId == 0)
			{
				if(useLock)
					pUser->pEUD->Lock();

				if(pUser->pEUD->autoConsumeHp.itemId > 0)
				{
					pUser->pUserSocket->Send("chdd", 0xFE, 0xC, pUser->pEUD->autoConsumeHp.itemId, 0);
					pUser->pEUD->autoConsumeHp.itemId = 0;
					pUser->pEUD->autoConsumeHp.lastUseTick = 0;
					pUser->pEUD->autoConsumeHp.pInfo = 0;
				}

				if(pUser->pEUD->autoConsumeMp.itemId > 0)
				{
					pUser->pUserSocket->Send("chdd", 0xFE, 0xC, pUser->pEUD->autoConsumeMp.itemId, 0);
					pUser->pEUD->autoConsumeMp.itemId = 0;
					pUser->pEUD->autoConsumeMp.lastUseTick = 0;
					pUser->pEUD->autoConsumeMp.pInfo = 0;
				}

				if(pUser->pEUD->autoConsumeCp.itemId > 0)
				{
					pUser->pUserSocket->Send("chdd", 0xFE, 0xC, pUser->pEUD->autoConsumeCp.itemId, 0);
					pUser->pEUD->autoConsumeCp.itemId = 0;
					pUser->pEUD->autoConsumeCp.lastUseTick = 0;
					pUser->pEUD->autoConsumeCp.pInfo = 0;
				}

				if(useLock)
					pUser->pEUD->Unlock();
				unguard;
				return;
			}

			map<INT32, AutoConsumeInfo*>::iterator Iter = m_items.find(itemId);
			if(Iter!=m_items.end())
			{
				AutoConsumeInfo *pInfo = Iter->second;
				
				if(useLock)
					pUser->pEUD->Lock();

				if(pInfo->type == AutoConsumeHp)
				{
					if(pUser->pEUD->autoConsumeHp.itemId == itemId)
					{
						pUser->pEUD->autoConsumeHp.itemId = 0;
						pUser->pEUD->autoConsumeHp.lastUseTick = 0;
						pUser->pEUD->autoConsumeHp.pInfo = 0;
					}					
				}else if(pInfo->type == AutoConsumeMp)
				{
					if(pUser->pEUD->autoConsumeMp.itemId == itemId)
					{
						pUser->pEUD->autoConsumeMp.itemId = 0;
						pUser->pEUD->autoConsumeMp.lastUseTick = 0;
						pUser->pEUD->autoConsumeMp.pInfo = 0;
					}					
				}else if(pInfo->type == AutoConsumeCp)
				{
					if(pUser->pEUD->autoConsumeCp.itemId == itemId)
					{
						pUser->pEUD->autoConsumeCp.itemId = 0;
						pUser->pEUD->autoConsumeCp.lastUseTick = 0;
						pUser->pEUD->autoConsumeCp.pInfo = 0;
					}					
				}
				
				if(useLock)
					pUser->pEUD->Unlock();

				pUser->pUserSocket->Send("chdd", 0xFE, 0xC, itemId, 0);

				if(outOfPots)
				{
					CSystemMessage msg(1435);
					msg.AddItem(itemId);
					pUser->pUserSocket->SendSystemMessage(msg);
				}else
				{
					CSystemMessage msg(1434);
					msg.AddItem(itemId);
					pUser->pUserSocket->SendSystemMessage(msg);
				}

				
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Not registered item[%d] - user[%S] !", __FUNCTION__, itemId, pUser->pSD->wName);
			}
		}
	}
	unguard;
}

void CItemAutoConsume::OnTiggerHp(User *pUser)
{
	guard;
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->blockAct)
			{
				unguard;
				return;
			}
			
			if(pUser->hideMode)
			{
				unguard;
				return;
			}

			if(!pUser->pSD->alive)
			{
				TurnOff(pUser, 0);
				unguard;
				return;
			}

			if(pUser->pEUD->championshipUser.state > Championship::UserTeleporting)
			{
				unguard;
				return;
			}

			INT32 itemId = 0;
			UINT lastUseTick = 0;
			AutoConsumeInfo *pInfo = 0;
			pUser->pEUD->Lock();
			itemId = pUser->pEUD->autoConsumeHp.itemId;
			if(itemId > 0)
			{
				lastUseTick = pUser->pEUD->autoConsumeHp.lastUseTick;
				pInfo = pUser->pEUD->autoConsumeHp.pInfo;
			}
			pUser->pEUD->Unlock();
			if(itemId > 0 && pInfo)
			{
				if(!pInfo->canUseInOlympiad)
				{
					if(pUser->olympiadUser.IsInside())
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action while fighting in olympiad.");
						unguard;
						return;
					}
				}

				if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(itemId))
				{
					double fixedMissing = pUser->pSD->max_hp - pUser->pSD->hp;
					double percentMissing = fixedMissing / pUser->pSD->max_hp * 100;
					if(fixedMissing >= pInfo->activateOnMissingFixed)
					{
						if( GetTickCount() > (lastUseTick + pInfo->reuseDelay) )
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeHp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_HP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_HP_TIMER);
								}
							}
						}else if( GetTickCount () < lastUseTick)
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeHp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_HP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_HP_TIMER);
								}
							}
						}
					}else if(percentMissing >= pInfo->activateOnMissingPercent)
					{
						if( GetTickCount() > (lastUseTick + pInfo->reuseDelay) )
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeHp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_HP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_HP_TIMER);
								}
							}
						}else if( GetTickCount () < lastUseTick)
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeHp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_HP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_HP_TIMER);
								}
							}
						}
					}
				}else
				{
					TurnOff(pUser, itemId, TRUE, TRUE);
				}
			}

		}
	}
	unguard;
}

void CItemAutoConsume::OnTiggerCp(User *pUser)
{
	guard;
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->blockAct)
			{
				unguard;
				return;
			}

			if(pUser->hideMode)
			{
				unguard;
				return;
			}

			if(!pUser->pSD->alive)
			{
				TurnOff(pUser, 0);
				unguard;
				return;
			}

			if(pUser->pEUD->championshipUser.state > Championship::UserTeleporting)
			{
				unguard;
				return;
			}

			INT32 itemId = 0;
			UINT lastUseTick = 0;
			AutoConsumeInfo *pInfo = 0;
			pUser->pEUD->Lock();
			itemId = pUser->pEUD->autoConsumeCp.itemId;
			if(itemId > 0)
			{
				lastUseTick = pUser->pEUD->autoConsumeCp.lastUseTick;
				pInfo = pUser->pEUD->autoConsumeCp.pInfo;
			}
			pUser->pEUD->Unlock();
			if(itemId > 0 && pInfo)
			{
				if(!pInfo->canUseInOlympiad)
				{
					if(pUser->olympiadUser.IsInside())
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action while fighting in olympiad.");
						unguard;
						return;
					}
				}
				if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(itemId))
				{
					double fixedMissing = pUser->pSD->max_cp - pUser->pSD->cp;
					double percentMissing = fixedMissing / pUser->pSD->max_cp * 100;
					if(fixedMissing >= pInfo->activateOnMissingFixed)
					{
						if( GetTickCount() > (lastUseTick + pInfo->reuseDelay) )
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeCp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_CP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_CP_TIMER);
								}
							}
						}else if( GetTickCount () < lastUseTick)
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeCp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_CP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_CP_TIMER);
								}
							}
						}
					}else if(percentMissing >= pInfo->activateOnMissingPercent)
					{
						if( GetTickCount() > (lastUseTick + pInfo->reuseDelay) )
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeCp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_CP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_CP_TIMER);
								}
							}
						}else if( GetTickCount () < lastUseTick)
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeCp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_CP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_CP_TIMER);
								}
							}
						}
					}
				}else
				{
					TurnOff(pUser, itemId, TRUE, TRUE);
				}
			}

		}
	}
	unguard;
}

void CItemAutoConsume::OnTiggerMp(User *pUser)
{
	guard;
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->blockAct)
			{
				unguard;
				return;
			}

			if(pUser->hideMode)
			{
				unguard;
				return;
			}

			if(!pUser->pSD->alive)
			{
				TurnOff(pUser, 0);
				unguard;
				return;
			}

			if(pUser->pEUD->championshipUser.state > Championship::UserTeleporting)
			{
				unguard;
				return;
			}

			INT32 itemId = 0;
			UINT lastUseTick = 0;
			AutoConsumeInfo *pInfo = 0;
			pUser->pEUD->Lock();
			itemId = pUser->pEUD->autoConsumeMp.itemId;
			if(itemId > 0)
			{
				lastUseTick = pUser->pEUD->autoConsumeMp.lastUseTick;
				pInfo = pUser->pEUD->autoConsumeMp.pInfo;
			}
			pUser->pEUD->Unlock();
			if(itemId > 0 && pInfo)
			{
				if(!pInfo->canUseInOlympiad)
				{
					if(pUser->olympiadUser.IsInside())
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action while fighting in olympiad.");
						unguard;
						return;
					}
				}
				if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(itemId))
				{
					double fixedMissing = pUser->pSD->max_mp - pUser->pSD->mp;
					double percentMissing = fixedMissing / pUser->pSD->max_mp * 100;
					if(fixedMissing >= pInfo->activateOnMissingFixed)
					{
						if( GetTickCount() > (lastUseTick + pInfo->reuseDelay) )
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeMp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_MP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_MP_TIMER);
								}
							}
						}else if( GetTickCount () < lastUseTick)
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeMp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_MP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_MP_TIMER);
								}
							}
						}
					}else if(percentMissing >= pInfo->activateOnMissingPercent)
					{
						if( GetTickCount() > (lastUseTick + pInfo->reuseDelay) )
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeMp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_MP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_MP_TIMER);
								}
							}
						}else if( GetTickCount () < lastUseTick)
						{
							if(pUser->UseItem(pItem))
							{
								pUser->pEUD->autoConsumeHp.lastUseTick = GetTickCount();
								if(pInfo->reuseDelay)
								{
									pUser->AddTimer(pInfo->reuseDelay, VALIDATE_AUTO_MP_TIMER);
								}else
								{
									pUser->AddTimer(250, VALIDATE_AUTO_MP_TIMER);
								}
							}
						}
					}
				}else
				{
					TurnOff(pUser, itemId, TRUE, TRUE);
				}
			}

		}
	}
	unguard;
}