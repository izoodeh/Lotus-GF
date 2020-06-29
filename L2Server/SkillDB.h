#pragma once
#include "SkillInfo.h"

class CSkillDB
{
	CSkillDB* lpInstance;
public:
	CSkillDB();
	~CSkillDB();
	CSkillInfo* GetSkillInfo(int skillid, int level, CCreature* pCreature = 0);
};

extern CSkillDB g_SkillDB;