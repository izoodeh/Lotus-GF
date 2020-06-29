#include "StdAfx.h"
#include "PledgeSystem.h"
#include "HookManager.h"
#include "DB.h"
#include "PledgeMap.h"
#include "SkillAcquireDB.h"

UINT g_LoadedPledge = 0;
CPledgeMap* lpPledgeMap = (CPledgeMap*)0x10B949C0;
CRWLock* lpPledgeLock = (CRWLock*)0x10B09658;
LPUINT lpReservedPledgeCounter = (LPUINT)0x10B09650;
list<int> lReservedPledge;
BOOL g_AutoAcquirePledgeSkills = FALSE;
BOOL g_AutoAcquireSubPledgeSkills = FALSE;

void CPledgeSystem::Init()
{
	LPUINT lpPenalty = 0;
	const TCHAR* sectionName = TEXT("PledgeSystem");
	g_AutoAcquirePledgeSkills = GetPrivateProfileInt(sectionName, _T("AutoAcquirePledgeSkills"), 0, g_ConfigFile);
	g_AutoAcquireSubPledgeSkills = GetPrivateProfileInt(sectionName, _T("AutoAcquireSubPledgeSkills"), 0, g_ConfigFile);
	UINT value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_WAR_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(PLEDGE_WAR_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_OUST_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(PLEDGE_OUST_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_WITHDRAW_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(PLEDGE_WITHDRAW_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_OUSTED_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(PLEDGE_OUSTED_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_RECHALLENGE_PENALTY_TIMEOUT"), 432000, g_ConfigFile);
	lpPenalty[(PLEDGE_RECHALLENGE_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_RECHALLENGE_DESPITE_REJECTION_PENALTY_TIMEOUT"), 432000, g_ConfigFile);
	lpPenalty[(PLEDGE_RECHALLENGE_DESPITE_REJECTION_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_DISMISS_TIMEOUT"), 604800, g_ConfigFile);
	lpPenalty[(PLEDGE_DISMISS_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_DISMISS_PENALTY_TIMEOUT"), 864000, g_ConfigFile);
	lpPenalty[(PLEDGE_DISMISS_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("PLEDGE_DISMISS_PENALTY_TIMEOUT2"), 604800, g_ConfigFile);
	lpPenalty[(PLEDGE_DISMISS_PENALTY_TIMEOUT2/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("ALLIANCE_OUST_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(ALLIANCE_OUST_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("ALLIANCE_WITHDRAW_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(ALLIANCE_WITHDRAW_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("ALLIANCE_OUSTED_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(ALLIANCE_OUSTED_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("ALLIANCE_DISMISS_PENALTY_TIMEOUT"), 86400, g_ConfigFile);
	lpPenalty[(ALLIANCE_DISMISS_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("ALLIANCE_RECHALLENGE_PENALTY_TIMEOUT"), 432000, g_ConfigFile);
	lpPenalty[(ALLIANCE_RECHALLENGE_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("ALLIANCE_RECHALLENGE_DESPITE_REJECTION_PENALTY_TIMEOUT"), 432000, g_ConfigFile);
	lpPenalty[(ALLIANCE_RECHALLENGE_DESPITE_REJECTION_PENALTY_TIMEOUT/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("CASTLE_ANNOUNCE_TIME"), 86400, g_ConfigFile);
	lpPenalty[(CASTLE_ANNOUNCE_TIME/4)] = value;
	value = GetPrivateProfileInt(sectionName, TEXT("CASTLE_STANDBY_TIME"), 86400, g_ConfigFile);
	lpPenalty[(CASTLE_STANDBY_TIME/4)] = value;

	//Fix for loading pledge
	WriteCall(0x6B2661, CPledgeSystem::RequestStartLoadPledgesJob);
	WriteCall(0x6915D3, CPledgeSystem::LoadPledgeJob);
	WriteCall(0x7CAE08, CPledgeSystem::RequestLoadPledgeReserved);

	//AutoLearn skills
	WriteCall(0x5A7C53, CPledgeSystem::OnCreateSubPledge);
}

void CPledgeSystem::RequestLoadPledgeReserved(LPVOID lpInstance, int pledgeId)
{
	if(lpPledgeMap->ExistKey(pledgeId))
	{
		lReservedPledge.push_back(pledgeId);
	}else
	{
		g_DB.RequestLoadPledge(pledgeId);
	}	
}

void CPledgeSystem::RequestStartLoadPledgesJob(LPVOID lpObject, int id)
{
	typedef void (*f)(LPVOID, int);
	f(0x691080L)(lpObject, id);

	g_Log.Add(CLog::Blue, "Synchronizing load pledge job");
	HANDLE hMutex = NULL;
	UINT nTick = 0;
	while(!hMutex && nTick < (11*600))
	{
		Sleep(100);
		lReservedPledge.sort();
		lReservedPledge.unique();
		for(list<int>::iterator Iter=lReservedPledge.begin();Iter!=lReservedPledge.end();Iter++)
		{

			typedef void (*f)(int);
			f(0x7D0D34L)((*Iter));
		}
		lReservedPledge.clear();

		hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("PLEDGE_LOAD_JOB_DONE"));
		nTick++;
	}
	if(hMutex)
	{
		Sleep(500);
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
	g_Log.Add(CLog::Blue, "Load pledge job done");
}

void CPledgeSystem::LoadPledgeJob()
{
	typedef void(*f)();
	f(0x7D831CL)();

	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("PLEDGE_LOAD_JOB_DONE"));
	if(hMutex)
	{
		Sleep(500);
		CloseHandle(hMutex);
	}
}

void CPledgeSystem::OnCreateSubPledge(CPledge *pPledge, UINT pledgeType, WCHAR *wMaster, int param, bool param2)
{
	typedef void (*f)(CPledge*, UINT, WCHAR*, int, bool);
	f(0x7D34CCL)(pPledge, pledgeType, wMaster, param, param2);

	if(g_AutoAcquireSubPledgeSkills)
	{
		for(INT32 n=0;n <= pPledge->pSD->level;n++)
		{
			pPledge->AutoAcquireSubPledgeSkills(n);
		}
	}
}