#include "StdAfx.h"
#include "AutoLoot.h"
#include "HookManager.h"
#include "Party.h"

CAutoLoot g_AutoLoot;

CAutoLoot::CAutoLoot() : m_enabled(FALSE), m_affectParty(FALSE), m_affectRaidBoss(FALSE), m_activeByDefault(FALSE)
{
	SetName(L"AutoLoot");
}

CAutoLoot::~CAutoLoot()
{
}

void CAutoLoot::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	const TCHAR* sectionName = TEXT("AutoLoot");
	m_enabled = GetPrivateProfileInt(sectionName, TEXT("Enabled"), 0, g_ConfigFile);
	m_affectParty = GetPrivateProfileInt(sectionName, TEXT("AffectParty"), 0, g_ConfigFile);
	m_affectRaidBoss = GetPrivateProfileInt(sectionName, TEXT("AffectRaidBoss"), 0, g_ConfigFile);
	m_activeByDefault = GetPrivateProfileInt(sectionName, _T("ActiveByDefault"), 0, g_ConfigFile);
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature enabled with setting AffectParty[%d] AffectRaidBoss[%d]", GetName(), m_affectParty, m_affectRaidBoss);
	}
}

bool CAutoLoot::Handle(CCreature *pCreature, CCreature *pEnemy, int itemId, INT64 amount)
{
	if(m_enabled)
	{
		if(pCreature->ValidCreature() && pEnemy->ValidCreature())
		{
			if( ( pCreature->IsBoss() || pCreature->IsZZoldagu() ) && m_affectRaidBoss == FALSE)
			{
				return false;
			}

			User *pUser = pEnemy->CastUser();
			if(!pUser)
			{
				if(CSummon* pSummon = pEnemy->CastSummon())
				{
					pUser = pSummon->GetMaster();
				}else if(CPet *pPet = pEnemy->CastPet())
				{
					pUser = pPet->GetMaster();
				}
			}
			if(pUser->ValidUser())
			{
				if(CParty* pParty = pUser->GetParty())
				{
					if(m_affectParty)
					{
						UserVector members;
						pParty->GetAllMembersInRange(members, pCreature->pSD->pos, 1500);

						if(itemId == 57)
						{
							if(members.size() > 0)
							{
								amount /= static_cast<int>(members.size());
							}

							if(amount > 0)
							{
								for(UINT index = 0;index < members.size();index++)
								{
									if(User *pMember = members[index]->CastUser())
									{
										if(pMember->IsNowTrade())
										{
											pMember->TradeCancel();
										}
										if(pMember->inventory.CheckAddable(itemId, amount))
										{
											pMember->AddItemToInventory(itemId, amount);
										}
									}
								}
							}
							return true;
						}

						//give item according to routing type
						if(pParty->itemRoutingType == IR_FINDERS_KEEPERS)
						{
							if(pUser->pEUD->autoLoot)
							{
								if(pUser->IsNowTrade())
								{
									pUser->TradeCancel();
								}
								if(pUser->inventory.CheckAddable(itemId, amount))
								{
									pUser->AddItemToInventory(itemId, amount);
									return true;
								}
							}
							return false;
						}else if(pParty->itemRoutingType == IR_RANDOM || pParty->itemRoutingType == IR_RANDOM_INCLUDING_SPOIL)
						{
							UINT diceIndex = g_Random.RandInt(static_cast<int>(members.size()));
							for(UINT index = 0;index < members.size();index++)
							{
								if(User *pMember = members[diceIndex]->CastUser())
								{
									if(pMember->pEUD->autoLoot)
									{
										if(pMember->IsNowTrade())
										{
											pMember->TradeCancel();
										}
										if(pMember->inventory.CheckAddable(itemId, amount))
										{
											pMember->AddItemToInventory(itemId, amount);
											return true;
										}
									}
								}
								diceIndex++;
								diceIndex %= members.size();
							}
							return false;
						}else if(pParty->itemRoutingType == IR_BY_TURN || pParty->itemRoutingType == IR_BY_TURN_INCLUDING_SPOIL)
						{
							for(UINT counter=0;counter<members.size();counter++)
							{
								UINT index = pParty->indexByOrder % pParty->memberCount;
								pParty->indexByOrder = (pParty->indexByOrder + 1) % pParty->memberCount;
								index %= members.size();

								if(User *pMember = members[index]->CastUser())
								{
									if(pMember->pEUD->autoLoot)
									{
										if(pMember->IsNowTrade())
										{
											pMember->TradeCancel();
										}
										if(pMember->inventory.CheckAddable(itemId, amount))
										{
											pMember->AddItemToInventory(itemId, amount);
											return true;
										}
									}
								}
							}
							return false;
						}
					}
				}else
				{
					if(pUser->pEUD->autoLoot)
					{
						if(pUser->IsNowTrade())
						{
							pUser->TradeCancel();
						}

						if(pUser->inventory.CheckAddable(itemId, amount))
						{
							pUser->AddItemToInventory(itemId, amount);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

