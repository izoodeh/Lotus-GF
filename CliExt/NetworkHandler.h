#pragma once
#include "ClientSocket.h"
#include "Packet.h"

typedef bool (__cdecl *HandlerCallback)(CClientSocket*);

//interlude 104h size
struct HandlerInfo
{
	HandlerCallback func;
	WCHAR wName[128];
};

struct NHandler
{
public:
	/* 00 */ DWORD _unkn00[18];
	/* 48 */ CClientSocket *pSocket;
};

typedef int (__cdecl *PacketHandler)(NHandler*, CInPacket& packet);

class CNetworkHandler
{
	UINT m_TickTimeout;
	BOOL m_Initialized;
	time_t m_PingExTime;
	UINT m_RequestQuit;
	time_t m_RequestQuitTime;
	HANDLE m_SeparatedTick;
	WCHAR m_RequestQuitModuleName[256];
	PacketHandler m_CustomHandler[256];
public:
	void Init();
	void Bind(UINT id, PacketHandler func);
	inline UINT GetTickTimeout() { return m_TickTimeout; };
	inline UINT GetRequestQuit() { return m_RequestQuit; };
	inline void SetRequestQuit(UINT value = 1, const WCHAR* wModuleName = 0) { m_RequestQuit = value; m_RequestQuitTime = time(0); if(wModuleName) { wcscpy(m_RequestQuitModuleName, wModuleName); }; };
	inline const WCHAR* GetRequestQuitModuleName() { return m_RequestQuitModuleName; };
	UINT GetMyRealIP(string m_urlHost, string m_urlPath);
	static void __fastcall OnTick();
	static void __cdecl SeparatedTick(void *args);
	static int __thiscall HandlePacket(NHandler *pNetwork, NPacket *pPacket);
};

int CPHDummyPacket(NHandler *pNetwor, CInPacket& packet);
int CPHPingExPacket(NHandler *pNetwork, CInPacket& packet);
int CPHGetHWIDPacket(NHandler *pNetwor, CInPacket& packet);
int CPHGetRealIpPacket(NHandler *pNetwork, CInPacket& packet);

extern CNetworkHandler g_NetworkHandler;