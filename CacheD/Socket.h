#pragma once
#include "IOSocketEx.h"

class CSocket;

typedef bool (*SocketCallback)(CSocket*, const unsigned char*);

//vtable: 64CD78h size: 0x1A0
class CSocket : public CIOSocketEx
{
public:

	/* 00B0 */ LPVOID _unkn00B0[10];
	/* 0100 */ LPVOID _unkn0100[2];
	/* 0110 */ SocketCallback* lpPacketTable;
	/* 0118 */ LPVOID _unkn0118[17];


	void Send(const char* format, ...);
};