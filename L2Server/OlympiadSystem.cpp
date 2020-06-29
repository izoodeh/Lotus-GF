#include "StdAfx.h"
#include "OlympiadSystem.h"
#include "HookManager.h"
#include "SSQSystem.h"
#include "UserDb.h"

extern WCHAR g_ConfigFile[260];

COlympiadSystem g_OlympiadSystem;

INT32 g_MaxPointsPerMatch = 10;
INT32 g_MaxPointsPerPenalty = 10;

double g_TeamPointsValue = 15.0f;
double g_ClassFreePointsValue = 5.0f;
double g_ClassPointsValue = 3.0f;

INT32 g_OlympiadMatchDuration = 360;
CompetitionTime g_CompetitionTime[7][2];
INT32 g_OlympiadTeleportToStadiumTime = 120;

COlympiadSystem::COlympiadSystem() : m_HWIDLimit(0), m_HWIDLimitMsgId(0)
{
	SetName(L"Olympiad System");
}

COlympiadSystem::~COlympiadSystem()
{
}

void COlympiadSystem::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	for(int n=0;n<7;n++)
	{
		g_CompetitionTime[n][0].startHour = 18;
		g_CompetitionTime[n][0].endHour = 0;
		g_CompetitionTime[n][1].startHour = 18;
		g_CompetitionTime[n][1].endHour = 0;
	}
	const TCHAR* section = _T("Olympiad");
	g_MaxPointsPerMatch = GetPrivateProfileInt(section, TEXT("MaxPointsPerMatch"), 0, g_ConfigFile);
	g_MaxPointsPerPenalty = GetPrivateProfileInt(section, TEXT("MaxPointsPerPenalty"), 0, g_ConfigFile);
	g_OlympiadTeleportToStadiumTime = GetPrivateProfileInt(section, TEXT("TeleportToStadium"), 120, g_ConfigFile);
	g_OlympiadTeleportToStadiumTime+=2;
	WriteBYTE(0x7651C4, static_cast<BYTE>(g_OlympiadTeleportToStadiumTime));

	INT32 param = GetPrivateProfileInt(section, TEXT("TeamPointsValue"), 15, g_ConfigFile);
	g_TeamPointsValue = (double)param;
	param = GetPrivateProfileInt(section, TEXT("ClassFreePointsValue"), 5, g_ConfigFile);
	g_ClassFreePointsValue = (double)param;
	param = GetPrivateProfileInt(section, TEXT("ClassPointsValue"), 3, g_ConfigFile);
	g_ClassPointsValue = (double)param;
	m_HWIDLimit = GetPrivateProfileInt(section, _T("HWIDLimit"), 0, g_ConfigFile);
	m_HWIDLimitMsgId = GetPrivateProfileInt(section, _T("HWIDLimitMsgId"), 0, g_ConfigFile);
	g_OlympiadMatchDuration = GetPrivateProfileInt(section, TEXT("MatchDuration"), 360, g_ConfigFile);
	
	TCHAR temp[32];
	memset(temp, 0, sizeof(temp));
	
	if(GetPrivateProfileString(section, _T("Sunday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[0][0].startHour >> g_CompetitionTime[0][0].endHour;
	}
	if(GetPrivateProfileString(section, _T("SundayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[0][1].startHour >> g_CompetitionTime[0][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("Monday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[1][0].startHour >> g_CompetitionTime[1][0].endHour;
	}
	if(GetPrivateProfileString(section, _T("MondayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[1][1].startHour >> g_CompetitionTime[1][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("Tuesday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[2][0].startHour >> g_CompetitionTime[2][0].endHour;
	}
	if(GetPrivateProfileString(section, _T("TuesdayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[2][1].startHour >> g_CompetitionTime[2][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("Wednesday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[3][0].startHour >> g_CompetitionTime[3][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("WednesdayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[3][1].startHour >> g_CompetitionTime[3][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("Thursday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[4][0].startHour >> g_CompetitionTime[4][0].endHour;
	}
	if(GetPrivateProfileString(section, _T("ThursdayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[4][1].startHour >> g_CompetitionTime[4][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("Friday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[5][0].startHour >> g_CompetitionTime[5][0].endHour;
	}
	if(GetPrivateProfileString(section, _T("FridayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[5][1].startHour >> g_CompetitionTime[5][1].endHour;
	}
	if(GetPrivateProfileString(section, _T("Saturday"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[6][0].startHour >> g_CompetitionTime[6][0].endHour;
	}
	if(GetPrivateProfileString(section, _T("SaturdayEx"), 0, temp, 32, g_ConfigFile))
	{
		wstringstream sstr;
		sstr << temp;
		sstr >> g_CompetitionTime[6][1].startHour >> g_CompetitionTime[6][1].endHour;
	}

	m_MaxArmorEnchant = GetPrivateProfileInt(section, _T("MaxArmorEnchant"), 0, g_ConfigFile);
	m_MaxWeaponEnchant = GetPrivateProfileInt(section, _T("MaxWeaponEnchant"), 0, g_ConfigFile);

	WriteDWORD(0x7667F8, g_OlympiadMatchDuration);

	WriteCall(0x76D90F, COlympiadSystem::GetTransferingPoint);
	WriteCall(0x76DFB9, COlympiadSystem::GetPenaltyPoint);
	WriteCall(0x76DFDD, COlympiadSystem::GetPenaltyPoint);
	WriteCall(0x76E451, COlympiadSystem::GetPenaltyPoint);
	WriteCall(0x7755FA, COlympiadSystem::GetWantedStatus);
	WriteCall(0x770F22, COlympiadSystem::CanAddOlympiadUser);

	g_Log.Add(CLog::Blue, "[%s] HWIDLimit[%d] MessageId[%d]", __FUNCTION__, m_HWIDLimit, m_HWIDLimitMsgId);
}

bool COlympiadSystem::CanUse(CItem *pItem)
{
	guard;

	bool ret = true;

	if(pItem)
	{
		if(pItem->pSID->itemType == ItemWeapon && m_MaxWeaponEnchant > 0)
		{
			if(pItem->pSID->enchantLevel > m_MaxWeaponEnchant)
			{
				ret = false;
			}
		}else if((pItem->pSID->itemType == ItemArmor || pItem->pSID->itemType == ItemAccessary) && m_MaxArmorEnchant > 0)
		{
			if(pItem->pSID->enchantLevel > m_MaxArmorEnchant)
			{
				ret = false;
			}
		}
	}

	unguard;
	return ret;
}

void COlympiadSystem::ValidateEquippedItems(User *pUser)
{
	guard;

	if(m_MaxArmorEnchant > 0 || m_MaxWeaponEnchant > 0)
	{
		for(UINT n=0;n<29;n++)
		{
			INT32 itemIndex = pUser->pSD->equipment[n];
			if(itemIndex > 0)
			{
				if(CItem *pItem = pUser->inventory.GetByIndex(itemIndex))
				{
					if(pItem->pSID->itemType == ItemWeapon)
					{
						if(m_MaxWeaponEnchant > 0 && m_MaxWeaponEnchant < pItem->pSID->enchantLevel)
						{
							pUser->UnequipItem(pItem);
						}
					}else if(pItem->pSID->itemType == ItemArmor || pItem->pSID->itemType == ItemAccessary)
					{
						if(m_MaxArmorEnchant > 0 && m_MaxArmorEnchant < pItem->pSID->enchantLevel)
						{
							pUser->UnequipItem(pItem);
						}
					}
				}
			}
		}
	}

	unguard;
}

bool COlympiadSystem::CanAddOlympiadUser(User *pUser, LPVOID lpOlympiadEntry, int& param, int& messageId)
{
	typedef bool (*f)(User*, LPVOID, int&, int&);
	if(f(0x770B58L)(pUser, lpOlympiadEntry, param, messageId))
	{
		//check if there's 10 minutes left
		tm ti; time_t rawtime;
		time ( &rawtime );
		localtime_s(&ti, &rawtime );

		bool sealPeriod = false;

		if(CSSQSystem *pSSQ = CSSQSystem::GetInstance())
		{
			if(pSSQ->pSD)
			{
				if(pSSQ->pSD->status == SSQ_SEAL_EFFECT)
				{
					sealPeriod = true;
				}
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot get SSQSystem instance!", __FUNCTION__);
		}

		INT32 start = g_CompetitionTime[ti.tm_wday][0].startHour;
		INT32 end = g_CompetitionTime[ti.tm_wday][0].endHour;
		if(sealPeriod)
		{
			start = g_CompetitionTime[ti.tm_wday][1].startHour;
			end = g_CompetitionTime[ti.tm_wday][1].endHour;
		}

		if(end == 0)
			end = 24;

		if( ( ti.tm_hour + 1 ) == end )
		{
			if(ti.tm_min > 50)
			{
				messageId = 800;
				return false;
			}
		}

		INT32 hwidLimit = g_OlympiadSystem.GetHWIDLimit();
		if(hwidLimit > 0)
		{
			INT32 regAmount = g_UserDb.GetOlympiadUsersByHWID(pUser->pUserSocket->hardwareId.hash);
			if(hwidLimit <= regAmount)
			{
				g_Log.Add(CLog::Error, L"[%s] User[%s] reched HWID limit[%d][%d] - msg[%d] !", __FUNCTIONW__, pUser->pSD->wName, g_OlympiadSystem.GetHWIDLimit(), regAmount, g_OlympiadSystem.GetHWIDLimitMsgId());
				//3035	1	a,You cannot register in Olympiad - reached the limit!\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
				messageId = g_OlympiadSystem.GetHWIDLimitMsgId();
				return false;
			}
		}

		return true;
	}
	return false;
}

bool COlympiadSystem::GetWantedStatus(LPVOID lpInstance, UINT seasonStart, OlympiadStatus& os)
{
	typedef bool (*f)(LPVOID, UINT, OlympiadStatus&);
	if(f(0x76898CL)(lpInstance, seasonStart, os))
	{
		tm ti; time_t rawtime;
		time ( &rawtime );
		localtime_s(&ti, &rawtime );

		bool sealPeriod = false;

		if(CSSQSystem *pSSQ = CSSQSystem::GetInstance())
		{
			if(pSSQ->pSD)
			{
				if(pSSQ->pSD->status == SSQ_SEAL_EFFECT)
				{
					sealPeriod = true;
				}
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot get SSQSystem instance!", __FUNCTION__);
		}

		INT32 start = g_CompetitionTime[ti.tm_wday][0].startHour;
		INT32 end = g_CompetitionTime[ti.tm_wday][0].endHour;
		if(sealPeriod)
		{
			start = g_CompetitionTime[ti.tm_wday][1].startHour;
			end = g_CompetitionTime[ti.tm_wday][1].endHour;
		}

		os.competitionEnabled = TRUE;
		
		if(end < start)
		{
			if(ti.tm_hour >= start)
			{
				return true;
			}
			if(ti.tm_hour >= end)
			{
				os.competitionEnabled = FALSE;
				return true;
			}
		}

		if(ti.tm_hour < start)
		{
			//Disable oly
			os.competitionEnabled = FALSE;
			return true;
		}

		if(ti.tm_hour >= end)
		{
			//Disable oly
			os.competitionEnabled = FALSE;
			return true;
		}

		return true;
	}
	return false;
}

int COlympiadSystem::GetPenaltyPoint(LPVOID lpInstance, GameRuleType type, int points)
{
	typedef int (*f)(LPVOID, GameRuleType, int);

	int ret = f(0x768F78L)(lpInstance, type, points);

	if(g_MaxPointsPerPenalty > 0)
	{
		if(ret > g_MaxPointsPerPenalty)
		{
			ret = g_MaxPointsPerPenalty;
		}
	}

	return ret;
}

int COlympiadSystem::GetTransferingPoint(LPVOID lpInstance, GameRuleType type, int team1Points, int team2Points)
{
	int points = team1Points;
	if(team1Points > team2Points)
		points = team2Points;

	double value = (double)points;
	
	switch(type)
	{
	case GR_TEAM:
		{
			value /= g_TeamPointsValue;
			break;
		}
	case GR_CLASS_FREE:
		{
			value /= g_ClassFreePointsValue;
			break;
		}
	case GR_CLASS:
		{
			value /= g_ClassPointsValue;
			break;
		}
	}

	points = (int)ceil(value);

	if(g_MaxPointsPerMatch > 0)
	{
		if(points > g_MaxPointsPerMatch)
		{
			points = g_MaxPointsPerMatch;
		}
	}

	return points;
}