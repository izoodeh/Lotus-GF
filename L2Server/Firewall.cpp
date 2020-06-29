#include "StdAfx.h"
#include "Firewall.h"
#include "HookManager.h"
#include "UserSocketDB.h"

CFirewall g_Firewall;

CFirewall::CFirewall() : hFile(0)
{
	InitializeCriticalSection(&m_bannedIpLock);
	SetName(L"Firewall");
	memset(actionType, 0, sizeof(actionType));
	memset(minTimestamp, 0, sizeof(minTimestamp));
}

CFirewall::~CFirewall()
{
	if(hFile)
	{
		CloseHandle(hFile);
	}

	DeleteCriticalSection(&m_bannedIpLock);
}

void CFirewall::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	if( GetFileAttributes(TEXT("FirewallLog.txt")) != INVALID_FILE_ATTRIBUTES)
	{
		tm ti;
		time_t rawtime;
		time ( &rawtime );
		localtime_s(&ti, &rawtime );

		wstringstream newName;
		newName << "FirewallLog.txt." << (ti.tm_year+1900) << "-" << (ti.tm_mon+1) << "-" << ti.tm_mday << "-" << ti.tm_hour << "-" << ti.tm_min << ".bak";
		MoveFile(TEXT("FirewallLog.txt"), newName.str().c_str());
	}

	Lock();
	hFile = CreateFile(TEXT("FirewallLog.txt"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	Unlock();
	AddLog(TEXT("Initializing"));
	LoadINI();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Connection Limit[%d], Ban Duration[%d], loaded [%d] safe ip(s)", GetName(), conLimit, m_banDuration, m_safeIpList.size());
		WriteCall(0x68FDC9, CFirewall::OnAccept, 1);
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled.", GetName());
	}
}

void CFirewall::LoadINI()
{
	TCHAR File[260];
	GetCurrentDirectory(MAX_PATH,File);
	lstrcat(File, TEXT("\\Firewall.ini"));
	const TCHAR* sectionName = TEXT("Firewall");
	enabled = GetPrivateProfileInt(sectionName, TEXT("Enabled"), 0, File);
	conLimit = GetPrivateProfileInt(sectionName, TEXT("ConnectionLimitPerIp"), 20, File);
	m_banDuration = GetPrivateProfileInt(sectionName, TEXT("BanDuration"), 300, File);

	{
		WCHAR tmp[8192];
		memset(tmp, 0, sizeof(tmp));
		GetPrivateProfileString(sectionName, TEXT("SafeIpList"), 0, tmp, sizeof(tmp), File);
		wstringstream sstr( tmp );
		wstring sIp;
		while( sstr >> sIp )
		{
			CIPAddress addr(sIp);
			m_safeIpList.push_back(addr);
		}
		m_safeIpList.sort();
		m_safeIpList.unique();
	}

	{
		for(int n=0;n<256;n++)
		{
			wstringstream sName;
			if(n < 16 )
			{
				sName << L"OpCode_0" << uppercase << hex << n;
			}else
			{
				sName << L"OpCode_" << uppercase << hex << n;
			}

			WCHAR tmp[512];
			memset(tmp, 0, sizeof(tmp));

			if( GetPrivateProfileString(sectionName, sName.str().c_str(), 0, tmp, sizeof(tmp), File) )
			{
				int action = 0;
				DWORD timestamp = 0;
				wstringstream str(tmp);
				WCHAR c = 0;
				str >> timestamp >> c >> action;
				actionType[n] = action;
				minTimestamp[n] = timestamp;
		//		g_Log.Add(CLog::Blue, "Firewall setting: opcode[%x] loadedStr[%S] parsed: action[%d] timestamp[%d]", n, tmp, action, timestamp);
			}
		}
	}

}

void CFirewall::AddLog(wstring log)
{
	tm timeinfo;
	time_t rawtime;
	time ( &rawtime );
	localtime_s(&timeinfo, &rawtime );

	wstringstream str;

	str << (timeinfo.tm_year+1900) << L"/" << (timeinfo.tm_mon+1) << L"/" << timeinfo.tm_mday << L" " << timeinfo.tm_hour << L":" << timeinfo.tm_min << L":" << timeinfo.tm_sec << L" " << log << L"\n";
	
	DWORD written;
	wstring output(str.str());
	Lock();
	WriteFile(hFile, output.c_str(), (DWORD)(output.size() * 2), &written, NULL);
	Unlock();
}

void CFirewall::OnPacketRecv(CUserSocket *pSocket, BYTE opCode, bool &drop, bool &kick)
{
	if(pSocket && enabled)
	{
		if( minTimestamp[opCode] > 0 )
		{
			//check timestamp
			DWORD currentTick = GetTickCount();
			if( pSocket->packetTimestamp[opCode] > currentTick )
			{
				DWORD diff = pSocket->packetTimestamp[opCode] - currentTick;
				switch(actionType[opCode] )
				{
				case ACTION_TYPE_LOG:
					{
						struct in_addr addr;
						pSocket->GetAddress(addr);
						wstringstream str;
						if( pSocket->pUser->ValidUser() )
						{
							str << L"Packet flood opCode[" << hex << opCode << L"] diff time [" << dec << diff << L"] ms from IP[" << inet_ntoa(addr) << L"] User[" << pSocket->pUser->pSD->wName << L"]";
						}else
						{
							str << L"Packet flood opCode[" << hex << opCode << L"] diff time [" << dec << diff << L"] ms from IP[" << inet_ntoa(addr) << L"]";
						}
						AddLog(str.str());
						break;
					}
				case ACTION_TYPE_DROP:
					{
						drop = true;
						break;
					}
				case ACTION_TYPE_LOG_DROP:
					{
						struct in_addr addr;
						pSocket->GetAddress(addr);
						wstringstream str;
						if( pSocket->pUser->ValidUser() )
						{
							str << L"Packet flood opCode[" << hex << opCode << L"] diff time [" << dec << diff << L"] ms from IP[" << inet_ntoa(addr) << L"] User[" << pSocket->pUser->pSD->wName << L"] - dropped";
						}else
						{
							str << L"Packet flood opCode[" << hex << opCode << L"] diff time [" << dec << diff << L"] ms from IP[" << inet_ntoa(addr) << L"] - dropped";
						}
						AddLog(str.str());
						drop = true;
						break;
					}
				case ACTION_TYPE_KICK:
					{
						kick = true;
						break;
					}
				case ACTION_TYPE_LOG_KICK:
					{
						struct in_addr addr;
						pSocket->GetAddress(addr);
						wstringstream str;
						if( pSocket->pUser->ValidUser() )
						{
							str << L"Packet flood opCode[" << hex << opCode << L"] diff time [" << dec << diff << L"] ms from IP[" << inet_ntoa(addr) << L"] User[" << pSocket->pUser->pSD->wName << L"] - kicked";
						}else
						{
							str << L"Packet flood opCode[" << hex << opCode << L"] diff time [" << dec << diff << L"] ms from IP[" << inet_ntoa(addr) << L"] - kicked";
						}
						AddLog(str.str());
						kick = true;
						break;
					}
				default:
					{
						break;
					}
				}
			}else
			{
				pSocket->packetTimestamp[opCode] = currentTick + minTimestamp[opCode];
			}
		}
	}
}

bool CFirewall::CanConnect(CIPAddress ip)
{
	for(list<CIPAddress>::iterator Iter = m_safeIpList.begin(); Iter!=m_safeIpList.end();Iter++)
	{
		if( (*Iter) == ip )
		{
			return true;
		}
	}

	time_t currentTime = time(NULL);
	EnterCriticalSection(&m_bannedIpLock);
	map<CIPAddress, time_t>::iterator Iter = m_bannedIp.find(ip);
	if( Iter != m_bannedIp.end() )
	{
		if( Iter->second == 0 || Iter->second > currentTime )
		{
			//still banned
			LeaveCriticalSection(&m_bannedIpLock);
			return false;
		}else
		{
			m_bannedIp.erase(Iter);
		}
	}
	

	int conCount = g_UserSocketDB.GetCountByIp(ip);
	if( conCount >= conLimit )
	{
		time_t timestamp = m_banDuration + currentTime;
		
		m_bannedIp.insert( pair<CIPAddress, time_t>( ip, timestamp ) );
		LeaveCriticalSection(&m_bannedIpLock);
		
		wstringstream str;
		str << L"Reached connection limit per ip for ip " << ip.ToWString() << L" - banned for " << m_banDuration << L" second(s).";
		g_Log.Add(CLog::Error, "[%S] Reached connection limit per ip for ip[%S] - banned for [%d] second(s)!", GetName(), ip.ToWString().c_str(), m_banDuration);
		AddLog(str.str());
		return false;
	}

	LeaveCriticalSection(&m_bannedIpLock);

	return true;
}

SOCKET CFirewall::OnAccept(SOCKET ListenSocket, sockaddr *addr, LPINT lpAddrLen)
{
//	CALL_TRACK;

	SOCKET s = accept(ListenSocket, addr, lpAddrLen);
	try
	{
		struct sockaddr_in* lpAddr = (struct sockaddr_in*)addr;
		
		CIPAddress ip(lpAddr->sin_addr);
		
		if( g_Firewall.CanConnect(ip) )
		{
			return s;
		}else
		{
			closesocket(s);
			WSASetLastError(WSAEWOULDBLOCK);
			s = -1;
		}
		
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	return s;
}
