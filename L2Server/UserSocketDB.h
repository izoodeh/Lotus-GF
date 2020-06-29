#pragma once
#include "IPAddress.h"

struct UserSocketData
{
	time_t conTime;
	CIPAddress ip;
};

class CUserSocketDB
{
	CRITICAL_SECTION _lock;
	inline void Lock() { EnterCriticalSection(&_lock); };
	inline void Unlock() { LeaveCriticalSection(&_lock); };

	map<CUserSocket*, UserSocketData> mData;
public:
	CUserSocketDB();
	~CUserSocketDB();
	void Add(CUserSocket* pSocket);
	void Del(CUserSocket* pSocket);
	int GetCountByIp(CIPAddress ip);
};

extern CUserSocketDB g_UserSocketDB;