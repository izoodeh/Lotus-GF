#pragma once

class CSkillReuseDb
{
	CSLock m_Lock;
	map<UINT, UserSkillReuse*> m_Data;
public:
	void OnEnterWorld(User *pUser);
};

extern CSkillReuseDb g_SkillReuseDb;