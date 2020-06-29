#include "StdAfx.h"
#include "CreatureAction.h"
#include "HookManager.h"
#include "SkillInfo.h"
#include "SpawnProtection.h"
#include "ItemAutoConsume.h"
#include "ItemReuseManager.h"
#include "SkillReuseDB.h"
#include "AllInOne.h"

extern double g_AttributeAttackMultipler;
extern double g_AttributeDefenceMultipler;
extern BOOL g_SpiritShotExtension;
extern BOOL g_OldSkillReuseFormula;
extern double g_MaxCriticalDamageMul;
extern double g_MaxCriticalDamageAdd;

double g_FrenzyRaidBossDmgPenalty = 1.0;
double g_MagicCriticalPvP = 2.5;
double g_MagicCriticalPvE = 3.0;
BOOL g_DisableMagicRandomDamage = FALSE;
BOOL g_DisableLevelGap = FALSE;

double g_DoubleValue100 = 100.0;

void CreatureAction::Init()
{
	g_DisableLevelGap = GetPrivateProfileInt(_T("Setting"), _T("DisableLevelGap"), 0, g_ConfigFile);
	g_FrenzyRaidBossDmgPenalty = GetPrivateProfileDouble(_T("SkillFx"), _T("FrenzyRaidBossDmgPenalty"), 1.0, g_ConfigFile);
	g_MagicCriticalPvP = GetPrivateProfileDouble(_T("SkillFx"), _T("MagicCriticalPvP"), 2.5, g_ConfigFile);
	g_MagicCriticalPvE = GetPrivateProfileDouble(_T("SkillFx"), _T("MagicCriticalPvE"), 3.0, g_ConfigFile);
	g_DisableMagicRandomDamage = GetPrivateProfileInt(_T("SkillFx"), _T("DisableMagicRandomDamage"), FALSE, g_ConfigFile);

	if(g_DisableMagicRandomDamage)
	{
		WriteCall(0x84A7DE, CreatureAction::GetRandomMagicDamage);
		WriteCall(0x84A855, CreatureAction::GetRandomMagicDamage);
		WriteCall(0x84A861, CreatureAction::GetRandomMagicDamage);
	}

	WriteCall(0x53EC70, CreatureAction::GetAttributeBonus);
	WriteCall(0x6FF776, CreatureAction::GetAttributeBonus);

	WriteCall(0x439299, CreatureAction::InfectedSkillBy);
	WriteCall(0x43937F, CreatureAction::InfectedSkillBy);
	WriteCall(0x45DF65, CreatureAction::InfectedSkillBy);
	WriteCall(0x45DF8C, CreatureAction::InfectedSkillBy);
	WriteCall(0x4AE4E6, CreatureAction::InfectedSkillBy);
	WriteCall(0x4AE506, CreatureAction::InfectedSkillBy);
	WriteCall(0x4FA2BD, CreatureAction::InfectedSkillBy);
	WriteCall(0x56B6F5, CreatureAction::InfectedSkillBy);
	WriteCall(0x81E18C, CreatureAction::InfectedSkillBy);
	WriteCall(0x81E1BC, CreatureAction::InfectedSkillBy);
	WriteCall(0x82002A, CreatureAction::InfectedSkillBy);
	WriteCall(0x820157, CreatureAction::InfectedSkillBy);
	WriteCall(0x8AA7B1, CreatureAction::InfectedSkillBy);
	WriteCall(0x8D6127, CreatureAction::InfectedSkillBy);

	WriteCall(0x54033B, CreatureAction::OutOfSight, 1);	//CCreature::SetVisible hook

	WriteCall(0x4FDB20, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x51ECB7, CreatureAction::CheckAbnormalVisualEffect);
	WriteJump(0x54A536, (INT64)CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x54A77F, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x54AACF, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x54B835, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x55CDCF, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x564ABC, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x56606F, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x5E5375, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x603B68, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x717600, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x84CB83, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x8840D3, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x8C2ECC, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x8CFA48, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x8FD293, CreatureAction::CheckAbnormalVisualEffect);
	WriteCall(0x909C67, CreatureAction::CheckAbnormalVisualEffect);

	WriteCall(0x4FAFAE, CreatureAction::ResurrectBy);
	WriteCall(0x4FDA9A, CreatureAction::ResurrectBy);
	WriteCall(0x500994, CreatureAction::ResurrectBy);
	WriteCall(0x55E73F, CreatureAction::ResurrectBy);
	WriteCall(0x9235F7, CreatureAction::ResurrectBy);

	WriteCall(0x8369BC, CreatureAction::DispelAll);

	WriteCall(0x45DF39, CreatureAction::DispelAllByGM);
	WriteCall(0x47F26C, CreatureAction::DispelAllByGM);
	WriteCall(0x56979C, CreatureAction::DispelAllByGM);
	WriteCall(0x603992, CreatureAction::DispelAllByGM);
	WriteCall(0x61493B, CreatureAction::DispelAllByGM);
	WriteCall(0x6149B1, CreatureAction::DispelAllByGM);
	WriteCall(0x6DAFB7, CreatureAction::DispelAllByGM);
	WriteCall(0x6DB306, CreatureAction::DispelAllByGM);
	WriteCall(0x76C1C0, CreatureAction::DispelAllByGM);
	WriteCall(0x76CF7F, CreatureAction::DispelAllByGM);
	WriteCall(0x771C1C, CreatureAction::DispelAllByGM);
	WriteCall(0x771DA7, CreatureAction::DispelAllByGM);
	WriteCall(0x9062CA, CreatureAction::DispelAllByGM);

	WriteCall(0x839400, CreatureAction::DispelByCategory);

	WriteCall(0x527FD0, CreatureAction::InitializeSkillMod);
	WriteCall(0x55465A, CreatureAction::InitializeSkillMod);
	WriteCall(0x5605ED, CreatureAction::InitializeSkillMod);
	WriteCall(0x8D4BC0, CreatureAction::InitializeSkillMod);
	WriteCall(0x90646C, CreatureAction::InitializeSkillMod);
	
	WriteCall(0x81E2B0, CreatureAction::CanActivateSkill);
	WriteCall(0x81F49F, CreatureAction::CanActivateSkill);

	WriteCall(0x71B20E, CreatureAction::MoveToLocation);

	WriteToVMT(VT_CBOSS, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CCOLLECTION, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CCREATURE, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CDOPPELGANGER, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CMERCHANT, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CNPC, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CSUMMON, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CPET, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CPCTRAP, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(0xC396C8, 0x858, CreatureAction::MoveToLocation);	//world trap ?
	WriteToVMT(0xC3B568, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_USER, 0x858, CreatureAction::MoveToLocation);
	WriteToVMT(VT_CZZOLDAGU, 0x858, CreatureAction::MoveToLocation);
	
	WriteToVMT(VT_CBOSS, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CCOLLECTION, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CCREATURE, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CDOPPELGANGER, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CMERCHANT, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CNPC, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CSUMMON, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CPET, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CPCTRAP, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(0xC396C8, 0x8C8, CreatureAction::GotDamageBy);	//world trap ?
	WriteToVMT(0xC3B568, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_USER, 0x8C8, CreatureAction::GotDamageBy);
	WriteToVMT(VT_CZZOLDAGU, 0x8C8, CreatureAction::GotDamageBy);

	WriteCall(0x8DD566, CreatureAction::UseItem);	//User::UseItem
	WriteToVMT(VT_USER, 0x790, CreatureAction::UseItem);

	WriteCall(0x550712, CreatureAction::CalcReuseDelay);
	
	WriteCall(0x84A947, CreatureAction::GetMagicCritical);
	WriteCall(0x84AC24, CreatureAction::GetMagicCritical);
	
	WriteCall(0x5736EC, CreatureAction::GetRemainReuseDelaySec);
	WriteCall(0x5761D4, CreatureAction::GetRemainReuseDelaySec);
	WriteCall(0x713129, CreatureAction::GetRemainReuseDelaySec);
	WriteCall(0x713284, CreatureAction::GetRemainReuseDelaySec);
	WriteCall(0x847404, CreatureAction::GetRemainReuseDelaySec);
	WriteCall(0x847421, CreatureAction::GetRemainReuseDelaySec);
	WriteCall(0x895B59, CreatureAction::GetRemainReuseDelaySec);

	//WriteCall(0x00, CreatureAction::GetLevelParam);
	if(g_DisableLevelGap)
	{
		WriteRelative(0x75D167, &g_DoubleValue100);
		WriteRelative(0x75F142, &g_DoubleValue100);
	}

	WriteCall(0x54C832, CreatureAction::SetItemUsableTime);
	WriteCall(0x703C3F, CreatureAction::CheckAttackValidity);

	WriteCall(0x47F129, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x49CEDD, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x4CD58F, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x4CD680, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x550749, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x550765, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x5B8380, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x5B83E6, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x5CE833, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x5CE89F, CreatureAction::AddOrUpdateSkillReuseDelay);
	WriteCall(0x8F9659, CreatureAction::AddOrUpdateSkillReuseDelay);

	WriteCall(0x55C9DD, CreatureAction::ValidateSkillMod);
	WriteCall(0x55CABC, CreatureAction::ValidateSkillMod);
	WriteCall(0x72965B, CreatureAction::ValidateSkillMod);
	WriteCall(0x859DA1, CreatureAction::ValidateSkillMod);
	WriteCall(0x85E642, CreatureAction::ValidateSkillMod);
	WriteCall(0x89448C, CreatureAction::ValidateSkillMod);
}

void CreatureAction::AddOrUpdateSkillReuseDelay(CCreature *pCreature, int skillId, UINT subjobId, int reuseDelay, UINT reuseType)
{
	guard;

	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pEUD->pUSR)
		{
		//	g_Log.Add(CLog::Blue, "[%s] setting skill[%d] reuse[%d] type[%d]", __FUNCTION__, skillId, reuseDelay, reuseType);
			pUser->pEUD->pUSR->Set(skillId, (GetTickCount()+reuseDelay), subjobId, reuseType);
		}
	}

	typedef void(*f)(CCreature*, int, UINT, int, UINT);
	f(0x549AF8L)(pCreature, skillId, subjobId, reuseDelay, reuseType);

	unguard;
}

bool CreatureAction::CheckAttackValidity(CCreature *pCreature, CObject *pTarget)
{
	guard;
	bool ret = false;

	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pEUD->isItemDataLock)
		{
			if(User *pTargetUser = pTarget->CastUser())
			{
				if(pTargetUser->pSD->is_guilty == false && pTargetUser->pSD->karma == 0 && pTargetUser->pSD->in_battlefield == false)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
					unguard;
					return false;
				}
			}else if(CSummon *pSummon = pTarget->CastSummon())
			{
				if(pSummon->pSD->is_guilty == false && pSummon->pSD->karma == 0 && pSummon->pSD->in_battlefield == false)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
					unguard;
					return false;
				}
			}else if(CPet *pPet = pTarget->CastPet())
			{
				if(pPet->pSD->is_guilty == false && pPet->pSD->karma == 0 && pPet->pSD->in_battlefield == false)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
					unguard;
					return false;
				}
			}
		}
	}

	typedef bool (*f)(CCreature*, CObject*);
	ret = f(0x6FC288L)(pCreature, pTarget);

	unguard;
	return ret;
}

void CreatureAction::SetItemUsableTime(CCreature *pCreature, int itemId, ULONG* pStamp, ULONG reuse)
{
	guard;

	if(User *pUser = pCreature->CastUser())
	{
		if(reuse > 10000)
		{
			//save reuse only for items with 10s + reuse
			if(g_ItemReuseManager.IsEnabled())
			{
				g_ItemReuseManager.SetReuse(pUser, itemId, reuse);
			}
		}
	}

	pCreature->SetItemUsableTime(itemId, pStamp, reuse);
	unguard;
}

double CreatureAction::GetLevelParam(INT32 level, INT32 level2)
{
	if(g_DisableLevelGap)
	{
		typedef double (*f)(int, int);
		return f(0x758498L)(80, 80);
	}
	typedef double (*f)(int, int);
	return f(0x758498L)(level, level2);
}

int CreatureAction::GetRemainReuseDelaySec(CCreature *pCreature, int skillId)
{
	guard;
	UINT reuse = 0;

	typedef UINT (*f)(CCreature*, int);
	UINT timestamp = f(0x550B9CL)(pCreature, skillId);	//CCreature::GetSkillUsedTime
//	g_Log.Add(CLog::Blue, "[%s] skill[%d] timestamp[%d] currentTick[%d] diff[%d]", __FUNCTION__, skillId, timestamp, GetTickCount(), (timestamp - GetTickCount()));
	if(timestamp > 0)
	{
		UINT currentTick = GetTickCount();
		if(timestamp > currentTick)
		{
			timestamp -= currentTick;
			if(timestamp != 0)
			{
				if((timestamp % 1000) != 0)
				{
					reuse++;
				}
				reuse += (timestamp / 1000);
			}
		}
	}

	unguard;

	return static_cast<int>(reuse);
}

double CreatureAction::GetRandomMagicDamage(CCreature *pCreature)
{
	return 1.0;
}

double CreatureAction::GetMagicCritical(CCreature *pCreature, CObject *pObject, double critRateParam)
{
	double ret = 1.0;
	guard;

	typedef double(*f)(CCreature*, CObject*, double);
	ret = f(0x849BD4L)(pCreature, pObject, critRateParam);

	if(ret > 1.0)
	{
	//	g_Log.Add(CLog::Blue, "[%s] Creature[%S] GF Crit[%f]", __FUNCTION__, pCreature->pSD->wName, ret);
		if(ret == 2.5)
		{
			//pvp magic critical
		//	g_Log.Add(CLog::Blue, "[%s] PvP Creature[%S] GF Crit[%f] New[%f]", __FUNCTION__, pCreature->pSD->wName, ret, g_MagicCriticalPvP);
			ret = g_MagicCriticalPvP;
		}else if(ret == 3.0)
		{
		//	g_Log.Add(CLog::Blue, "[%s] PvE Creature[%S] GF Crit[%f] New[%f]", __FUNCTION__, pCreature->pSD->wName, ret, g_MagicCriticalPvE);
			ret = g_MagicCriticalPvE;
		}
	}

	unguard;
	return ret;
}

int CreatureAction::CalcReuseDelay(CCreature *pCreature, CSkillInfo *pSI)
{
	if(g_OldSkillReuseFormula)
	{
		typedef int(*f)(CCreature*, CSkillInfo*);
		int gfReuse = f(0x52ED4CL)(pCreature, pSI);

		double reuseDelay = pSI->reuseDelay;
		int magicType = pSI->isMagic;
		if(magicType < 4)
		{
			reuseDelay *= pCreature->reuseDelayMultipler[magicType];
		}

		double param = 1.0;

		if(magicType == 0 || magicType == 3)
		{
			//physical skill & dance
			reuseDelay /= pCreature->pSD->physical_use_speed;
			param = pCreature->pSD->physical_use_speed;
		}else if(magicType == 1)
		{
			//magic skill
			reuseDelay /= pCreature->pSD->magic_use_speed;
			param = pCreature->pSD->magic_use_speed;
			if(pCreature->spiritShotParam > 1.0)
			{
				reuseDelay *= 0.8;
			}
		}
		
		/*
		int reuseDelay = (int)(skill.getReuseDelay() * getStat().getMReuseRate(skill));
        reuseDelay *= 333.0 / (skill.isMagic() ? getMAtkSpd() : getPAtkSpd());
		*/

		int ilReuse = static_cast<INT32>(reuseDelay);

//		g_Log.Add(CLog::Blue, "Reuse skill gf[%d] il[%d] param [%f]", gfReuse, ilReuse, param);

		return ilReuse;
	}else
	{
		typedef int(*f)(CCreature*, CSkillInfo*);
		return f(0x52ED4CL)(pCreature, pSI);
	}
}

bool CreatureAction::UseItem(CCreature *pCreature, CItem *pItem, int param)
{
	guard;

	int itemId = 0;
	if(pItem->ValidItem())
	{
		itemId = pItem->pSID->itemId;
	}

	typedef bool (*f)(CCreature*, CItem*, int);
	if(f(0x54C4FCL)(pCreature, pItem, param))
	{
		if(User *pUser = pCreature->CastUser())
		{
			if(pUser->pEUD->spawnProtection)
			{
				g_SpawnProtection.Deactivate(pUser);
			}

			if(g_SpiritShotExtension && pUser->spiritShotInUse)
			{
				//check id for spiritshots and blessed spiritshots
				if( (itemId > 2508 && itemId < 2515) || itemId == 5790 || (itemId > 3946 && itemId < 3953) /* Normal SpiritShots */
					|| (itemId > 17174 && itemId < 17185) || (itemId > 18074 && itemId < 18100) || (itemId > 22071 && itemId < 22082)/* Cash Spiritshots */	)
				{	
					for(vector<CSkillInfo*>::iterator Iter = pItem->vItemSkill.begin(); Iter!= pItem->vItemSkill.end();Iter++)
					{
						CSkillInfo *pInfo = (*Iter);
						if(pInfo)
						{
							pInfo->ActivateSkill(pUser, pUser);
							pUser->spiritShotInUse = 1;
						}
					}
				}
			}
		}
		unguard;
		return true;
	}

	unguard;
	return false;
}

bool CreatureAction::CanActivateSkill(CSkillInfo *pSI, CCreature *pCaster, CObject *pTarget)
{
	typedef bool (*f)(CSkillInfo*, CCreature*, CObject*);

//	g_Log.Add(CLog::Blue, "[%s]", __FUNCTION__);
	if(User *pUser = pCaster->CastUser())
	{
//		g_Log.Add(CLog::Blue, "[%s] [%S] effectPoint[%d]", __FUNCTION__, pUser->pSD->wName, pSI->effectPoint);
		if(pUser->pEUD->isItemDataLock && pSI->effectPoint < 0)
		{
			if(User *pTargetUser = pTarget->CastUser())
			{
				if(pTargetUser->pSD->is_guilty == false && pTargetUser->pSD->karma == 0 && pTargetUser->pSD->in_battlefield == false)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
					return false;
				}
			}else if(CSummon *pSummon = pTarget->CastSummon())
			{
				if(pSummon->pSD->is_guilty == false && pSummon->pSD->karma == 0 && pSummon->pSD->in_battlefield == false)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
					return false;
				}
			}else if(CPet *pPet = pTarget->CastPet())
			{
				if(pPet->pSD->is_guilty == false && pPet->pSD->karma == 0 && pPet->pSD->in_battlefield == false)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
					return false;
				}
			}
		}
		if(User *pTargetUser = pTarget->CastUser())
		{
			/* do checks here */
			if(pUser == pTargetUser && pSI->skillId == 1505)	//Sublime Self-Sacrifice
			{
				return false;
			}
		}
	}

	return f(0x81C410L)(pSI, pCaster, pTarget);
}

double CreatureAction::GotDamageBy(CCreature *pCreature, CCreature *pEnemy, double damage, bool param1, UINT damageType, bool param2)
{
	typedef double (*f)(CCreature*, CCreature*, double, bool, UINT, bool);

	if(pCreature->IsBoss() || pCreature->IsZZoldagu())
	{
		if(pEnemy->ValidCreature())
		{
			if( pEnemy->GetAbnormalLevel(AB_PINCH) > 0)
			{
				//make sure it's frenzy
				if( pEnemy->GetAbnormalLevelBySkillId(176) > 0 )
				{
					damage *= g_FrenzyRaidBossDmgPenalty;
				}
			}
		}
	}

	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pEUD->tvtUser.status == TvT::UserFighting)
		{
			User *pEn = pEnemy->CastUser();
			if(!pEn && pEnemy->IsSummon())
			{
				pEn = pEnemy->CastSummon()->GetMaster();
			}
			if(pEn->ValidUser())
			{
				if(pEn->pEUD->tvtUser.status == TvT::UserFighting && pEn->pEUD->tvtUser.team == pUser->pEUD->tvtUser.team)
				{
					if(pEn->pEUD->tvtUser.friendlyFire == FALSE)
					{
						damage = 0;
					}
				}
			}
		}
	}

	double ret =  f(0x569908L)(pCreature, pEnemy, damage, param1, damageType, param2);

	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pSD->alive)
		{
			g_ItemAutoConsume.OnTiggerHp(pUser);
			g_ItemAutoConsume.OnTiggerCp(pUser);
		}
	}

	return ret;
}

void CreatureAction::MoveToLocation(CCreature *pCreature, int x, int y, int z, bool param)
{
	typedef void (*f)(CCreature*, int, int, int, bool);
	f(0x535FB4L)(pCreature, x, y, z, param);

	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}
	}
}

bool CreatureAction::ValidateSkillMod(CCreature *pCreature)
{
	guard;
	
	typedef bool (*f)(CCreature*);
	bool ret = f(0x554168L)(pCreature);

	if(pCreature->ValidCreature())
	{
		if(pCreature->criticalDamageM > g_MaxCriticalDamageMul)
		{
			pCreature->criticalDamageM = g_MaxCriticalDamageMul;
		}
		if(pCreature->criticalDamageA > g_MaxCriticalDamageAdd)
		{
			pCreature->criticalDamageA = g_MaxCriticalDamageAdd;
		}
	}

	unguard;
	return ret;
}

void CreatureAction::InitializeSkillMod(CCreature *pCreature)
{
	guard;
	if(User *pUser = pCreature->CastUser())
	{
		pUser->pEUD->blockBuffEx = FALSE;
		pUser->pEUD->blockBuffExPump = FALSE;
		pUser->pEUD->blockExpPump = FALSE;
		pUser->pEUD->bonusExp = 1.0;
		pUser->pEUD->bonusSp = 1.0;
		pUser->pEUD->bonusDrop = 1.0;
		pUser->pEUD->bonusSpoil = 1.0;
	}
	typedef void(*f)(CCreature*);
	f(0x54937CL)(pCreature);
	unguard;
}

bool CreatureAction::DispelByCategory(CCreature *pCreature, DebuffType debuffType, CCreature *pOther, UINT chance, CSkillInfo *pSI, UINT count)
{
	typedef bool (*f)(CCreature*, DebuffType, CCreature*, UINT, CSkillInfo*, UINT);

	if(User *pUser = pCreature->CastUser())
	{
		
		if(debuffType == DT_STORE_BUFF)
		{
			if(pUser->olympiadUser.IsInside())
			{
				return f(0x566F38L)(pCreature, DT_BUFF, pOther, chance, pSI, count);
			}

			if(!pUser->blockBuff && pUser->pSD->alive)
			{
				return pUser->StoreRandomBuffs(pOther, pSI, chance, count);
			}
			return false;
		}

		if(debuffType == DT_BUFF)
		{
			if(pUser->blockBuff)
			{
				return false;
			}
		}
	}

	return f(0x566F38L)(pCreature, debuffType, pOther, chance, pSI, count);
}

bool CreatureAction::DispelAllByGM(CCreature *pCreature, UINT skillPumpEndType)
{
	typedef bool (*f)(CCreature*, UINT);

	if(User *pUser = pCreature->CastUser())
	{
		pUser->DeleteStoredAbnormals();
	}

	return f(0x567438L)(pCreature, skillPumpEndType);
}

bool CreatureAction::DispelAll(CCreature *pCreature, int magicLevel, CCreature *pOther, CSkillInfo *pSI)
{
	typedef bool (*f)(CCreature*, int, CCreature*, CSkillInfo*);

	if(User *pUser = pCreature->CastUser())
	{
		pUser->DeleteStoredAbnormals();
	}

	return f(0x567200L)(pCreature, magicLevel, pOther, pSI);
}

bool CreatureAction::ResurrectBy(CCreature *pCreature, CCreature *pOther, INT64 exp)
{
	typedef bool (*f)(CCreature*, CCreature*, INT64);

	if(pCreature->ValidCreature())
	{
		pCreature->lpYieldLock->Enter(__FILEW__, __LINE__);

		if(pCreature->pSD->hp < 0)
		{
			pCreature->pSD->hp = 0;
		}
		if(pCreature->pSD->mp < 0)
		{
			pCreature->pSD->mp = 0;
		}
		if(pCreature->pSD->cp < 0)
		{
			pCreature->pSD->cp = 0;
		}

		pCreature->lpYieldLock->Leave();
	}

	bool ret = f(0x55CC2CL)(pCreature, pOther, exp);
	
	if(ret && pCreature->ValidCreature())
	{
		bool restored = false;
		pCreature->lpYieldLock->Enter(__FILEW__, __LINE__);

		double restoreHp = pCreature->pSD->max_hp * 0.7;
		double restoreCp = pCreature->pSD->max_cp * 0.7;
		if(pCreature->pSD->hp < restoreHp)
		{
			pCreature->pSD->hp = restoreHp;
			restored = true;
		}
		if(pCreature->pSD->cp < restoreCp)
		{
			pCreature->pSD->cp = restoreCp;
			restored = true;
		}

		pCreature->lpYieldLock->Leave();
	
		if(restored)
		{
			pCreature->UpdateStat(STAT_CP);
			pCreature->UpdateStat(STAT_HP);
		}
	}

	return ret;
}

void CreatureAction::CheckAbnormalVisualEffect(CCreature *pCreature)
{
	typedef void(*f)(CCreature*);
	f(0x54A098L)(pCreature);

	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pEUD->spawnProtection)
		{
			pUser->abnormalVisualEffect |= 0x200000;
		}
		if(pUser->pEUD->offlineShopTime)
		{
			pUser->abnormalVisualEffect |= 0x000080;
		}
	}
}


void CreatureAction::OutOfSight(CCreature *pCreature, CObject *pObject, bool param)
{
	guard;

	typedef void (*f)(CCreature*, CObject*, bool);

	if(User *pTarget = pObject->CastUser())
	{
		if(pCreature->ValidCreature())
		{
			if(pCreature->targetId == pTarget->serverId)
			{
				pCreature->DoNothing();
				pCreature->ChangeTarget(0, 2);
				pCreature->DoNothing();
				if(CSummon *pSummon = pCreature->CastSummon())
				{
					User *pMaster = pSummon->GetMaster();
					if(pMaster->ValidUser())
					{
						pMaster->BasicAction(23, false, false, 3);
					}
				}else if(CPet *pPet = pCreature->CastPet())
				{
					User *pMaster = pPet->GetMaster();
					if(pMaster->ValidUser())
					{
						pMaster->BasicAction(23, false, false, 3);
					}
				}
			}
		}
	}

	INT64 addr = (*(PINT64)pCreature);
	addr = (*(PINT64)(addr + 0x410));

	f((LPVOID)addr)(pCreature, pObject, param);
	unguard;
}

double CreatureAction::GetAttributeBonus(CCreature *pCreature, CObject *pObject, CSkillInfo *pSI)
{
	typedef double (*f)(CCreature*, CObject*, CSkillInfo*);
	double ret = f(0x53DE5CL)(pCreature, pObject, pSI);
	
	if(ret > 1)
	{
		ret = (((ret - 1 ) * g_AttributeAttackMultipler) + 1);
	}else if (ret < 1)
	{
		ret *= g_AttributeDefenceMultipler;
	}


	return ret;
}

bool CreatureAction::InfectedSkillBy(CCreature *pCreature, CCreature *pCaster, CSkillInfo *pSI, CSkillInfo *pSI2, double timeMultipler, int param)
{
	guard;
	if(!pCreature->Is(0x198) || pCreature->Is(0x3C0) || pSI->skillId == 5660)
	{
		if( pSI->slotType == AB_HP_RECOVER )
		{
			typedef bool (*f)(CCreature*, CCreature*, CSkillInfo*);
			bool ret = f(0x54A53CL)(pCreature, pCaster, pSI);

			unguard;
			return ret;
		}

		if(User *pUser = pCreature->CastUser())
		{
			if(pUser->pEUD->blockBuffExPump && pSI->debuffType == DT_BUFF && pSI->effectPoint > 0)
			{
				unguard;
				return false;
			}

			if(pUser->pEUD->blockBuffEx && pCreature!=pCaster && pSI->debuffType == DT_BUFF && pSI->effectPoint > 0)
			{
				unguard;
				return false;
			}
		}

		if( pCreature->CanBeBuffed(pSI->debuffType, pSI->slotType) || pSI->skillId == 4515 )
		{
			bool done = false;
			pCreature->abnormalDataLock.Enter(__FILEW__, __LINE__);
			typedef bool (*f)(CCreature*, CCreature*, CSkillInfo*, CSkillInfo*, double, int, bool&);
			bool ret = f(0x56AE48L)(pCreature, pCaster, pSI, pSI2, timeMultipler, param, done);
			if(ret)
			{
				if(g_AllInOne.Enabled())
				{
					if(User *pUser = pCaster->CastUser())
					{
						if(pUser->pEUD->enterWorldTime + 3 < time(0))
						{
							if(pUser->pEUD->aioUser.aioId > 0 && pUser->pSD->subjob_id == 0)
							{
								if(INT32 abnormalTime = g_AllInOne.GetAbnormalTime(pUser, pSI->skillId))
								{
									pCreature->SetNewAbnormalTime(pSI->skillId, pSI->level, abnormalTime, false, false);
								}							
							}
						}
					}
				}
			}
			pCreature->abnormalDataLock.Leave();

			unguard;
			return ret;
		}

	}
	unguard;
	return false;
}