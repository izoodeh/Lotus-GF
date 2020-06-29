#include "stdafx.h"
#include "Door.h"

CDoorDB g_DoorDB;

void CDoor::Open(bool param1, bool param2)
{
	typedef void (*f)(CDoor*, bool, bool);
	f(0x5F7E8CL)(this, param1, param2);
}

void CDoor::Close(bool param1, bool param2)
{
	typedef void (*f)(CDoor*, bool, bool);
	f(0x5F8168L)(this, param1, param2);
}


CDoorDB::CDoorDB()
{
	lpOrgInstance = (CDoorDB*)0x226F590;
}

CDoorDB::~CDoorDB()
{
}

CDoor* CDoorDB::GetDoor(const WCHAR *wName, UINT param)
{
	typedef CDoor*(*f)(CDoorDB*, const WCHAR*, UINT);
	return f(0x5F9B20L)(lpOrgInstance, wName, param);
}