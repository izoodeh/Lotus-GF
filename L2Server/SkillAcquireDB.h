#pragma once

#pragma once

struct SkillAcquireInfoEx
{
	SkillPchId skill;
	INT32 requiredLevel;
};

class CSkillAcquireDbEx
{
	map<wstring, UINT> m_ClassId;
	vector<SkillPchId> m_AcquirableSkills;
	vector<SkillPchId> m_MaxAcquirableLevel;
	map<UINT, vector<SkillAcquireInfoEx>> m_ClassSkills;
	map<UINT, vector<SkillAcquireInfoEx>> m_SharedSkills;
	map<wstring, SkillPchId> m_SkillPch;
	void LoadData();
	void LoadSkillPch();
public:
	void Init();
	UINT GetClassByName(wstring name);
	bool GetSkillIdByName(wstring name, SkillPchId& pchId);
	bool IsAcquirableSkill(int skillId, int level);
	UINT GetMaxAcquirableSkillLevel(int skillId);
	bool CanAcquire(UINT classId, int skillId, int skillLevel, int level);
	bool IsSharedSkill(UINT classId, int skillId, int skillLevel);
	int GetMaxAvailableLevel(UINT classId, int skillId, int level);
	wstring GetSkillNameById(SkillPchId pch);
};

extern CSkillAcquireDbEx g_SkillAcquireDbEx;

class CSkillAcquireDB
{
	CSkillAcquireDB* lpInstance;
public:
	CSkillAcquireDB();
	~CSkillAcquireDB();
	const vector<SkillAcquireInfo*>& GetAcquirableEtcSkills(EtcSkillAcquireType type);
	const vector<SkillAcquireInfo*>& GetAcquirableSkills(UINT classType);
};

extern CSkillAcquireDB g_SkillAcquireDB;