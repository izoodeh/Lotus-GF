#pragma once
#include "PumpEffect.h"

class CTraitSystem
{
	UINT m_traitType[TRAIT_COUNT];
public:
	CTraitSystem();
	~CTraitSystem();
	void Init();
	static bool CSkillEffect_p_attack_trait_Pump(CSkillEffect_p_attack_trait* pEffect, CCreature *pCreature, CSkillInfo *pInfo, int param);
	static bool CSkillEffect_p_defence_trait_Pump(CSkillEffect_p_defence_trait* pEffect, CCreature *pCreature, CSkillInfo *pInfo, int param);
	static void CCreature_InitializeSkillMod(CCreature *pCreature);
	static double CCreature_GetGeneralTraitBonus(CCreature *pCreature, CCreature *pOther, UINT trait, bool param);
	static double CCreature_GetAttackTraitBonus(CCreature *pCreature, CCreature *pOther);
	static double CCreature_GetWeaponTraitBonus(CCreature *pCreature, UINT attackType);
};

extern CTraitSystem g_TraitSystem;