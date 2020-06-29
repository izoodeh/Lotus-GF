#pragma once
#include "StaticObject.h"

//vtable B1DEE8h
class CDoor : public CStaticObject
{
public:
	void Close(bool param1 = false, bool param2 = false);
	void Open(bool param1 = false, bool param2 = false);
};

class CDoorDB
{
	CDoorDB * lpOrgInstance;
public:
	CDoorDB();
	~CDoorDB();
	CDoor* GetDoor(const WCHAR* wName, UINT param = 0);
};

extern CDoorDB g_DoorDB;