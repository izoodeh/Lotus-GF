#include "StdAfx.h"
#include "AutoLearn.h"
#include "DB.h"
#include "HookManager.h"

CAutoLearn g_AutoLearn;

CAutoLearn::CAutoLearn()
{
	SetName(L"Auto Learn");
}

CAutoLearn::~CAutoLearn()
{

}

void CAutoLearn::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	WriteCall(0x55C682, CAutoLearn::AcquireSkills);	//CCreature::LevelUp
	WriteCall(0x560952, CAutoLearn::AcquireSkills);	//CCreature::EnterWorld
	WriteCall(0x56134B, CAutoLearn::AcquireSkills);	//CCreature::LevelDown
	WriteCall(0x90695D, CAutoLearn::AcquireSkills);	//User::ValidateSubjobParameters
	WriteCall(0x909910, CAutoLearn::AcquireSkills);	//User::OnChangeSubjobEnd

	WriteNOP(0x8F5812, 6);
}

void CAutoLearn::AcquireSkills(CCreature *pCreature, std::vector<SkillAcquireInfo*> &vSkills, bool checkAutoLearn, bool param)
{
	map<int, int> mToSave;
	if(User *pUser = pCreature->CastUser())
	{
		for(vector<SkillAcquireInfo*>::iterator Iter = vSkills.begin();Iter!=vSkills.end();Iter++)
		{
			SkillAcquireInfo *pInfo = (*Iter);
			if(pInfo)
			{
				int skillId = pInfo->skillId;
				int level = pInfo->level;
				if(pUser->GetAcquiredSkillLevel(skillId) < level)
				{
					mToSave[skillId] = level;
				}
			}
		}
	}

	typedef void(*f)(CCreature*, vector<SkillAcquireInfo*>&, bool, bool);
	f(0x554B1CL)(pCreature, vSkills, checkAutoLearn, param);

	if(User *pUser = pCreature->CastUser())
	{
		for(map<int, int>::iterator Iter = mToSave.begin();Iter!=mToSave.end();Iter++)
		{
			int skillId = Iter->first;
			int level = Iter->second;
			if(pUser->GetAcquiredSkillLevel(Iter->first) == level)
			{
				g_DB.SendAcquireSkillToDB(pUser->databaseId, skillId, level, false);
			}
		}
	}
}