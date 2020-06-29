#include "StdAfx.h"
#include "PacketCryption.h"
#include "HookManager.h"
#include "CliExt.h"

void PacketCryption::Init()
{
	if(g_CliExt.IsEnabled())
	{
		WriteCall(0x92ECA2, PacketDecryptHook);
	}
}


void PacketDecryptWrapper(LPBYTE packet, CUserSocket *pSocket, UINT size)
{
	if(g_CliExt.IsEnabled())
	{
		g_CliExt.DecryptPacket(pSocket, packet, size);
	}

	typedef void(*f)(LPBYTE, LPBYTE, UINT);
	f(0x91C148L)(packet, pSocket->inKey, size);

	if(g_CliExt.IsEnabled())
	{
		g_CliExt.HandleCheckSum(pSocket, packet, size);
	}
}

void SendKeyPacketWrapper(CUserSocket* pSocket, const char* format, BYTE opCode, BYTE param1, UINT64 key, DWORD param2, DWORD param3, BYTE param4, DWORD param5)
{
	LPBYTE lpKey = (LPBYTE)&key;

	pSocket->Send(format, opCode, param1, key, param2, param3, param4, param5);
}