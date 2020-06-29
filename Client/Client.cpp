#include "stdafx.h"
#include "Network.h"
#include "Guard.h"
#include "PacketHandler.h"
#include "VirtualizerSDK.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HANDLE g_NtDll = 0;
HANDLE g_Kernel32 = 0;
BYTE trashArray1[324];
HANDLE g_Engine = 0;
BYTE trashArray2[3234];
HANDLE g_Core = 0;
HANDLE g_Client = 0;
HANDLE g_DllFileHandle = 0;
UINT g_AuthPort = 2106;

char newDllName[260];

bool InitializeExtender(HMODULE hModule)
{
	VIRTUALIZER_START;

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

	_WIN32_FIND_DATAA FileData;
	HANDLE hFile = FindFirstFileA("*.ds", &FileData);
	if(hFile != INVALID_HANDLE_VALUE )
	{
		FindClose(hFile);
		DeleteFileA(FileData.cFileName);
	}
	hFile = FindFirstFileA("*.ds", &FileData);
	if(hFile != INVALID_HANDLE_VALUE )
	{
		FindClose(hFile);
		DeleteFileA(FileData.cFileName);
	}

	BYTE arrntdll_dll[20] = { 110, 0, 116, 148, 100, 23, 108, 163, 108, 197, 46, 0, 100, 198, 108, 0, 108, 56, 0, 0 };
	char sntdll_dll[10];

	g_NtDll = GetModuleHandleA(UnpackString(arrntdll_dll, sntdll_dll, 20));

	BYTE arrkernel32_dll[26] = { 107, 223, 101, 101, 114, 0, 110, 252, 101, 99, 108, 45, 51, 0, 50, 0, 46, 0, 100, 46, 108, 214, 108, 24, 0, 0 };
	char skernel32_dll[13];

	g_Kernel32 = GetModuleHandleA(UnpackString(arrkernel32_dll, skernel32_dll, 26));


	bool loader = false;

	if(!loader)
	{

		//Fix files are corrupted error
	//	Utils::WriteHook(0xE9, 0x10909BAD, 0x10909B5C, 0);
	//	Utils::WriteBYTE(0x10926A5D, 0x10);

		loader = true;
		BYTE arrengine_dll[22] = { 101, 212, 110, 104, 103, 182, 105, 115, 110, 163, 101, 110, 46, 0, 100, 217, 108, 55, 108, 17, 0, 0 };
		char sengine_dll[11];
		UnpackString(arrengine_dll, sengine_dll, 22);

		g_Engine = Kernel::GetModuleHandleA(sengine_dll);
		if(g_Engine)
		{
			//change 222.231.15.60 to
			//127.0.0.1
			const char *sIP = "127.0.0.1";
			UINT addr = (UINT)g_Engine + 0x47E7F4;
			Utils::WriteBYTES(addr, (PBYTE)sIP, 10);

			const TCHAR* section = _T("Setting");
			TCHAR configFile[256];
			GetCurrentDirectory(MAX_PATH, configFile);
			lstrcat(configFile, _T("\\Client.ini"));

			g_AuthPort = GetPrivateProfileInt(section, _T("AuthPort"), 2106, configFile);
			Utils::WriteDWORD((UINT)g_Engine + 0x3A5C4E, g_AuthPort);
		}

		BYTE arrcore_dll[18] = { 99, 81, 111, 0, 114, 0, 101, 0, 46, 114, 100, 0, 108, 98, 108, 104, 0, 0 };
		char score_dll[9];
		UnpackString(arrcore_dll, score_dll, 18);

		g_Core = Kernel::GetModuleHandleA(score_dll);

		g_PacketHandler.Init();
		

		srand((UINT)time(NULL));

		CGuard guard;
		guard.Init();

		BYTE arrshutdown__s__t_1[34] = { 115, 73, 104, 100, 117, 227, 116, 174, 100, 0,111, 136, 119, 254, 110, 0, 32, 0, 45, 59, 115, 0, 32, 110, 45, 14, 116, 9, 32,246, 49, 0, 0, 0 };
		char sshutdown__s__t_1[17];
		UnpackString(arrshutdown__s__t_1, sshutdown__s__t_1, 34);

		CInitializeExt initExt;

		g_Client = hModule;


		if(!initExt.Init())
		{
			Utils::DestroySectionHeader();
		}
	}

	VIRTUALIZER_END;

	BYTE arrshutdown__s__t_2[34] = { 115, 73, 104, 100, 117, 227, 116, 174, 100, 0,111, 136, 119, 254, 110, 0, 32, 0, 45, 59, 115, 0, 32, 110, 45, 14, 116, 9, 32,246, 49, 0, 0, 0 };
	char sshutdown__s__t_2[17];
	UnpackString(arrshutdown__s__t_2, sshutdown__s__t_2, 34);
	string sTest =  sshutdown__s__t_2;

	return loader;
}


DllExport BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{			
			if(InitializeExtender(hModule))
			{
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory( &si, sizeof(si) );
				si.cb = sizeof(si);
				ZeroMemory( &pi, sizeof(pi) );

				WCHAR cmdLine[260];
				GetCurrentDirectory(MAX_PATH, cmdLine);
				lstrcat(cmdLine, _T("\\extra.exe"));

				// Start the child process. 
				if( CreateProcess( NULL,   // No module name (use command line)
					cmdLine,        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si,            // Pointer to STARTUPINFO structure
					&pi )           // Pointer to PROCESS_INFORMATION structure
				) 
				{
					// Close process and thread handles. 
					CloseHandle( pi.hProcess );
					CloseHandle( pi.hThread );
				}
			}
			break;
		}
		case DLL_PROCESS_DETACH:
			{
				break;
			}
		default:
			break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
