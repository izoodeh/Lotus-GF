#pragma once
#include "IPAddress.h"

class CProtector : public CFeature
{
	vector<UINT> m_AccountIds;
	vector<CIPAddress> m_IPs;
	vector<LPBYTE> m_HardwareIds;
	DWORD reloadTimestamp;
	UINT refreshTime;
	BOOL enabled;
	BOOL firstLoad;
	time_t accessCheckTime;
	string status;
public:
	CProtector();
	~CProtector();
	void Init();
	void TimerExpired();
	void TimerExpired(User *pUser);
	static const WCHAR* GetHTMLFile(LPVOID lpInstance, const WCHAR* file, int lang);
	static const CHAR* RTypeIdFix(LPVOID lpPointer);
	static void AddDeadlockLog(LPVOID lpInstance, UINT type, const WCHAR* format, UINT threadId, UINT diff);
	static void AccessCheck(LPVOID param);
};

extern CProtector g_Protector;