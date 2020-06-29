#pragma once
#include "AdminSocket.h"
#include "..\\Shared\\CacheDPacket.h"

class CAdminPacketHandler
{
	AdminCallback m_Handler[256];
public:
	CAdminPacketHandler();
	~CAdminPacketHandler();
	void Init();
	void Bind(UINT id, AdminCallback handler);
};

bool AdminDummyPacket(CAdminSocket *pSocket, const unsigned char *packet);
bool AdminTestPacket(CAdminSocket *pSocket, const unsigned char *packet);
bool AdminSetItemLock(CAdminSocket *pSocket, const unsigned char *packet);

extern CAdminPacketHandler g_AdminPacketHandler;