#include "StdAfx.h"
#include "Network.h"
#include "Guard.h"
#include "VirtualizerSDK.h"


CNetwork g_Network;

BYTE trashMemory[8192];

extern BOOL g_RequestExit;

void __declspec(naked) JumpSomewhere(DWORD addr)
{
	__asm
	{
		jmp [addr]
	};
};

CNetwork::CNetwork()
{
}

CNetwork::~CNetwork()
{
}

void CNetwork::Init()
{
	VIRTUALIZER_START;

	for(int n=0;n<4023;n++)
	{
		trashMemory[n] = (BYTE)(rand() % 255);
	}

	CGuard guard;
	guard.Init();

	for(int n=4023;n<8192;n++)
	{
		trashMemory[n] = (BYTE)(rand() % 255);
	}
	
	VIRTUALIZER_END;
}