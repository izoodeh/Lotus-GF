#include "stdafx.h"
#include "AutoAnnounce.h"

CAutoAnnounce g_AutoAnnounce;

CAutoAnnounce::CAutoAnnounce()
{
}

CAutoAnnounce::~CAutoAnnounce()
{
}

void CAutoAnnounce::Init()
{
	m_reload = FALSE;
	m_data.clear();
	LoadINI();
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "Auto Announce feature is enabled. Loaded [%d] announces.", m_data.size());
	}
}

/*
Enabled=1
AnnounceCount=2
Announce1_Message=Welcome to ServerName Lineage II Interlude server, hope you enjoy.
Announce1_ShowOnEnterWorld=1
Announce1_Interval=0
Announce2_Message=Please remember to vote for us, more info at www.servername.com thx in advance.
Announce2_ShowOnEnterWorld=1
Announce2_Interval=300
*/

void CAutoAnnounce::LoadINI()
{
	TCHAR file[260];
	GetCurrentDirectory(MAX_PATH,file);
	lstrcat(file, _T("\\ServerExt.ini"));

	const TCHAR* section = _T("AutoAnnounce");
	m_enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, file);
	UINT count = GetPrivateProfileInt(section, _T("AnnounceCount"), 0, file);
	for(UINT n = 0;n<count;n++)
	{
		AutoAnnounceData aad;
		aad.nextAnnounceTime = 0;
		{
			wstringstream keyStream;
			keyStream << L"Announce" << (n+1) <<L"_Message";
			TCHAR temp[8192];
			if(GetPrivateProfileString(section, keyStream.str().c_str(), 0, temp, 8192, file))
			{
				aad.announce = temp;
			}
		}
		{
			wstringstream keyStream;
			keyStream << L"Announce" << (n+1) <<L"_ShowOnEnterWorld";
			aad.onEnterWorld = GetPrivateProfileInt(section, keyStream.str().c_str(), 0, file);
		}
		{
			wstringstream keyStream;
			keyStream << L"Announce" << (n+1) <<L"_Interval";
			aad.interval = GetPrivateProfileInt(section, keyStream.str().c_str(), 0, file);
		}
		m_data.push_back(aad);
	}
	TCHAR temp[8190] = { 0 };
	if(GetPrivateProfileString(section, _T("PvPMessage"), 0, temp, 8190, file))
	{
		m_PvPMessage = temp;
	}
	m_PvPMessageType = GetPrivateProfileInt(section, _T("PvPMessageType"), -1, file);
	if(GetPrivateProfileString(section, _T("PvPMessengerName"), 0, temp, 8190, file))
	{
		m_PvPMessenger = temp;
	}
	if(GetPrivateProfileString(section, _T("PKMessage"), 0, temp, 8190, file))
	{
		m_PKMessage = temp;
	}
	m_PKMessageType = GetPrivateProfileInt(section, _T("PKMessageType"), -1, file);
	if(GetPrivateProfileString(section, _T("PKMessengerName"), 0, temp, 8190, file))
	{
		m_PKMessenger = temp;
	}
}

void CAutoAnnounce::TimerExpired()
{
	if(m_enabled || m_reload)
	{
		m_lock.Enter();
		if(m_reload)
		{
			Init();
			if(!m_enabled)
			{
				m_lock.Leave();
				return;
			}
		}
		time_t currentTime = time(0);
		for(UINT n=0;n<m_data.size();n++)
		{
			AutoAnnounceData& data = m_data[n];
			if(data.interval > 0)
			{
				if(currentTime > data.nextAnnounceTime)
				{
					//announce and set new next announce time
					data.nextAnnounceTime = currentTime + data.interval;
					Utils::BroadcastToAllUser_Announce(data.announce.c_str());
				}
			}
		}
		m_lock.Leave();
	}
}

void CAutoAnnounce::OnEnterWorld(User *pUser)
{
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			m_lock.Enter();
			for(UINT n=0;n<m_data.size();n++)
			{
				AutoAnnounceData& data = m_data[n];
				if(data.onEnterWorld)
				{
					pUser->pUserSocket->Send("cddSS", 0x4A, 0, 0xA, L"", data.announce.c_str());
				}
			}
			m_lock.Leave();			
		}
	}
}

void CAutoAnnounce::PKMessage(User *pUser, User *pEnemy)
{
	guard;
	if(m_enabled)
	{
		if(m_PKMessageType >= 0 && m_PKMessage.size() > 0)
		{
			wstring message = ReplaceChar(m_PKMessage, L"$s1", pEnemy->pSD->wName);
			message = ReplaceChar(message, L"$s2", pUser->pSD->wName);
			BroadcastToAllUser("cddSS", 0x4A, 0, m_PKMessageType, m_PKMessenger.c_str(), message.c_str());
		}
	}
	unguard;
}

void CAutoAnnounce::PvPMessage(User *pUser, User *pEnemy)
{
	guard;
	if(m_enabled)
	{
		if(m_PvPMessageType >= 0 && m_PvPMessage.size() > 0)
		{
			wstring message = ReplaceChar(m_PvPMessage, L"$s1", pEnemy->pSD->wName);
			message = ReplaceChar(message, L"$s2", pUser->pSD->wName);
			BroadcastToAllUser("cddSS", 0x4A, 0, m_PvPMessageType, m_PvPMessenger.c_str(), message.c_str());
		}
	}
	unguard;
}