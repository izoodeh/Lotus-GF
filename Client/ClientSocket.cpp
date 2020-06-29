#include "stdafx.h"
#include "ClientSocket.h"
#include "Guard.h"
#include "VirtualizerSDK.h"
#include "NWindow.h"

extern HANDLE g_Engine;

DWORD g_CryptPacketFunc = 0;
DWORD g_CryptPacketHook = 0;
DWORD g_CryptPacketAddr = 0;

BYTE g_OutKey[4];

LONG pckCounter = 4;

BOOL g_FirstRun = TRUE;

void CClientSocket::Init()
{
	VIRTUALIZER_START;

	g_CryptPacketHook = (DWORD)g_Engine + PACKET_CRYPTION_OFFSET;
	g_CryptPacketAddr = (DWORD)CClientSocket::Crypt;
	g_CryptPacketFunc = Utils::WriteDWORD(g_CryptPacketHook, g_CryptPacketAddr);

	for(int n=0;n<4;n++)
	{
		g_OutKey[n] = (BYTE)(rand() % 255);
	}

	VIRTUALIZER_END;
}

#pragma optimize("", off)

void CClientSocket::OnKeyPacket(LPBYTE lpPacket)
{
	VIRTUALIZER_START;

	LPBYTE lpKey = &lpPacket[1];
	BYTE xor = 46;
	UINT keyIndex = 0;
	for(INT32 n=0;n<8;n++)
	{
		if( (n%2) != 0)
		{
			g_OutKey[keyIndex] = lpKey[n];
			keyIndex++;
		}
		lpKey[n] ^= (xor*(n+7));
	}

	CGuard guard;
	guard.DetectIGBot();
	guard.ValidateRunningApps();
	guard.ValidateRunningAppsEx();
	Utils::CompareMemory();

	g_NWindow.Init();
	
	VIRTUALIZER_END;
}

void __thiscall CClientSocket::Crypt(CClientSocket *pSocket, LPBYTE lpBuff, LPBYTE lpKey, INT32 size)
{
	VIRTUALIZER_START;

	typedef void (__thiscall *f)(CClientSocket*, LPBYTE, LPBYTE, INT32);
	static f CryptPacket = (f)g_CryptPacketFunc;

	LONG cnt = InterlockedIncrement(&pckCounter);

	CGuard guard;

	if((cnt % 20 ) == 0 )
	{
		guard.DetectIGBot();
	}

	if((cnt % 30) == 0)
	{
		guard.ValidateRunningAppsEx();
	}else if((cnt % 49) == 0)
	{
		guard.ValidateRunningApps();
	}

	CryptPacket(pSocket, lpBuff, lpKey, size);

	for(INT32 n = 0; n< size; n++)
	{
		if( (n%3) == 0 )
		{
			lpBuff[n] ^= g_OutKey[0];
			lpBuff[n] += g_OutKey[1];
			g_OutKey[2]++;
			g_OutKey[0]++;

		}else
		{
			lpBuff[n] ^= g_OutKey[2];
			lpBuff[n] += g_OutKey[3];
			g_OutKey[3]++;
			g_OutKey[1]++;
		}
	}

	VIRTUALIZER_END;
}

#pragma optimize("", on)


void CClientSocket::Send(int len, char *buff)
{
	DWORD funcAddr = (*(LPDWORD)this);
	funcAddr += 0x6C;
	funcAddr = (*(LPDWORD)funcAddr);

	typedef void (__cdecl *f)(CClientSocket*, const char*, int, char*);
	f((PVOID)funcAddr)(this, "b", len, buff);
}