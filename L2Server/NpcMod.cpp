#include "stdafx.h"
#include "NpcMod.h"
#include "AutoLoot.h"
#include "HookManager.h"

CNpcMod g_NpcMod;

CNpcMod::CNpcMod() : m_Enabled(FALSE)
{
	SetName(L"Npc Mod");
}

CNpcMod::~CNpcMod()
{
}

void CNpcMod::Init()
{
	m_DropList.clear();
	LoadINI();
	if(m_Enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Loaded [%d] items.", GetName(), m_DropList.size());
	}
}

void CNpcMod::LoadINI()
{
	const TCHAR* section = _T("NpcMod");
	m_Enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, g_ConfigFile);
	if(m_Enabled)
	{
		m_DisableLevelPenalty = GetPrivateProfileInt(section, _T("DisableLevelPenalty"), 0, g_ConfigFile);
		if(m_DisableLevelPenalty)
		{
			WriteBYTE(0x72997D, 0x33);
			WriteBYTE(0x72997E, 0xDB);
		}else
		{
			WriteBYTE(0x72997D, 0x2B);
			WriteBYTE(0x72997E, 0xDA);
		}

		TCHAR temp[8192];
		memset(temp, 0, sizeof(temp));
		if(GetPrivateProfileString(section, _T("ItemIdList"), 0, temp, 8192, g_ConfigFile))
		{
			wstringstream sstr;
			sstr << temp;
			INT32 itemId = 0;
			while(sstr >> itemId)
			{
				NpcModData data;
				data.itemId = itemId;
				{
					wstringstream sKey;
					sKey << L"AffectAllNpcs_" << itemId;
					data.affectAllNpcs = GetPrivateProfileInt(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				{
					wstringstream sKey;
					sKey << L"RequiredItemId_" << itemId;
					data.requiredItemId = GetPrivateProfileInt(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				{
					wstringstream sKey;
					sKey << L"NormalChance_" << itemId;
					data.normalChance = GetPrivateProfileDouble(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				{
					wstringstream sKey;
					sKey << L"NormalMinCount_" << itemId;
					data.normalMinCount = GetPrivateProfileInt(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				{
					wstringstream sKey;
					sKey << L"NormalMaxCount_" << itemId;
					data.normalMaxCount = GetPrivateProfileInt(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				{
					wstringstream sKey;
					sKey << L"RaidBossChance_" << itemId;
					data.raidBossChance = GetPrivateProfileDouble(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				{
					wstringstream sKey;
					sKey << L"RaidBossMinCount_" << itemId;
					data.raidBossMinCount = GetPrivateProfileInt(section, sKey.str().c_str(), 0, g_ConfigFile); 
				}
				{
					wstringstream sKey;
					sKey << L"RaidBossMaxCount_" << itemId;
					data.raidBossMaxCount = GetPrivateProfileInt(section, sKey.str().c_str(), 0, g_ConfigFile);
				}
				memset(temp, 0, sizeof(temp));
				wstringstream sKey;
				sKey << L"NpcIdList_" << itemId;
				if(GetPrivateProfileString(section, sKey.str().c_str(), 0, temp, 8192, g_ConfigFile))
				{
					wstringstream sNpc;
					sNpc << temp;
					INT32 npcId = 0;
					while(sNpc >> npcId)
					{
						data.affectedNpc.push_back(npcId);
					}
				}
				m_DropList.push_back(data);
			}
		}
	}
}

void CNpcMod::OnDie(CNpc *pNpc, CCreature *pEnemy)
{
	if(m_Enabled)
	{
		if(pNpc->ValidCreature() && pEnemy->ValidCreature() && pNpc != pEnemy)
		{
			for(UINT n=0;n<m_DropList.size();n++)
			{
				NpcModData& nmd = m_DropList[n];
				bool affected = false;
				if(nmd.requiredItemId > 0)
				{
					if(!pEnemy->inventory.HaveItemByClassId(nmd.requiredItemId, 1))
					{
						continue;
					}
				}

				if(nmd.affectAllNpcs)
				{
					affected = true;
				}else
				{
					for(UINT m=0;m<nmd.affectedNpc.size();m++)
					{
						if(nmd.affectedNpc[m] == pNpc->pSD->npc_class_id)
						{
							affected = true;
							break;
						}
					}
				}

				if(affected)
				{
					double chance = nmd.normalChance;
					INT64 amount = 0;
					if(pNpc->IsBoss())
					{
						chance = nmd.raidBossChance;
						INT32 diff = nmd.raidBossMaxCount - nmd.raidBossMinCount;
						if(diff > 0)
						{
							amount = nmd.raidBossMinCount + g_Random.RandInt(diff+1);
						}else
						{
							amount = nmd.raidBossMinCount;
						}
					}else
					{
						INT32 diff = nmd.normalMaxCount - nmd.normalMinCount;
						if(diff > 0)
						{
							amount = nmd.normalMinCount + g_Random.RandInt(diff+1);
						}else
						{
							amount = nmd.normalMinCount;
						}
					}

					if(m_DisableLevelPenalty == FALSE && pNpc->pSD->level <= (pEnemy->pSD->level - 9))
					{
						chance = 0.0;
					}

					double dice  = RandDoubleEx(100.0);
					//g_Log.Add(CLog::Blue, "[%s] Chance[%f] dice[%f] itemId[%d]", __FUNCTION__, chance, dice, Iter->itemId);
					if(dice < chance)
					{
						if(!g_AutoLoot.Handle(pNpc, pEnemy, nmd.itemId, amount))
						{
							pNpc->AddItemToInventory(nmd.itemId, amount);
						}
					}
				}
			}
		}
	}
}