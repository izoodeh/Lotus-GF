#pragma once
#include "MemoryObject.h"

class CIOBufferEx
{
public:
	/* 0000 */ LPVOID _unkn0000;
	/* 0000 */ LPVOID _unkn0008;
	/* 0010 */ BYTE buff[16384];
	/* 4010 */ UINT size;
	/* 4014 */ INT32 _unkn4014;
	/* 4018 */ CIOBufferEx *pNext;
	/* 4020 */ LONG refCount;
	/* 4024 */ INT32 _unkn4024;
	/* 4028 */ LPVOID _unkn4028;
	/* 4030 */ INT32 _unkn4030;
	/* 4034 */ INT32 poolThreadIdx;
};

class CIOBuffer
{
public:
	/* 0000 */ LPVOID _unkn0000;
	/* 0000 */ LPVOID _unkn0008;
	/* 0010 */ BYTE buff[8192];
	/* 2010 */ UINT size;
	/* 2014 */ INT32 _unkn4014;
	/* 2018 */ CIOBuffer *pNext;
	/* 2020 */ LONG refCount;
	/* 2024 */ INT32 _unkn4024;
	/* 2028 */ LPVOID _unkn4028;
	/* 2030 */ INT32 _unkn4030;
	/* 2034 */ INT32 poolThreadIdx;
};

//vTable: ABDDA8h
class CIOSocketEx : public CBaseObject
{
public:
	/* 0018 */ SOCKET s;
	/* 0020 */ INT32 pendingWrite;
	/* 0024 */ INT32 _unkn0024;
	/* 0028 */ LPVOID asyncExec[6];	//CExecutionSerializerFast
	/* 0058 */ LPVOID _unkn0058[4];
	/* 0078 */ OVERLAPPED overlapped;
	/* 0098 */ CIOBufferEx* lpIncomingPck;
	/* 00A0 */ CIOBufferEx *lpOutgoingPck;	/* that's gonna be sent */
	/* 00A8 */ CIOBufferEx *pPendingPck;
};