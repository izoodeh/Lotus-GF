#pragma once

namespace Network
{
	namespace IncommingPacket
	{
		enum CustomPacket
		{
			BaseId = 0x8D,
			AntiBot = 0x01,
			HardwareId = 0x02
		};
	};
	namespace OutgoingPacket
	{
		enum CustomPacket
		{
			BaseId = 0xA,
			AntiBot = 0x01,
			HardwareId = 0x02
		};
	};
};

typedef bool (__cdecl *HandlerCallback)(CClientSocket*);

struct HandlerInfo
{
	HandlerCallback func;
	WCHAR wName[16];
};

class CNetworkHandler
{
public:
	/* 00 */ DWORD _unkn00[18];
	/* 48 */ CClientSocket *pSocket;
};

class CPacketHandler
{
	HandlerInfo *lpHandler;
	HandlerInfo *lpHandlerEx;
public:
	CPacketHandler();
	~CPacketHandler();
	void Init();
	void DumpTable();
	static int __thiscall Handle(CNetworkHandler* lpNetwork, NetworkPacket *lpPacket);
	static int HandleCustomPacket(CClientSocket *pSocket, NetworkPacket* lpPacket);
	int RequestCheckOffsetPacket(CClientSocket *pSocket, CInPacket& packet);
	int RequestHardwareIdPacket(CClientSocket *pSocket, CInPacket& packet);
};

extern CPacketHandler g_PacketHandler;