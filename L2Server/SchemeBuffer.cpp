#include "StdAfx.h"
#include "SchemeBuffer.h"
#include "SkillDB.h"

CSchemeBuffer g_SchemeBuffer;

CSchemeBuffer::CSchemeBuffer() : validateTalkTarget(TRUE), htmlLinkCheck(FALSE), toLowLevelMsg(0), toHighLevelMsg(0), notEnoughAdenaMsg(0), allowOlympiad(0), allowPk(0), allowCombat(0), allowGuilty(0), addToListMsg(0), addToSummonListMsg(0)
{
	SetName(L"Scheme Buffer");
}

CSchemeBuffer::~CSchemeBuffer()
{
}

void CSchemeBuffer::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	mBuffs.clear();
	enabled = FALSE;
	LoadData();
	g_Log.Add(CLog::Blue, "[%S] Loaded %d buffs into database.", GetName(), mBuffs.size());
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled.", GetName());
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled.", GetName());
	}
}

void CSchemeBuffer::LoadData()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//SchemeBuffer.txt", lpData);
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
					//setting_begin	enabled=1 show_animation=1	to_low_level_msg=2344	to_high_level_msg=2345	not_enough_adena_msg=2346	setting_end
					enabled = ParseOptionInt(line, L"enabled", 0);
					allowPk = ParseOptionInt(line, L"allow_pk", 1);
					allowOlympiad = ParseOptionInt(line, L"allow_olympiad", 0);
					allowCombat = ParseOptionInt(line, L"allow_combat", 1);
					allowGuilty = ParseOptionInt(line, L"allow_guilty", 1);
					maxListSize = ParseOptionInt(line, L"list_size", 46);
					listPrice.itemId = ParseOptionInt(line, L"list_price_id", 0);
					listPrice.amount = ParseOptionInt(line, L"list_price_amount", 0);
					overrideListPrice = ParseOptionInt(line, L"override_list_price", 0);
					toLowLevelMsg = ParseOptionInt(line, L"to_low_level_msg", 0);
					toHighLevelMsg = ParseOptionInt(line, L"to_high_level_msg", 0);
					notEnoughAdenaMsg = ParseOptionInt(line, L"not_enough_adena_msg", 0);
					addToListMsg = ParseOptionInt(line, L"add_to_list_msg", 0);
					addToSummonListMsg = ParseOptionInt(line, L"add_to_summon_list_msg", 0);
					validateTalkTarget = ParseOptionInt(line, L"validate_talk_target", 1);
					htmlLinkCheck = ParseOptionInt(line, L"html_link_check", 0);
					continue;
				}
				if( line.find(L"skill_begin") == 0 )
				{
					//skill_begin	skill_id=1204	skill_level=2	price=0	min_level=1	max_level=90	custom_time=3600	skill_end
					SchemeBuffData data;
					memset(&data, 0, sizeof(SchemeBuffData));

					data.skillId = ParseOptionInt(line, L"skill_id", 0);
					data.level = ParseOptionInt(line, L"skill_level", 0);
					data.price.itemId = ParseOptionInt(line, L"price_id", 0);
					data.price.amount = ParseOptionInt(line, L"price_amount", 0);
					data.minLevel = ParseOptionInt(line, L"min_level", 0);
					data.maxLevel = ParseOptionInt(line, L"max_level", 90);
					data.customTime = ParseOptionInt(line, L"custom_time", 0);
					if(data.skillId > 0 && data.level > 0)
					{
						CSkillInfo *pSI = g_SkillDB.GetSkillInfo(data.skillId, data.level, 0);
						if(pSI)
						{
							mBuffs.insert(pair<SkillPchId, SchemeBuffData>(SkillPchId(data.skillId, data.level), data));
						}else
						{
							g_Log.Add(CLog::Error, "[%S] Tried to load not existing skill_id[%d] level[%d]!", GetName(), data.skillId, data.level);
						}
					}else
					{
						g_Log.Add(CLog::Error, "[%S] Tried to load not existing skill_id[%d] level[%d]!", GetName(), data.skillId, data.level);
					}
				}
			}
		}
		delete [] lpData;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//SchemeBuffer.txt !", GetName());
	}
}

void CSchemeBuffer::BuffPlayer(User *pUser, int skillClassId, bool byList)
{
	if(pUser->ValidUser())
	{
		if(pUser->olympiadUser.status != 0 && !allowOlympiad)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that during olympiad fight!");
			return;
		}
		if(pUser->pSD->karma > 0 && !allowPk)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return;
		}
		if(pUser->pSD->is_in_combat_mode && !allowCombat)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return;
		}
		if(pUser->pSD->is_guilty && !allowGuilty)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return;
		}

		SkillPchId pchId(skillClassId);
		map<SkillPchId, SchemeBuffData>::iterator Iter = mBuffs.find(pchId);
		if( Iter != mBuffs.end() )
		{
			if( pUser->pSD->level < Iter->second.minLevel )
			{
				pUser->pUserSocket->SendSystemMessage(toLowLevelMsg);
				return;
			}
			if( pUser->pSD->level > Iter->second.maxLevel )
			{
				pUser->pUserSocket->SendSystemMessage(toHighLevelMsg);
				return;
			}

			if(Iter->second.price.itemId > 0 && !byList)
			{
				if(!pUser->inventory.HaveItemByClassId(Iter->second.price.itemId, Iter->second.price.amount))
				{
					pUser->pUserSocket->SendSystemMessage(L"Not enough required item(s).");
					return;
				}

				pUser->DeleteItemInInventory(Iter->second.price.itemId, Iter->second.price.amount);
				
			}

			if(CSkillInfo *pSI = g_SkillDB.GetSkillInfo(Iter->second.skillId, Iter->second.level, 0))
			{
				pSI->ActivateSkill(pUser, pUser);
				pUser->SetSelfBuff(Iter->second.skillId, Iter->second.level, false);
				if(Iter->second.customTime > 0)
				{
					pUser->SetNewAbnormalTime(Iter->second.skillId, Iter->second.level, Iter->second.customTime);
				}
			}

		}else
		{
			g_Log.Add(CLog::Error, "[%S] Invalid skill class id[%d] - user[%S]!", GetName(), skillClassId, pUser->pSD->wName);
		}
	}
}

void CSchemeBuffer::BuffSummon(User *pUser, int skillClassId, bool byList)
{
	if(pUser->ValidUser())
	{
		if(pUser->olympiadUser.status != 0 && !allowOlympiad)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that during olympiad fight!");
			return;
		}
		if(pUser->pSD->karma > 0 && !allowPk)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return;
		}
		if(pUser->pSD->is_in_combat_mode && !allowCombat)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return;
		}
		if(pUser->pSD->is_guilty && !allowGuilty)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return;
		}

		CSummon* pSummon = pUser->GetSummonOrPet();

		if(!pSummon->ValidCreature())
		{
			return;
		}

		//check dist
		FVector pos1(pUser->pSD->pos);
		FVector pos2(pSummon->pSD->pos);
		double x = pos1.x - pos2.x;
		x *= x;
		double y = pos1.y - pos2.y;
		y *= y;

		if( (x + y) > 62500.0 )
		{
			pUser->pUserSocket->SendSystemMessage(L"Scheme Buffer", L"Your summon/pet is too far.");
			return;
		}


		SkillPchId pchId(skillClassId);
		map<SkillPchId, SchemeBuffData>::iterator Iter = mBuffs.find(pchId);
		if( Iter != mBuffs.end() )
		{
			if( pUser->pSD->level < Iter->second.minLevel )
			{
				pUser->pUserSocket->SendSystemMessage(toLowLevelMsg);
				return;
			}
			if( pUser->pSD->level > Iter->second.maxLevel )
			{
				pUser->pUserSocket->SendSystemMessage(toHighLevelMsg);
				return;
			}

			if(Iter->second.price.itemId > 0 && !byList)
			{
				if(!pUser->inventory.HaveItemByClassId(Iter->second.price.itemId, Iter->second.price.amount))
				{
					pUser->pUserSocket->SendSystemMessage(L"Not enough required item(s).");
					return;
				}

				pUser->DeleteItemInInventory(Iter->second.price.itemId, Iter->second.price.amount);
				
			}

			if(CSkillInfo *pSI = g_SkillDB.GetSkillInfo(Iter->second.skillId, Iter->second.level, 0))
			{
				pSI->ActivateSkill(pSummon, pSummon);
				pSummon->SetSelfBuff(Iter->second.skillId, Iter->second.level, FALSE);
				if(Iter->second.customTime > 0)
				{
					pSummon->SetNewAbnormalTime(Iter->second.skillId, Iter->second.level, Iter->second.customTime, false);
				}
			}

		}else
		{
			g_Log.Add(CLog::Error, "[%S] Invalid skill class id[%d] - user[%S]!", GetName(), skillClassId, pUser->pSD->wName);
		}
	}
}

void CSchemeBuffer::AddBuff(User *pUser, int skillClassId, bool summon)
{
	if(pUser->ValidUser())
	{
		if(pUser->olympiadUser.status != 0 && !allowOlympiad)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that during olympiad fight!");
			return;
		}

		if(pUser->pSD->karma > 0 && !allowPk)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return;
		}
		if(pUser->pSD->is_in_combat_mode && !allowCombat)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return;
		}
		if(pUser->pSD->is_guilty && !allowGuilty)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return;
		}

		SkillPchId pchId(skillClassId);
		map<SkillPchId, SchemeBuffData>::iterator Iter = mBuffs.find(pchId);
		if( Iter != mBuffs.end() )
		{
			pUser->pEUD->schemeBuffList.Add(skillClassId, summon);

			if(summon)
			{
				CSystemMessage msg(addToSummonListMsg);
				msg.AddSkill(pchId.skillId, pchId.level);
				pUser->pUserSocket->SendSystemMessage(msg);

				BuffSummon(pUser, skillClassId);
			}else
			{
				CSystemMessage msg(addToListMsg);
				msg.AddSkill(pchId.skillId, pchId.level);
				pUser->pUserSocket->SendSystemMessage(msg);

				BuffPlayer(pUser, skillClassId);
			}
		}
	}
}

void CSchemeBuffer::UseList(User *pUser, bool summon)
{
	if(pUser->ValidUser())
	{
		if(pUser->olympiadUser.status != 0 && !allowOlympiad)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that during olympiad fight!");
			return;
		}
		if(pUser->pSD->karma > 0 && !allowPk)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return;
		}
		if(pUser->pSD->is_in_combat_mode && !allowCombat)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return;
		}
		if(pUser->pSD->is_guilty && !allowGuilty)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return;
		}

		

		vector<UINT> buffs;
		pUser->pEUD->schemeBuffList.GetData(buffs, summon);
		if(listPrice.itemId > 0 && buffs.size() > 0)
		{
			if(!pUser->inventory.HaveItemByClassId(listPrice.itemId, listPrice.amount))
			{
				pUser->pUserSocket->SendSystemMessage(L"Not enough required item(s).");
				return;
			}

			pUser->DeleteItemInInventory(listPrice.itemId, listPrice.amount);	
		}

		for(UINT n = 0; n < buffs.size(); n++)
		{
			if(buffs[n] > 0)
			{
				if(summon)
				{
					if(overrideListPrice)
					{
						BuffSummon(pUser, buffs[n], false);
					}else
					{
						BuffSummon(pUser, buffs[n], true);
					}
				}else
				{
					if(overrideListPrice)
					{
						BuffPlayer(pUser, buffs[n], false);
					}else
					{
						BuffPlayer(pUser, buffs[n], true);
					}
				}
			} 
		}
	}
}

UINT CSchemeBuffer::GetMaxListSize()
{
	return maxListSize;
}

BOOL CSchemeBuffer::IsEnabled()
{
	return enabled;
}

BOOL CSchemeBuffer::IsValidateTalkTarget()
{
	return validateTalkTarget;
}