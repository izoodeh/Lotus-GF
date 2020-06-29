#include "StdAfx.h"
#include "EventMatchMgr.h"

CEventMatchMgr::CEventMatchMgr()
{
}

CEventMatchMgr::~CEventMatchMgr()
{
}

CEventMatchMgr* CEventMatchMgr::GetInstance()
{
	typedef CEventMatchMgr* (*f)();
	return f(0x617BC8L)();
}

void CEventMatchMgr::Lock(const WCHAR *file, int line)
{
	typedef void(*f)(CEventMatchMgr*, const WCHAR*, int);
	f(0x97E9D4L)(this, file, line);
}

void CEventMatchMgr::Unlock(const WCHAR *file, int line)
{
	typedef void(*f)(CEventMatchMgr*, const WCHAR*, int);
	f(0x97EA74L)(this, file, line);
}