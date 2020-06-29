// L2Npc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

void PrintFile()
{
	const WCHAR** lpHandlers = (const WCHAR**)0x7643F0;
	int n=0;
	wstringstream fi(L"handlers.txt");
	while(lpHandlers[n] != 0)
	{
		fi << n << L"|" << lpHandlers[n] << endl;
		n++;
	}
	wstring str = fi.str();
	WriteFileBinary("handlers.txt", (LPBYTE)str.c_str(), str.size()*2);
}

DLLEXPORT BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
		//	PrintFile();
			//nop mail to ncsoft
			WriteNOP(0x42BF85, 5);
			WriteNOP(0x42C1CF, 5);
			WriteNOP(0x42C444, 5);
			WriteNOP(0x42C5FF, 5);
			WriteNOP(0x587C17, 5); // nop too far broadcast script event
			MovLogFiles(TEXT("LogBackup"));
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			break;
		}
	default:
		{
			break;
		}
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

