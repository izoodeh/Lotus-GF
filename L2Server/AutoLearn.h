#pragma once

class CAutoLearn : public CFeature
{
public:
	CAutoLearn();
	~CAutoLearn();
	void Init();
	static void AcquireSkills(CCreature *pCreature, vector<SkillAcquireInfo*> &vSkills, bool checkAutoLearn, bool param);
};

extern CAutoLearn g_AutoLearn;