#include "StdAfx.h"
#include "Pledge.h"
#include "SkillAcquireDB.h"
#include "HookManager.h"

extern BOOL g_AutoAcquirePledgeSkills;
extern BOOL g_AutoAcquireSubPledgeSkills;

void CPledge::ExtInit()
{

	WriteCall(0x44549F, CPledge::SetPledgeLevel);
	WriteCall(0x4B3252, CPledge::SetPledgeLevel);
	WriteCall(0x5E73C1, CPledge::SetPledgeLevel);
	WriteCall(0x73B980, CPledge::SetPledgeLevel);
	WriteCall(0x7DBB91, CPledge::SetPledgeLevel);
}

CPledge* CPledge::GetPledge(UINT sid)
{
	typedef CPledge* (*f)(UINT);
	return f(0x7C8EB8L)(sid);
}

void CPledge::AcquirePledgeSkillAndSave(int skillId, int level, bool param, bool param2)
{
	guard;
	typedef void(*f)(CPledge*, int, int , bool, bool);
	f(0x7D9D44L)(this, skillId, level, param, param2);
	unguard;
}

void CPledge::SetPledgeLevel(CPledge *pPledge, INT32 level, bool param)
{
	typedef void(*f)(CPledge*, int, bool);
	f(0x7DB690L)(pPledge, level, param);

	if(g_AutoAcquirePledgeSkills)
	{
		pPledge->AutoAcquirePledgeSkills(level);
	}
	if(g_AutoAcquireSubPledgeSkills)
	{
		pPledge->AutoAcquireSubPledgeSkills(level);
	}
}

void CPledge::AcquireSubPledgeSkillAndSave(UINT pledgeType, int skillId, int level, bool param, bool param2)
{
	typedef void(*f)(CPledge*, UINT, int, int, bool, bool);
	f(0x7DAC08L)(this, pledgeType, skillId, level, param, param2);
}

void CPledge::AutoAcquirePledgeSkills(int pledgeLevel)
{
	vector<SkillAcquireInfo*> vSkills = g_SkillAcquireDB.GetAcquirableEtcSkills(EtcSkillAcquirePledge);
	for(UINT n=0;n<vSkills.size();n++)
	{
		if(pledgeLevel == vSkills[n]->requiredLevel)
		{
			g_Log.Add(CLog::Blue, "Pledge acquire skill [%d] [%d]", vSkills[n]->skillId, vSkills[n]->level);
			AcquirePledgeSkillAndSave(vSkills[n]->skillId, vSkills[n]->level, 0, 1);
		}
	}
}

void CPledge::AutoAcquireSubPledgeSkills(int pledgeLevel)
{
	vector<SkillAcquireInfo*> vSkills = g_SkillAcquireDB.GetAcquirableEtcSkills(EtcSkillAcquireSubPledge);
	for(UINT n=0;n<vSkills.size();n++)
	{
		if(pledgeLevel == vSkills[n]->requiredLevel && HasSubPledgeType(vSkills[n]->requiredPledgeType))
		{
			g_Log.Add(CLog::Blue, "SubPledge acquire skill [%d] [%d] pledgeType", vSkills[n]->skillId, vSkills[n]->level, vSkills[n]->requiredPledgeType);
			AcquireSubPledgeSkillAndSave(vSkills[n]->requiredPledgeType, vSkills[n]->skillId, vSkills[n]->level, 0, 1);
		}
	}
}

bool CPledge::HasSubPledgeType(UINT pledgeType)
{
	typedef bool (*f)(CPledge*, UINT);
	return f(0x7CEB08L)(this, pledgeType);
}

bool CPledge::InWar(UINT sid)
{
	guard;
	bool ret = false;
	pLock->Enter(__FILEW__,__LINE__);
	for(map<UINT, UINT>::iterator it = m_WarList.begin();it!=m_WarList.end();it++)
	{
		if(it->first == sid)
		{
			ret = true;
			break;
		}
	}
	pLock->Leave();
	unguard;
	return ret;
}