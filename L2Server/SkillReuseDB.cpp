#include "stdafx.h"
#include "SkillReuseDB.h"

CSkillReuseDb g_SkillReuseDb;


//TODO:
//Add UserSkillReuse pointer to ExtUserData on EnterWorld - make creature action to use these functions
//Add OnEnterWorld to CSkillReuseDb

void CSkillReuseDb::OnEnterWorld(User *pUser)
{
	guard;
	UserSkillReuse *pUSR = 0;
	m_Lock.Enter();
	map<UINT, UserSkillReuse*>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		pUSR = it->second;
	}else
	{
		pUSR = new UserSkillReuse;
		m_Data[pUser->databaseId] = pUSR;
	}
	m_Lock.Leave();

	if(pUSR)
	{
		pUser->pEUD->pUSR = pUSR;
		UINT currentTick = GetTickCount();
		pUSR->m_Lock.Enter();
		//Update/Add reuse for static reuse skillls
		map<INT32, UINT>::iterator it = pUSR->m_Reuse[4].begin();
		for(;it!=pUSR->m_Reuse[4].end();it++)
		{
			if(it->second > currentTick)
			{
			//	g_Log.Add(CLog::Blue, "[%s] Restoring skill[%d] reuse[%d]", __FUNCTION__, it->first, it->second - currentTick);
				typedef void(*f)(CCreature*, int, UINT, int, UINT);
				f(0x549AF8L)(pUser, it->first, 4, (it->second - currentTick), 1);	//CCreature::AddOrUpdateSkillReuseDelay
			}
		}
		//update/add reuse for normal reuse skills
		for(UINT n=0;n<5;n++)
		{
			for(it = pUSR->m_Reuse[n].begin(); it!=pUSR->m_Reuse[n].end();it++)
			{
			//	g_Log.Add(CLog::Blue, "[%s] skill[%d] reuse[%d]", __FUNCTION__, it->first, it->second);
				if(it->second > currentTick)
				{
			//		g_Log.Add(CLog::Blue, "[%s] Restoring skill[%d] reuse[%d]", __FUNCTION__, it->first, it->second - currentTick);
					typedef void(*f)(CCreature*, int, UINT, int, UINT);
					f(0x549AF8L)(pUser, it->first, n, (it->second - currentTick), 0);	//CCreature::AddOrUpdateSkillReuseDelay
				}
			}
		}
		pUSR->m_Lock.Leave();
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Cannot allocate UserSkillReuse for user[%d] - low memory!", __FUNCTION__, pUser->databaseId);
	}

	unguard;
}