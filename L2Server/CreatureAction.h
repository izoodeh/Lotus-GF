#pragma once

class CreatureAction
{
public:
	static void Init();
	static double GetAttributeBonus(CCreature* pCreature, CObject* pObject, CSkillInfo* pSI);
	static bool InfectedSkillBy(CCreature *pCreature, CCreature *pCaster, CSkillInfo *pSI, CSkillInfo *pSI2, double timeMultipler, int param);
	static void OutOfSight(CCreature *pCreature, CObject *pObject, bool param);
	static void CheckAbnormalVisualEffect(CCreature *pCreature);
	static bool ResurrectBy(CCreature *pCreature, CCreature *pOther, INT64 exp);
	static bool DispelAll(CCreature *pCreature, int magicLevel, CCreature *pOther, CSkillInfo *pSI);
	static bool DispelAllByGM(CCreature *pCreature, UINT skillPumpEndType);
	static bool DispelByCategory(CCreature *pCreature, DebuffType debuffType, CCreature *pOther, UINT chance, CSkillInfo *pSI, UINT count);
	static void InitializeSkillMod(CCreature *pCreature);
	static bool ValidateSkillMod(CCreature *pCreature);
	static void MoveToLocation(CCreature* pCreature, int x, int y, int z, bool param);
	static double GotDamageBy(CCreature *pCreature, CCreature *pEnemy, double damage, bool param1, UINT damageType, bool param2);
	static bool CanActivateSkill(CSkillInfo *pSI, CCreature *pCaster, CObject *pTarget);
	static bool UseItem(CCreature *pCreature, CItem *pItem, int param);
	static int CalcReuseDelay(CCreature *pCreature, CSkillInfo *pSI);
	static double GetMagicCritical(CCreature *pCreature, CObject *pObject, double critRateParam);
	static double GetRandomMagicDamage(CCreature *pCreature);
	static int GetRemainReuseDelaySec(CCreature *pCreature, int skillId);
	static double GetLevelParam(INT32 level, INT32 level2);
	static void SetItemUsableTime(CCreature *pCreature, int itemId, ULONG* pStamp, ULONG reuse);
	static bool CheckAttackValidity(CCreature *pCreature, CObject *pTarget);
	static void AddOrUpdateSkillReuseDelay(CCreature *pCreature, int skillId, UINT subjobId, int reuseDelay, UINT reuseType);
	static UINT GetSkillUsedTime(CCreature *pCreature, int skillId);
};