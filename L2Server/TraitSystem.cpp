#include "stdafx.h"
#include "TraitSystem.h"
#include "HookManager.h"

CTraitSystem g_TraitSystem;

BOOL g_CustomTraitSystem = FALSE;

CTraitSystem::CTraitSystem()
{
}

CTraitSystem::~CTraitSystem()
{
}

void CTraitSystem::Init()
{
	g_CustomTraitSystem = GetPrivateProfileInt(_T("Setting"), _T("CustomTraitSystem"), 0, g_ConfigFile);
	if(g_CustomTraitSystem)
	{
		UINT traitTypeArray = static_cast<UINT>(reinterpret_cast<UINT64>(&m_traitType[0]));

		PUINT lpOrgTraitType = (PUINT)0xE5DBA0;
		for(UINT n=0;n<42;n++)
		{
			m_traitType[n] = lpOrgTraitType[n];
		}
		m_traitType[TRAIT_WATER] = 2;
		m_traitType[TRAIT_FIRE] = 2;
		m_traitType[TRAIT_WIND] = 2;
		m_traitType[TRAIT_EARTH] = 2;
		m_traitType[TRAIT_HOLY] = 2;
		m_traitType[TRAIT_UNHOLY] = 2;

		WriteQWORD(0xCE93A8, (UINT64)CTraitSystem::CSkillEffect_p_attack_trait_Pump);
		WriteQWORD(0xC132B8, (UINT64)CTraitSystem::CSkillEffect_p_defence_trait_Pump);
		WriteNOP(0x549783, 102);
		WriteCall(0x5497EC, CTraitSystem::CCreature_InitializeSkillMod);

		WriteCall(0x53EC5E, CTraitSystem::CCreature_GetGeneralTraitBonus);
		WriteCall(0x53FE24, CTraitSystem::CCreature_GetGeneralTraitBonus);
		WriteCall(0x55EC6B, CTraitSystem::CCreature_GetGeneralTraitBonus);
		WriteCall(0x6FF5E2, CTraitSystem::CCreature_GetAttackTraitBonus);

		WriteCall(0x53FDA7, CTraitSystem::CCreature_GetWeaponTraitBonus);
		WriteCall(0x81F770, CTraitSystem::CCreature_GetWeaponTraitBonus);
		WriteCall(0x84586C, CTraitSystem::CCreature_GetWeaponTraitBonus);
		WriteCall(0x8461DB, CTraitSystem::CCreature_GetWeaponTraitBonus);
		WriteCall(0x846B87, CTraitSystem::CCreature_GetWeaponTraitBonus);
		WriteCall(0x846F64, CTraitSystem::CCreature_GetWeaponTraitBonus);
		WriteCall(0x847E52, CTraitSystem::CCreature_GetWeaponTraitBonus);

		//xarray::getelementb
		WriteBYTE(0x53E14F, static_cast<BYTE>(TRAIT_COUNT));	//.text:000000000053E14C                 cmp     rdx, 42
		//xarray::getelementd
		WriteBYTE(0x53E1E7, static_cast<BYTE>(TRAIT_COUNT));	//.text:000000000053E1E4                 cmp     rdx, 42

		g_Log.Add(CLog::Blue, "Initializing Trait System");
	}
}

bool CTraitSystem::CSkillEffect_p_attack_trait_Pump(CSkillEffect_p_attack_trait* pEffect, CCreature *pCreature, CSkillInfo *pInfo, int param)
{
	guard;

	if(pEffect->m_traitType < TRAIT_COUNT)
	{
		pCreature->pECD->attackTrait[pEffect->m_traitType] = true;
		double value = pEffect->m_value / 100.0;
		pCreature->pECD->attackTraitValue[pEffect->m_traitType] += value;
	//	g_Log.Add(CLog::Blue, "[%s] trait[%d] value[%f]", __FUNCTION__, pEffect->m_traitType, value);
	}

	unguard;
	return true;
}

bool CTraitSystem::CSkillEffect_p_defence_trait_Pump(CSkillEffect_p_defence_trait *pEffect, CCreature *pCreature, CSkillInfo *pInfo, int param)
{
	guard;
	if(pEffect->m_traitType < TRAIT_COUNT)
	{
		pCreature->pECD->defenceTrait[pEffect->m_traitType] = true;
		if(pEffect->m_param)
		{
			pCreature->pECD->perfectDefenceTrait[pEffect->m_traitType] = true;
		}

		double value = pEffect->m_value / 100.0;
		pCreature->pECD->defenceTraitValue[pEffect->m_traitType] += value;
	//	g_Log.Add(CLog::Blue, "[%s] trait[%d] value[%f]", __FUNCTION__, pEffect->m_traitType, value);
	}
	unguard;
	return true;
}

void CTraitSystem::CCreature_InitializeSkillMod(CCreature *pCreature)
{
	guard;

	if(pCreature->pECD)
	{
		for(UINT n=0;n<TRAIT_COUNT;n++)
		{
			pCreature->pECD->attackTrait[n] = false;
			pCreature->pECD->defenceTrait[n] = false;
			pCreature->pECD->perfectDefenceTrait[n] = false;
			pCreature->pECD->attackTraitValue[n] = 1.0;
			pCreature->pECD->defenceTraitValue[n] = 1.0;
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] null ExtCreatureData!", __FUNCTION__);
	}

	typedef void(*f)(CCreature*);
	f(0x53A3C4L)(pCreature);

	unguard;
}

double CTraitSystem::CCreature_GetGeneralTraitBonus(CCreature *pCreature, CCreature *pOther, UINT trait, bool param)
{
	guard;

	double bonus = 1.0;
	if(trait < TRAIT_COUNT)
	{
		if(pOther->pECD->perfectDefenceTrait[trait])
		{
	//		g_Log.Add(CLog::Blue, "[%s] trait[%d] - perfect def", __FUNCTION__, trait);
			unguard;
			return 0.0;
		}

		UINT traitType = g_TraitSystem.m_traitType[trait];
		if(traitType == 2 || (traitType == 3 && !param))
		{
			if(pCreature->pECD->attackTrait[trait] || !param)
			{
				if(pOther->pECD->defenceTrait[trait])
				{
					if(param)
					{
						bonus = pCreature->pECD->attackTraitValue[trait];
						double defence = pOther->pECD->defenceTraitValue[trait];

						if(defence > 1.0)
						{
							//resist - deal less dmg
							defence = defence - 1.0;	//1.3 -> 0.3 = 30% resist
							bonus = bonus - 1.0;		//2.0 -> 1.0 or 1.5 -> 0.5
							defence *= bonus;			//0.3 -> 0.3 or 0.3 -> 0.15
							bonus = 1.0 - defence;		//0.7 or 0.85
						}else
						{
							//weakness - deal more dmg
							defence = 1.0 - defence;	//0.7 -> 0.3
							bonus -= 1.0;				//2.0 -> 1.0 or 1.5 -> 0.5
							defence *= bonus;			//0.3 or 0.15
							bonus = 1.0 + defence;		//1.3 or 1.15
						}
						
					}else
					{
						bonus -= pOther->pECD->defenceTraitValue[trait];
						bonus += 1.0;
					}

					if(bonus < 0.05)
					{
						bonus = 0.05;
					}else if(bonus > 2.0)
					{
						bonus = 2.0;
					}else if(bonus < 0)
					{
						bonus = 0.0;
					}
				}
			}
		}
	//	g_Log.Add(CLog::Blue, "[%s] trait[%d] type[%d] bonus[%f]", __FUNCTION__, trait, traitType, bonus);
	}

	unguard;

	return bonus;
}

double CTraitSystem::CCreature_GetAttackTraitBonus(CCreature *pCreature, CCreature *pOther)
{
	guard;
	double bonus = 1.0;

	double weaponBonus = CCreature_GetWeaponTraitBonus(pOther, pCreature->pSD->attack_type);
	if(weaponBonus >= 0.0)
	{
		for(UINT trait = 9;trait<TRAIT_COUNT;trait++)
		{
			if(g_TraitSystem.m_traitType[trait] == 2)
			{
				if(pCreature->pECD->attackTrait[trait])
				{
					if(pOther->pECD->defenceTrait[trait])
					{
						bonus *= CCreature_GetGeneralTraitBonus(pCreature, pOther, trait, true);
					}
				}
			}
		}

		if(bonus > 0.0)
		{
			bonus *= weaponBonus;
			if(bonus < 0.05)
			{
				bonus = 0.05;
			}else if(bonus > 2.0)
			{
				bonus = 2.0;
			}
		}else
		{
			bonus = 0.0;
		}
	}else
	{
		bonus = 0.0;
	}

//	g_Log.Add(CLog::Blue, "[%s] weaponBonus[%f] bonus[%f]", __FUNCTION__, weaponBonus, bonus);

	unguard;

	return bonus;
}

double CTraitSystem::CCreature_GetWeaponTraitBonus(CCreature *pCreature, UINT attackType)
{
	guard;
	double bonus = 1.0;

	switch(attackType)
	{
	case TRAIT_SWORD:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_SWORD];
			break;
		}
	case TRAIT_DAGGER:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_DAGGER];
			break;
		}
	case TRAIT_BLUNT:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_BLUNT];
			break;
		}
	case TRAIT_POLE:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_POLE];
			break;
		}
	case TRAIT_FIST:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_FIST];
			break;
		}
	case TRAIT_BOW:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_BOW];
			break;
		}
	case TRAIT_ETC:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_ETC];
			break;
		}
	case TRAIT_DUAL:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_DUAL];
			break;
		}
	case TRAIT_DUALFIST:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_DUALFIST];
			break;
		}
	case TRAIT_RAPIER:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_RAPIER];
			break;
		}
	case TRAIT_CROSSBOW:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_CROSSBOW];
			break;
		}
	case TRAIT_ANCIENTSWORD:
		{
			bonus = pCreature->pECD->defenceTraitValue[TRAIT_ANCIENTSWORD];
			break;
		}
	};

	if(bonus != 1.0)
	{
		bonus -= 1.0;
		bonus = 1.0 - bonus;
	}

	unguard;
	return bonus;
}