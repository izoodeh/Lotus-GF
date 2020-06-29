#pragma once
#include "MemoryObject.h"
#include "IOCriticalSection.h"

//that's ilext struct could change - not used atm so not important
class CIOBufferEx
{
	public:
		/* 0000 */ LPVOID _unkn0000;
		/* 0008 */ LPVOID _unkn0008;
		/* 0010 */ BYTE buff[0x4000];
		/* 2010 */ INT32 size;
		/* 2014 */ INT _unkn2014;
		/* 2018 */ CIOBufferEx *pPending;
		/* 2020 */ long volatile inUse;
		/* 2024 */ long _unkn2024;
		/* 2028 */ LPVOID _unkn2028;
		/* 2030 */ LPVOID _unnn2030;
		/* 2038 */ DWORD sendTickCount;
		/* 203C */ DWORD pingPacket;
		/* 2040 */ DWORD pingPacketId;
		/* 2044 */ DWORD _unkn2048;
		static CIOBufferEx* Allocate();
		void Free();
};
// end

//vTable: 60E708h size: 0xB0
class CIOSocketEx : public CBaseObject
{
public:
	/* 0018 */ SOCKET s;
	/* 0020 */ INT32 pendingWrite;
	/* 0024 */ INT32 _unkn0024;
	/* 0028 */ CIOCriticalSection lock;
	/* 0054 */ INT32 _unkn54;
	/* 0058 */ LPVOID _unkn0058[4];
	/* 0078 */ OVERLAPPED overlapped;
	/* 0098 */ CIOBufferEx* lpIncomingPck;
	/* 00A0 */ CIOBufferEx *lpOutgoingPck;	/* that's gonna be sent */
	/* 00A8 */ CIOBufferEx *pPendingPck;
};