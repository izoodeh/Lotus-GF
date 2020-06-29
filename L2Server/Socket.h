#pragma once
#include "IOSocket.h"


typedef bool (*Callback)(CSocket*, const unsigned char*);

//vTable: C29A88h
class CSocket : public CIOSocketEx
{
public:
	/* 00B0 */ LPVOID _unknB0;
	/* 00B8 */ in_addr address;
	/* 00BC */ INT32 _unknBC;
	/* 00C0 */ Callback* lpPacketHandler;
	/* 00C8 */ UINT _unknC8;
	/* 00CC */ UINT status; //2 - closed
	/* 00D0 */ UINT incommingPacketSize;
	/* 00D4 */ INT32 _unknD4;
	/* 00D8 */ INT32 _unknD8;
	/* 00DC */ BYTE inKey[17];
	/* 00ED */ BYTE outKey[17];
	/* 00FE */ bool _alignFE;

	void SetAddress(struct in_addr addr);
	struct in_addr GetAddress( struct in_addr& addr );
	void Send(const char* format, ...);
	void SendV(const char* format, va_list va);
	CUserSocket* CastUserSocket();
};