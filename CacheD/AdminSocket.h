#pragma once
#include "Socket.h"

class CAdminSocket;

typedef bool (*AdminCallback)(CAdminSocket*, const unsigned char*);

//0x1A8 size, VT: 0x5EDE68
class CAdminSocket : public CSocket
{
public:
	/* 0x1A0 */ bool echoPacket;

	void Send(const char* format, ...);
};