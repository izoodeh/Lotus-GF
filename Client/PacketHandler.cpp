#include "stdafx.h"
#include "PacketHandler.h"
#include "VirtualizerSDK.h"
#include "HWID.h"

CPacketHandler g_PacketHandler;

DWORD g_handlePacketFunc = 0;
DWORD g_PacketHandlerHook = 0;
DWORD g_PacketHandlerHandleAddr = 0;

CHWID g_HWID;

CPacketHandler::CPacketHandler() : lpHandler(0), lpHandlerEx(0)
{

}

CPacketHandler::~CPacketHandler()
{
}

void CPacketHandler::Init()
{
	VIRTUALIZER_START;
	lpHandler = (HandlerInfo*)((DWORD)g_Engine + PACKET_HANDLER_TABLE);
	lpHandlerEx = (HandlerInfo*)((DWORD)g_Engine + PACKET_EX_HANDLER_TABLE);
	g_PacketHandlerHook = (DWORD)g_Engine + PACKET_HANDLER_OFFSET;
	g_PacketHandlerHandleAddr = (DWORD)CPacketHandler::Handle;
	g_handlePacketFunc = Utils::WriteDWORD(g_PacketHandlerHook, (DWORD)CPacketHandler::Handle);
	CClientSocket::Init();
//	DumpTable();
	VIRTUALIZER_END;
}

#pragma optimize("", off)

int __thiscall CPacketHandler::Handle(CNetworkHandler* lpNetwork, NetworkPacket *lpPacket)
{
	VIRTUALIZER_START;
	int ret = 0;
	
	typedef int (__thiscall *f)(CNetworkHandler*, NetworkPacket*);
	static f HandlePacket = (f)g_handlePacketFunc;

	if(lpPacket->id == Network::IncommingPacket::BaseId)
	{
		return HandleCustomPacket(lpNetwork->pSocket, lpPacket);
	}else if(lpPacket->id == KEY_PACKET_OPCODE && lpPacket->len == 22)
	{
		CClientSocket::OnKeyPacket(reinterpret_cast<LPBYTE>(lpPacket->buff));
	}


	ret = HandlePacket(lpNetwork, lpPacket);

	VIRTUALIZER_END;
	return ret;
}

#pragma optimize("", on)


/*
void CPacketHandler::DumpTable()
{
	wofstream file(L"PacketTable.txt");
	for(int n=0;n<0xFF;n++)
	{
		file << "ID: " << hex << n << " Name: " << lpHandler[n].wName << endl;
	}
	for(int n=0;n<0xFF;n++)
	{
		file << "ID_EX: " << hex << n << " Name: " << lpHandlerEx[n].wName << endl;
	}
	file.close();
}*/

int CPacketHandler::HandleCustomPacket(CClientSocket *pSocket, NetworkPacket* lpPacket)
{
	VIRTUALIZER_START;
	CInPacket in(lpPacket->len, lpPacket->buff);
	UINT customId = in.ReadD();
	
	switch(customId)
	{
	case Network::IncommingPacket::AntiBot:
		{
			return g_PacketHandler.RequestCheckOffsetPacket(pSocket, in);
		}
	case Network::IncommingPacket::HardwareId:
		{
			return g_PacketHandler.RequestHardwareIdPacket(pSocket, in);
		}
	}

	VIRTUALIZER_END;
	return 0;
}

int CPacketHandler::RequestHardwareIdPacket(CClientSocket *pSocket, CInPacket& packet)
{
	VIRTUALIZER_START;

	COutPacket pck;
	pck.WriteC(Network::OutgoingPacket::BaseId);
	pck.WriteD(Network::OutgoingPacket::HardwareId);

	UINT id = 0;
	g_HWID.Init();
	id = g_HWID.Get();
	pck.WriteD(id);
	LPBYTE lpHash = g_HWID.GetHash();
	for(UINT i=0;i<32;i++)
	{
		pck.WriteC(lpHash[i]);
	}
	pSocket->Send(pck.GetLen(), pck.GetBuff());

	VIRTUALIZER_END;
	return 0;
}

int CPacketHandler::RequestCheckOffsetPacket(CClientSocket *pSocket, CInPacket &packet)
{
	VIRTUALIZER_START;
	DWORD count = packet.ReadD();
	DWORD errorCode = 0;

	COutPacket pck;
	pck.WriteC(Network::OutgoingPacket::BaseId);
	pck.WriteD(Network::OutgoingPacket::AntiBot);
	pck.WriteD(count);

	for(DWORD n = 0; n < count; n++)
	{
		DWORD type = packet.ReadD();
		DWORD offset = packet.ReadD();
		pck.WriteD(type);
		pck.WriteD(offset);
		if(type == ModuleEngine)
		{				
			pck.WriteC(Utils::GetModuleByte(g_Engine, offset));
		}else if(type == ModuleCore)
		{
			pck.WriteC(Utils::GetModuleByte(g_Core, offset));
		}else if(type == ModuleAntiBot)
		{
			pck.WriteC(Utils::GetModuleByte(g_Client, offset));
		}
	}
	
	pSocket->Send(pck.GetLen(), pck.GetBuff());


	VIRTUALIZER_END;
	return 0;
}