// CliExt.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CliExt.h"
#include "ClientSocket.h"
#include "NetworkHandler.h"
#include "MemoryProtector.h"
#include "Crypt.h"

BOOL g_Initialized = FALSE;
HMODULE g_Engine = 0;
UINT g_AuthPort = 2106;

HMODULE g_CliExt = 0;
HANDLE g_HardwareIdSM = 0;
LPBYTE g_lpHardwareIdSM = 0;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

CLIEXT_API BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeExtender(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#pragma optimize("", off)
void InitializeExtender(HMODULE hModule)
{
	if(g_Initialized == FALSE)
	{
		TCHAR path[260];
		if(GetModuleFileName(0 , path, 260))
		{
			wstring wPath(path);
			size_t lastPos = 0;
			size_t temp = wPath.find(L"\\");
			while(temp != wstring::npos)
			{
				lastPos = temp;
				temp = wPath.find(L"\\", lastPos+1);
			}
			if(lastPos > 0)
			{
				wPath = wPath.substr(0, lastPos);
				SetCurrentDirectory(wPath.c_str());
			}
		}
		VIRTUALIZER_START;
		g_CliExt = hModule;
		g_Initialized = TRUE;
		const WCHAR engineName[] = CRYPT32W(L"Engine.dll");
		g_Engine = GetModuleHandle(DECRYPT32W(engineName));
		if(g_Engine)
		{
			//change 222.231.15.60 to
			//127.0.0.1
			const char *sIP = "127.0.0.1";
			UINT addr = (UINT)g_Engine + 0x47E7F4;
			Memory::Write(addr, (LPVOID)sIP, 10);

			const TCHAR* section = _T("Setting");
			TCHAR configFile[256];
			GetCurrentDirectory(MAX_PATH, configFile);
			lstrcat(configFile, _T("\\Client.ini"));

			g_AuthPort = GetPrivateProfileInt(section, _T("AuthPort"), 2106, configFile);
			Memory::WriteDWORD((UINT)g_Engine + 0x3A5C4E, g_AuthPort);
		}
		CClientSocket::Init();
		g_NetworkHandler.Init();
		g_MemoryProtector.Init();
		VIRTUALIZER_END;
	}
}

#pragma optimize("", on)