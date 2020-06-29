#include "stdafx.h"
#include "AuthStat.h"
#include "HookManager.h"

CAuthStat g_AuthStat;

LPINT g_lpUserLimit = (LPINT)0x163414C;
LPINT g_lpUserConnected = (LPINT)0x163413C;


CAuthStat::CAuthStat() : baseUserCount(0), userCountMultipler(1.0)
{
	SetName(L"Auth Stat");
}

CAuthStat::~CAuthStat()
{
}

void CAuthStat::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	LoadINI();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled [%d][%f]", GetName(), baseUserCount, userCountMultipler);
		//set hooks
		WriteCall(0x456173, CAuthStat::AuthUserNum);
		WriteCall(0x57ED15, CAuthStat::AuthUserNum);
	}else
	{
		baseUserCount = 0;
		userCountMultipler = 1.0;
	}
}

void CAuthStat::LoadINI()
{
	enabled = GetPrivateProfileInt(_T("AuthStat"), _T("Enabled"), 0, g_ConfigFile);
	baseUserCount = GetPrivateProfileInt(_T("AuthStat"), _T("BaseUserCount"), 0, g_ConfigFile);
	userCountMultipler = GetPrivateProfileDouble(_T("AuthStat"), _T("UserCountMultipler"), 1.0, g_ConfigFile);
}

bool CAuthStat::AuthUserNum(LPVOID lpInstance, int userCount, int userLimit)
{
	typedef bool (*f)(LPVOID, int, int);

	if(g_AuthStat.enabled)
	{
		if(userCount > 0)
		{
			userCount += g_AuthStat.baseUserCount;
			double value = (double)userCount;
			value *= g_AuthStat.userCountMultipler;
			userCount = (int)value;
		}
	}

	return f(0x57EBB4)(lpInstance, userCount, userLimit);
}