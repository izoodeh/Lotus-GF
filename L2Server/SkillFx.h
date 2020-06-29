#pragma once
#include "SkillEffect.h"

extern "C"
{
	void CSkillEffect_i_dispel_by_category_new_slot_type();
	void AugmentationGlowBlockUserInfo();
	void AugmentationGlowBlockCharInfo();
	void CheckAllianceHook();
}

class CSkillFx : public CFeature
{
	UINT antiBuffSkillId;
	BOOL stealOnlySelfbuff;
public:
	CSkillFx();
	~CSkillFx();
	void Init();
	static double GetDispelChance(double baseChance, CSkillInfo* pSIBuff, CSkillInfo* pSI, LPVOID lpSkillMod);
	static bool CSkillEffect_p_block_buff_Pump(CSkillEffect* pEffect, CCreature* pCreature, CSkillInfo* pSI, int param);
	static bool CanBeStealed(AbnormalData& ad, UINT type);
	static void UnequipWeapon(CCreature *pCreature);	//p_disarm fix
};

extern CSkillFx g_SkillFx;