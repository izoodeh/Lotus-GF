#include "stdafx.h"
#include "NetworkHandler.h"
#include "Packet.h"
#include "MemoryProtector.h"
#include "HWID.h"
#include "NWindow.h"
#include "Crypt.h"

CNetworkHandler g_NetworkHandler;
extern HMODULE g_Engine;
PVOID g_UNetworkHandler_Handle = 0;
UINT g_NetworkHandlerOnTick = 0;

#pragma optimize("", off)

UINT g_PopNetworkRetn = 0;

void __declspec(naked) NetworkHandlerOnTickWrapper()
{
	__asm
	{
		call CNetworkHandler::OnTick
		push g_PopNetworkRetn
		retn
	}
}

void __declspec(naked) NetworkHandlerOnTick()
{
	__asm
	{
		pop g_PopNetworkRetn
		push NetworkHandlerOnTickWrapper
		push ebp
		mov ebp, esp
		push 0FFFFFFFFh
		jmp g_NetworkHandlerOnTick;
	}
}


void CNetworkHandler::Init()
{
	VIRTUALIZER_START;
	UINT baseAddress = reinterpret_cast<UINT>(g_Engine);
	Memory::WriteJump((baseAddress + 0x3A5580), NetworkHandlerOnTick);	//?Tick@UNetworkHandler@@UAEXM@Z
	g_NetworkHandlerOnTick = baseAddress + 0x3A5585;
	for(UINT n=0;n<256;n++)
	{
		m_CustomHandler[n] = CPHDummyPacket;
	}

	m_Initialized = FALSE;
	m_RequestQuitTime = 0;
	m_RequestQuit = FALSE;
	m_PingExTime = time(0);
	VIRTUALIZER_END;
}

void CNetworkHandler::Bind(UINT id, PacketHandler func)
{
	if(id < 256)
	{
		m_CustomHandler[id] = func;
	}
}


void CNetworkHandler::SeparatedTick(void *args)
{
	VIRTUALIZER_START;

	bool quit = false;
	while(!quit)
	{
		time_t currentTime = time(0);
		if( (currentTime % 14) == 0 )
		{
			g_MemoryProtector.ValidateRunningApps();
		}else if((currentTime % 23) == 0 )
		{
			g_MemoryProtector.ValidateRunningAppsEx();
		}else if((currentTime % 10) == 0)
		{
			g_MemoryProtector.ValidateModules();
		}
		Sleep(999);
	}

	_endthread();
	VIRTUALIZER_END;
}

void CNetworkHandler::OnTick()
{
	VIRTUALIZER_START;
	UINT currentTick = GetTickCount();
	if(currentTick > g_NetworkHandler.m_TickTimeout)
	{
		g_NetworkHandler.m_TickTimeout = currentTick + 900;
		if(g_NetworkHandler.m_Initialized == FALSE)
		{
			g_NetworkHandler.m_Initialized = TRUE;
			UINT baseAddress = reinterpret_cast<UINT>(g_Engine);
			g_UNetworkHandler_Handle = (PVOID)(*(PUINT)(baseAddress + 0x493D04));
			Memory::WriteDWORD((baseAddress + 0x493D04), (DWORD)CNetworkHandler::HandlePacket);	//?AddNetworkQueue@UNetworkHandler@@UAEHPAUNetworkPacket@@@Z

			g_NetworkHandler.Bind(1, CPHPingExPacket);
			g_NetworkHandler.Bind(2, CPHGetHWIDPacket);
			g_NWindow.Init();
		
			g_MemoryProtector.InitCliExt();
			g_MemoryProtector.ValidateRunningApps();
				
			g_NetworkHandler.m_SeparatedTick = (HANDLE)_beginthread(CNetworkHandler::SeparatedTick, 0, 0);

		}else
		{
			g_MemoryProtector.ValidateMemory();
		}
	}
	
	VIRTUALIZER_END;
}

//ofstream pckLog("PacketLogt.xt");

int __thiscall CNetworkHandler::HandlePacket(NHandler *pNetwork, NPacket *pPacket)
{
	VIRTUALIZER_START;
	int ret = 0;

	if(pPacket->id == 0x8D)
	{
		//handle custom packets
		CInPacket pck(pPacket->len, pPacket->buff);
		UINT opCode = pck.ReadD();
		if(opCode < 256)
		{
//			pckLog <<  dec << (UINT)time(0) <<  " Handling custom packet: " << hex << (UINT)pPacket->id << " type: " << dec << opCode << endl;
			ret = g_NetworkHandler.m_CustomHandler[opCode](pNetwork, pck);
		}
	}else
	{
		typedef int (__thiscall *f)(NHandler*, NPacket*);
		ret = f(g_UNetworkHandler_Handle)(pNetwork, pPacket);
	}

	VIRTUALIZER_END;
	return ret;
}

int CPHDummyPacket(NHandler *pNetwork, CInPacket& packet)
{
//	MessageBox(0, L"DummyPacket", L"CustomPacket", MB_OK);
	return 0;
}

int CPHPingExPacket(NHandler *pNetwork, CInPacket& packet)
{
	VIRTUALIZER_START;

	UINT tickId = packet.ReadD();
	COutPacket pckA;
	pckA.WriteC(0xA);	//custom packet out opcode
	pckA.WriteD(1);	//PingEx opcode
	COutPacket pckB;
	pckB.WriteD(GetTickCount());
	pckB.WriteD(tickId);
	pckB.WriteD(g_NetworkHandler.GetTickTimeout());
	pckB.WriteD(g_NetworkHandler.GetRequestQuit());
	pckB.WriteS(g_NetworkHandler.GetRequestQuitModuleName());
	pckA.WriteH(static_cast<USHORT>(pckB.GetLen()));
	CryptXor((LPBYTE)pckB.GetBuff(), pckB.GetLen());
	pckA.WriteB(pckB.GetBuff(), pckB.GetLen());

	pNetwork->pSocket->Send(pckA.GetLen(), pckA.GetBuff());
//	pckLog << dec << (UINT)time(0) <<  " Sending net ping packet for tick: " << dec << tickId << endl;

	VIRTUALIZER_END;
	return 0;
}

int CPHGetHWIDPacket(NHandler *pNetwork, CInPacket& packet)
{
	VIRTUALIZER_START;

	UINT hwid = g_HWID.Get();
	LPBYTE lpHWID = g_HWID.GetHash();

	COutPacket pck;
	pck.WriteC(0xA);
	pck.WriteD(2);	//HWID opcode
	pck.WriteD(hwid);
	for(UINT n=0;n<32;n++)
	{
		pck.WriteC(lpHWID[n]);
	}
	pNetwork->pSocket->Send(pck.GetLen(), pck.GetBuff());
//	pckLog << dec << (UINT)time(0) <<  " Sending HWID packet: " << dec << hwid << endl;

	VIRTUALIZER_END;
	return 0;
}

#pragma optimize("", on)