#include "StdAfx.h"
#include "UserSocketDB.h"

CUserSocketDB g_UserSocketDB;

CUserSocketDB::CUserSocketDB()
{
	InitializeCriticalSection(&_lock);
}

CUserSocketDB::~CUserSocketDB()
{
	DeleteCriticalSection(&_lock);
}

void CUserSocketDB::Add(CUserSocket *pSocket)
{
	UserSocketData data;
	data.conTime = time(NULL);

	sockaddr_in addr;
	INT addrLen = sizeof(addr);
	getpeername(pSocket->s, (sockaddr*)&addr, &addrLen);
	CIPAddress ip(addr);
	data.ip = ip;

	Lock();
	mData.insert( pair<CUserSocket*, UserSocketData>( pSocket, data ) );
	Unlock();
}

void CUserSocketDB::Del(CUserSocket *pSocket)
{
	Lock();
	map<CUserSocket*, UserSocketData>::iterator Iter = mData.find(pSocket);
	if(Iter!=mData.end())
	{
		mData.erase(Iter);
	}
	Unlock();
}

int CUserSocketDB::GetCountByIp(CIPAddress ip)
{
	int count = 0;
	Lock();
	for(map<CUserSocket*, UserSocketData>::iterator Iter=mData.begin();Iter!=mData.end();Iter++)
	{
		if(Iter->second.ip == ip)
		{
			count++;
		}
	}
	Unlock();

	return count;
}