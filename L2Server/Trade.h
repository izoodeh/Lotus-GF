#pragma once
#include "MemoryObject.h"
#include "SharedRWLock.h"

//size: 4090 vt: C35A70h
class CTrade : public MemoryObject
{
public:
	/* 18 */ DWORD _unkn18;
	/* 1C */ DWORD _unkn1C;
	/* 20 */ DWORD _unkn20;
	/* 24 */ DWORD timestamp;
	/* 28 */ UINT userServerId1;
	/* 2C */ UINT userServerId2;
	/* 30 */ vector<int> vItems1;
	/* 50 */ vector<int> vItems2;
	/* 70 */ INT32 _unkn70;
	/* 74 */ BYTE arrUser1[0x2000];
	/* 2074 */ BYTE arrUser2[0x2000];
	/* 4074 */ DWORD _unkn4074;
	/* 4078 */ DWORD _unkn4078;
	/* 407C */ CYieldLock lock;
	/* 4088 */ DWORD tickCount;
	/* 408C */ DWORD _unkn408C;
};