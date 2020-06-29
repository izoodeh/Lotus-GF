#include "StdAfx.h"
#include "SkillDB.h"

CSkillDB g_SkillDB;

CSkillDB::CSkillDB()
{
	lpInstance = (CSkillDB*)0x10F69440;
}

CSkillDB::~CSkillDB()
{
}

CSkillInfo* CSkillDB::GetSkillInfo(int skillId, int level, CCreature *pCreature)
{
	CSkillKey key(skillId, level);
	typedef CSkillInfo* (*f)(CSkillDB*, CSkillKey*, CCreature*);
	return f(0x8216C0L)(lpInstance, &key, pCreature);
}

void CSkillInfo::ActivateSkill(CCreature *pCaster, CObject *pTarget, double smth, CSkillAction2 *pAction2, EffectActivateTiming timing, double smth2)
{
	typedef void(*f)(CSkillInfo*, CCreature*, CObject*, double, CSkillAction2*, EffectActivateTiming, double);
	f(0x81F3F0L)(this, pCaster, pTarget, smth, pAction2, timing, smth2);
}