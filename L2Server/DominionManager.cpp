#include "StdAfx.h"
#include "DominionManager.h"
#include "HookManager.h"

CDominionManager g_DominionManager;

CDominionManager::CDominionManager()
{
	m_GlobalState = reinterpret_cast<PUINT>(0x226F560L);
	m_StartTime = reinterpret_cast<PUINT>(0x226F534L);
	m_PrevEndTime = reinterpret_cast<PUINT>(0x226F530L);
	m_Period = reinterpret_cast<PUINT>(0xE5AC34L);
	m_Instance = reinterpret_cast<PVOID>(0x226F4C0L);
	m_ElapsedTime = reinterpret_cast<PUINT>(0x226F548L);
}

void CDominionManager::Init()
{
	LoadINI();
	if(m_Enabled)
	{
		g_Log.Add(CLog::Blue, "[%s] Feature is enabled. FirstSiegeTime[%d].", __FUNCTION__, m_FirstSiegeTime);
		WriteCall(0x5F51F4, CDominionManager::AdjustNextSiegeTime);
		WriteDWORD(0xB196A8, static_cast<UINT>(reinterpret_cast<UINT64>(CDominionManager::TimerExpired)));
		//bind cached packet
		typedef void(*f)(UINT64, UINT, PVOID);
		f(0x5A0BD8L)(0x1633390, 0x6E, CDominionManager::ReplyLoadDominionSiege);
		m_CheckForRegistration = FALSE;

	}
}

void CDominionManager::LoadINI()
{
	memset(&m_FirstSiegeTM, 0, sizeof(m_FirstSiegeTM));
	m_FirstSiegeTime = 0;

	const TCHAR* section = _T("DominionManager");
	m_Enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, g_ConfigFile);
	m_CycleTime = GetPrivateProfileInt(section, _T("CycleTime"), 1209600, g_ConfigFile);
	//2013/07/06 18:00:00
	TCHAR temp[8190] = { 0 };
	if(GetPrivateProfileString(section, _T("FirstSiegeTime"), 0, temp, 8190, g_ConfigFile))
	{
		wstring date = Utils::ReplaceString(temp, L"/", L" ", true);
		date = Utils::ReplaceString(date, L":", L" ", true);
		wstringstream dateStream(date);
		int year, month, day, hour, minute, second;
		if(dateStream >> year >> month >> day >> hour >> minute >> second)
		{
			time_t rawtime = 0;
			time ( &rawtime );
			localtime_s(&m_FirstSiegeTM, &rawtime);
			m_FirstSiegeTM.tm_year = year - 1900;
			m_FirstSiegeTM.tm_mon = month - 1;
			m_FirstSiegeTM.tm_mday = day;
			m_FirstSiegeTM.tm_hour = hour;
			m_FirstSiegeTM.tm_min = minute;
			m_FirstSiegeTM.tm_sec = second;

			m_FirstSiegeTime = static_cast<UINT>(mktime ( &m_FirstSiegeTM ));
		}
	}
}
void CDominionManager::ChangeState(UINT state, bool dbSave)
{
	typedef void(*f)(PVOID, UINT, bool);
	f(0x5EA100L)(m_Instance, state, dbSave);
}

void CDominionManager::SetSiegeEndTime(UINT value, bool dbSave)
{
	typedef void(*f)(PVOID, UINT, bool);
	f(0x5E9DA8L)(m_Instance, value, dbSave);
}

void CDominionManager::SetNextSiegeTime(UINT value, bool dbSave)
{
	typedef void(*f)(PVOID, UINT, bool);
	f(0x5EA038L)(m_Instance, value, dbSave);
}

void CDominionManager::StartTimer()
{
	typedef void(*f)(PVOID);
	f(0x5E9D00L)(m_Instance);
}

UINT CDominionManager::CheckStartDominionRegistration(UINT param, bool paramEx)
{
	typedef UINT (*f)(PVOID, UINT, bool);
	return f(0x5F5318L)(m_Instance, param, paramEx);
}

UINT CDominionManager::AdjustNextSiegeTime()
{
	time_t currentTime = time(0);
	UINT lastSiegeTime = g_DominionManager.GetPrevEndTime();

	UINT siegeTime = g_DominionManager.m_FirstSiegeTime;
	if(siegeTime > 0)
	{
		while(siegeTime < currentTime)
		{
			siegeTime += g_DominionManager.m_CycleTime;
		}
		if(lastSiegeTime == 0)
		{
			g_DominionManager.SetSiegeEndTime((siegeTime - g_DominionManager.m_CycleTime), false);
		}
	}
	g_Log.Add(CLog::Blue, "[%s] current[%d] siegeTime[%d]", __FUNCTION__, currentTime, siegeTime);
	return siegeTime;
}

bool CDominionManager::ReplyLoadDominionSiege(CDBSocket *pSocket, const unsigned char* packet)
{
	guard;

	UINT siegeState, prevSiegeEndTime, siegeElapsedTime, nextSiegeTime;
	Disassemble(packet, "dddd", &siegeState, &prevSiegeEndTime, &siegeElapsedTime, &nextSiegeTime);


	g_DominionManager.ChangeState(siegeState, false);
	time_t currentTime = time(0);

	g_DominionManager.SetSiegeEndTime(prevSiegeEndTime, false);
	g_DominionManager.SetElapsedTime(siegeElapsedTime);

	g_Log.Add(CLog::Blue, "[%s] NextSiegeTime[%d], PrevSiegeEndTime[%d], SiegeState[%d]", __FUNCTION__, nextSiegeTime, prevSiegeEndTime, siegeState);

	if(nextSiegeTime > 0 && (nextSiegeTime + 172800) > currentTime )
	{
		g_DominionManager.SetNextSiegeTime(nextSiegeTime, false);
	}else
	{
		nextSiegeTime = g_DominionManager.AdjustNextSiegeTime();
		g_DominionManager.SetNextSiegeTime(nextSiegeTime, true);
	}

	//typedef void(*f)(CDBSocket*, const unsigned char*);
	//f(0x5A0890L)(pSocket, packet);

	g_DominionManager.StartTimer();

	unguard;
	return false;
}

void CDominionManager::TimerExpired(PVOID lpInstance, UINT id)
{
	guard;

	typedef void(*f)(PVOID, UINT);
	f(0x5F6194L)(lpInstance, id);

	if(g_DominionManager.GetGlobalState() == 1 || g_DominionManager.GetGlobalState() == 0)
	{
		time_t currentTime = time(0);
		UINT startTime = g_DominionManager.GetStartTime();
		if(startTime > currentTime && (currentTime + (g_DominionManager.m_CycleTime - 172800)) > startTime)
		{
			//registration should be open by now
			if(g_DominionManager.CheckStartDominionRegistration(0, true) == 1)
			{
				if(g_DominionManager.GetGlobalState() < 2)
				{
					g_DominionManager.ChangeState(2, true);
				}
				g_Log.Add(CLog::Blue, "[%s] Changed state to 2 and opened the registration.", __FUNCTION__);
			}
		}else if( (startTime + 172800) < currentTime )
		{
			//set new time
			startTime = g_DominionManager.AdjustNextSiegeTime();
			g_DominionManager.SetNextSiegeTime(startTime, true);
		}
	}

	unguard;
}