#include "StdAfx.h"
#include "Guard.h"
#include "Network.h"
#include "VirtualizerSDK.h"
#include "process.h"

BOOL initializedFunctions = FALSE;

typedef HMODULE (WINAPI * _getModuleHandleA)(LPCSTR);
_getModuleHandleA getModuleHandleA = 0;

typedef HMODULE (WINAPI * _getModuleHandleW)(LPCWSTR);
_getModuleHandleW getModuleHandleW = 0;

BYTE junkMemory1[0x100];
_openProcess openProcess;
_enumWindows enumWindows;
BYTE junkMemory2[0x100];
_writeProcessMemory writeProcessMemory;
_closeHandle closeHandle;
_messageBoxA messageBoxA;
_exitWindowsEx exitWindowsEx;
BYTE junkMemory3[0x110];
_getFileSize getFileSize;
_createFile createFile;
_createThread createThread;
BYTE junkMemory4[0x120];
_exitProcess exitProcess;
_setWindowsHookExW setWindowsHookExW = 0;
_getClassNameW getClassNameW = 0;
_getWindowTextW getWindowTextW;
_getWindowThreadProcessId getWindowThreadProcessId;

BYTE junkMemory[0x4000];

#define NAME_PAIR_COUNT 4

BOOL g_RequestExit = FALSE;

TCHAR g_NamePair[NAME_PAIR_COUNT][2][25];

extern DWORD g_PacketHandlerHook;
extern DWORD g_PacketHandlerHandleAddr;


#pragma optimize ("", off)

void ValidateExitThread(LPVOID param)
{
	VIRTUALIZER_START;
	BYTE arrAGIsRequestingExit[48] = { 63, 0, 71, 198, 73, 0, 115, 117, 82, 0,101, 40, 113, 0, 117, 0, 101, 0, 115, 242, 116, 31, 105, 231, 110, 74, 103, 5, 69, 235, 120, 42, 105, 0, 116, 114, 64, 0, 64, 34, 51, 73, 72, 0, 65, 237, 0, 0 };
	char sAGIsRequestingExit[24];
	UnpackString(arrAGIsRequestingExit, sAGIsRequestingExit, 48);
	LPBOOL lpExit = (LPBOOL)Utils::GetProcAddress(g_Core, sAGIsRequestingExit);
	UINT crashTick = 0;
	try
	{
		while(crashTick < 12) 
		{
			if((*lpExit) > 0)
			{
				crashTick++;
			}
			Sleep(1000);
		}
	}catch(...)
	{
	}

	g_RequestExit = TRUE;
	//ExitProcess(0);
	LPDWORD lpCrash = 0;
	(*lpCrash) = 1234;
	_endthread();
	
	VIRTUALIZER_END;
}

#pragma optimize ("", on)

CGuard::CGuard()
{

}

CGuard::~CGuard()
{
}

void CGuard::Init()
{
	VIRTUALIZER_START;

	_beginthread(ValidateExitThread, 0, 0);
	LPBYTE lpAntiAttach = reinterpret_cast<LPBYTE>(Nt::DbgUiRemoteBreakin);
	ULONG prot = PAGE_EXECUTE_READWRITE;
	if(Utils::Protect(lpAntiAttach, 1, prot))
	{
		(*lpAntiAttach) = 0xC3;
		Utils::Protect(lpAntiAttach, 1, prot);
	}
	lpAntiAttach = reinterpret_cast<LPBYTE>(Nt::DbgUserBreakPoint);
	prot = PAGE_EXECUTE_READWRITE;
	if(Utils::Protect(lpAntiAttach, 1, prot))
	{
		(*lpAntiAttach) = 0xC3;
		Utils::Protect(lpAntiAttach, 1, prot);
	}
	lpAntiAttach = reinterpret_cast<LPBYTE>(Nt::DbgBreakPoint);
	prot = PAGE_EXECUTE_READWRITE;
	if(Utils::Protect(lpAntiAttach, 1, prot))
	{
		(*lpAntiAttach) = 0xC3;
		Utils::Protect(lpAntiAttach, 1, prot);
	}

	if(!initializedFunctions)
	{
		BYTE arrkernel32_dll[26] = { 107, 223, 101, 101, 114, 0, 110, 252, 101, 99, 108, 45, 51, 0, 50, 0, 46, 0, 100, 46, 108, 214, 108, 24, 0, 0 };
		char skernel32_dll[13];
		UnpackString(arrkernel32_dll, skernel32_dll, 26);

		memset(g_NamePair, 0, sizeof(g_NamePair));

		HMODULE hMod = Kernel::GetModuleHandleA(skernel32_dll);

		if(hMod)
		{

			BYTE arrATickUGameEngineUAEXMZ[54] = { 63, 0, 84, 42, 105, 249, 99, 167, 107, 51, 64, 106, 85, 106, 71, 93, 97, 0, 109, 243, 101, 0, 69, 253, 110, 247, 103, 198, 105, 73, 110, 0, 101, 109, 64, 0, 64, 115, 85, 200, 65, 200, 69, 0, 88, 154, 77, 0, 64, 45, 90, 0, 0, 0 };
			char sATickUGameEngineUAEXMZ[27];
			UnpackString(arrATickUGameEngineUAEXMZ, sATickUGameEngineUAEXMZ, 54);

			BYTE arrOpenProcess[24] = { 79, 106, 112, 32, 101, 0, 110, 94, 80, 181, 114, 147, 111, 4, 99, 41, 101, 0, 115, 189, 115, 66, 0, 0 };
			char sOpenProcess[12];
			UnpackString(arrOpenProcess, sOpenProcess, 24);
			openProcess = (_openProcess)Utils::GetProcAddress(hMod, sOpenProcess);
			BYTE arrVirtualProtect[30] = { 86, 0, 105, 155, 114, 0, 116, 225, 117, 149, 97,187, 108, 2, 80, 226, 114, 0, 111, 21, 116, 7, 101, 0, 99, 137, 116, 152, 0, 0 };
			char sVirtualProtect[15];
			UnpackString(arrVirtualProtect, sVirtualProtect, 30);

			BYTE arrWriteProcessMemory[38] = { 87, 141, 114, 243, 105, 0, 116, 23, 101, 195, 80, 81, 114, 4, 111, 106, 99, 0, 101, 26, 115, 75, 115, 109, 77, 17, 101, 221,109, 69, 111, 0, 114, 9, 121, 0, 0, 0 };
			char sWriteProcessMemory[19];
			UnpackString(arrWriteProcessMemory, sWriteProcessMemory, 38);
			writeProcessMemory = (_writeProcessMemory)Utils::GetProcAddress(hMod, sWriteProcessMemory);
			BYTE arrCloseHandle[24] = { 67, 24, 108, 0, 111, 0, 115, 78, 101, 0, 72, 26, 97, 0, 110, 46, 100, 145, 108, 0, 101, 167, 0, 0 };
			char sCloseHandle[12];
			UnpackString(arrCloseHandle, sCloseHandle, 24);
			closeHandle = (_closeHandle)Utils::GetProcAddress(hMod, sCloseHandle);

			BYTE arruser32_dll[22] = { 117, 0, 115, 0, 101, 9, 114, 225, 51, 0, 50, 0, 46, 0, 100, 27, 108, 0, 108, 0, 0, 0 };
			char suser32_dll[11];
			UnpackString(arruser32_dll, suser32_dll, 22);
			HMODULE hUserLib = Kernel::GetModuleHandleA(suser32_dll);

			BYTE arrGetFileSize[24] = { 71, 0, 101, 74, 116, 0, 70, 0, 105, 163, 108, 0, 101, 193, 83, 176, 105, 0, 122, 180, 101, 209, 0, 0 };
			char sGetFileSize[12];
			UnpackString(arrGetFileSize, sGetFileSize, 24);
			getFileSize = (_getFileSize) Utils::GetProcAddress(hMod, sGetFileSize);

			WORD arrZRanger[16] = { 90, 10216, 82, 0, 97, 0, 110, 8550, 103, 8602, 101, 10441, 114, 20465, 0, 0 };
			UnpackStringW(arrZRanger, g_NamePair[0][1], 16);

			WORD arrTower[12] = { 84, 31615, 111, 0, 119, 17550, 101, 0, 114, 0, 0, 0 };
			UnpackStringW(arrTower, g_NamePair[1][1], 12);

			BYTE arrCreateFileA[24] = { 67, 11, 114, 0, 101, 230, 97, 117, 116, 223, 101, 57, 70, 13, 105, 0, 108, 228, 101, 0, 65, 52, 0, 0 };
			char sCreateFileA[12];
			UnpackString(arrCreateFileA, sCreateFileA, 24);
			createFile = (_createFile)Utils::GetProcAddress(hMod, sCreateFileA);

			WORD arrTFormular[20] = { 84, 20955, 70, 9520, 111, 22, 114, 0, 109, 0, 117, 27857, 108, 21236, 97, 23009, 114, 27134, 0, 0 };
			UnpackStringW(arrTFormular, g_NamePair[2][0], 20);

			WORD arrCP_Reloader[24] = { 67, 43, 80, 13534, 32, 12477, 82, 0, 101, 19140, 108, 11533, 111, 15098, 97, 0, 100, 0, 101, 10072, 114, 860, 0, 0 };
			UnpackStringW(arrCP_Reloader, g_NamePair[2][1], 24);

			WORD arrTApplication[26] = { 84, 0, 65, 11201, 112, 26414, 112, 0, 108, 0, 105, 25109, 99, 7235, 97, 15891, 116, 21715, 105, 19965, 111, 18122, 110, 0, 0, 0 };
			UnpackStringW(arrTApplication, g_NamePair[3][0], 26);
			WORD arrPotion_User[24] = { 80, 0, 111, 13446, 116, 0, 105, 0, 111, 4197, 110, 18805, 32, 28318, 85, 28158, 115, 5423, 101, 8667, 114, 31677, 0, 0 };
			UnpackStringW(arrPotion_User, g_NamePair[3][1], 24);


			BYTE arrCreateThread[26] = { 67, 32, 114, 167, 101, 221, 97, 237, 116, 0, 101, 224, 84, 0, 104, 157, 114, 0, 101, 89, 97, 0, 100, 234, 0, 0 };
			char sCreateThread[13];
			UnpackString(arrCreateThread, sCreateThread, 26);
			createThread = (_createThread)Utils::GetProcAddress(hMod, sCreateThread);

			WORD arrwxWindowClassNR[32] = { 119, 31588, 120, 7762, 87, 0, 105, 31631, 110, 11369, 100, 17136, 111, 4106, 119, 0, 67, 0, 108, 0, 97, 0, 115, 11984, 115, 0, 78, 17675, 82, 0, 0, 0 };

			UnpackStringW(arrwxWindowClassNR, g_NamePair[0][0], 32);

			WORD arrWindowsForms10[30] = { 87, 7442, 105, 8827, 110, 17621, 100, 0, 111, 9576, 119, 0, 115, 28985, 70, 0, 111, 19730, 114, 5455, 109, 12410, 115, 0, 49, 26511, 48, 0, 0, 0 };
			UnpackStringW(arrWindowsForms10, g_NamePair[1][0], 30);

			BYTE arrExitProcess[24] = { 69, 232, 120, 203, 105, 197, 116, 239, 80, 153, 114, 0, 111, 48, 99, 8, 101, 199, 115, 132, 115, 137, 0, 0 };
			char sExitProcess[12];
			UnpackString(arrExitProcess, sExitProcess, 24);
			exitProcess = (_exitProcess)Utils::GetProcAddress(hMod, sExitProcess);

			if(hUserLib)
			{
				BYTE arrEnumWindows[24] = { 69, 223, 110, 0, 117, 237, 109, 0, 87, 180, 105, 230, 110, 250, 100, 0, 111, 0, 119, 250, 115, 18, 0, 0 };
				char sEnumWindows[12];
				UnpackString(arrEnumWindows, sEnumWindows, 24);

				enumWindows = (_enumWindows)Utils::GetProcAddress(hUserLib, sEnumWindows);

				BYTE arrGetWindowTextW[30] = { 71, 0, 101, 0, 116, 174, 87, 0, 105, 94, 110, 0, 100, 170, 111, 0, 119, 173, 84, 205, 101, 122, 120, 51, 116, 6, 87, 95, 0, 0 };
				char sGetWindowTextW[15];
				UnpackString(arrGetWindowTextW, sGetWindowTextW, 30);

				getWindowTextW = (_getWindowTextW)Utils::GetProcAddress(hUserLib, sGetWindowTextW);

				BYTE arrGetClassNameW[28] = { 71, 88, 101, 242, 116, 23, 67, 182, 108, 76, 97, 0, 115, 230, 115, 91, 78, 225, 97, 159, 109, 33, 101, 0, 87, 240, 0, 0 };
				char sGetClassNameW[14];
				UnpackString(arrGetClassNameW, sGetClassNameW, 28);

				BYTE arrGetWindowThreadProcessId[50] = { 71, 122, 101, 91, 116, 0, 87, 53, 105, 0, 110, 0, 100, 0, 111, 213, 119, 205, 84, 0, 104, 0, 114, 243, 101, 219, 97, 0, 100, 0, 80, 181, 114, 0, 111, 136, 99, 2, 101, 38, 115, 104, 115, 143, 73, 221, 100, 0, 0, 0 };
				char sGetWindowThreadProcessId[25];
				UnpackString(arrGetWindowThreadProcessId, sGetWindowThreadProcessId, 50);

				getClassNameW = (_getClassNameW)Utils::GetProcAddress(hUserLib, sGetClassNameW);
				getWindowThreadProcessId = (_getWindowThreadProcessId)Utils::GetProcAddress(hUserLib, sGetWindowThreadProcessId); 

				BYTE arrMessageBoxA[24] = { 77, 122, 101, 164, 115, 0, 115, 28, 97, 75, 103, 145, 101, 0, 66, 0, 111, 230, 120, 1, 65, 33, 0, 0 };
				char sMessageBoxA[12];
				UnpackString(arrMessageBoxA, sMessageBoxA, 24);
				messageBoxA = (_messageBoxA)Utils::GetProcAddress(hUserLib, sMessageBoxA);

				BYTE arrExitWindowsEx[28] = { 69, 137, 120, 44, 105, 118, 116, 224, 87, 5, 105,25, 110, 56, 100, 179, 111, 75, 119, 34, 115, 158, 69, 226, 120, 250, 0, 0 };
				char sExitWindowsEx[14];
				UnpackString(arrExitWindowsEx, sExitWindowsEx, 28);
				exitWindowsEx = (_exitWindowsEx)Utils::GetProcAddress(hUserLib, sExitWindowsEx);
				initializedFunctions = TRUE;
			}

		}

//		Utils::WriteHook(0xE8, WIN_MAIN_HOOK_ADDRESS, (DWORD)WinMainHook, 0);
	}

	VIRTUALIZER_END;
}

time_t g_IGBotCheckTime = 0;

bool CGuard::DetectIGBot()
{
	VIRTUALIZER_START;
	time_t currentTime = time(0);
	if(g_IGBotCheckTime < currentTime)
	{
		g_IGBotCheckTime = currentTime + 30;
		BYTE arrLineageII_dll[28] = { 76, 108, 105, 236, 110, 0, 101, 112, 97, 0, 103, 0, 101, 0, 73, 60, 73, 179, 46, 139, 100, 229, 108, 0, 108, 227, 0, 0 };
		char sLineageII_dll[14];
		UnpackString(arrLineageII_dll, sLineageII_dll, 28);

		HMODULE hModule = Kernel::GetModuleHandleA(sLineageII_dll);
		if(hModule)
		{
			CRequestExit requestExit;
			g_RequestExit = 442;
		}
		BYTE arrL2Tower_dll[24] = { 76, 0, 50, 67, 84, 69, 111, 162, 119, 195, 101, 246, 114, 211, 46, 219, 100, 0, 108, 0, 108, 0, 0, 0 };
		char sL2Tower_dll[12];
		UnpackString(arrL2Tower_dll, sL2Tower_dll, 24);

		hModule = Kernel::GetModuleHandleA(sL2Tower_dll);
		if(hModule)
		{
			CRequestExit requestExit;
			g_RequestExit = 4427;
		}
		BYTE arrL2Tower2_dll[26] = { 76, 200, 50, 0, 84, 76, 111, 0, 119, 0, 101, 0, 114, 139, 50, 57, 46, 93, 100, 246, 108, 0, 108, 0, 0, 0 };
		char sL2Tower2_dll[13];
		UnpackString(arrL2Tower2_dll, sL2Tower2_dll, 26);

		LPDWORD lpAddr = (LPDWORD)g_PacketHandlerHook;
		if((*lpAddr) != g_PacketHandlerHandleAddr)
		{
			CRequestExit requestExit;
			g_RequestExit = 2442;
		}

		hModule = Kernel::GetModuleHandleA(sL2Tower2_dll);
		if(hModule)
		{
			CRequestExit requestExit;
			g_RequestExit = 2442;
		}
		BYTE arrL2t_dll[16] = { 76, 0, 50, 0, 116, 0, 46, 105, 100, 158, 108, 0, 108, 78, 0, 0 };
		char sL2t_dll[8];
		UnpackString(arrL2t_dll, sL2t_dll, 16);

		hModule = Kernel::GetModuleHandleA(sL2t_dll);
		if(hModule)
		{
			CRequestExit requestExit;
			g_RequestExit = 4542;
		}
	}
	VIRTUALIZER_END;
	return false;
}

BOOL CALLBACK WindowsCallback(HWND hWnd, LPARAM lParam)
{
	VIRTUALIZER_START;

	BOOL ret = TRUE;
	TCHAR className[255];
	getClassNameW(hWnd, className, 255);

	LPDWORD lpProcId = (LPDWORD)lParam;
	(*lpProcId) = 0;


	for(int n=0;n<NAME_PAIR_COUNT;n++)
	{
		if(_tcsstr(className, g_NamePair[n][0]))
		{
			TCHAR windowTitle[255];
			if(getWindowTextW(hWnd, windowTitle, 255))
			{
				if(_tcsstr(windowTitle, g_NamePair[n][1]))
				{
					DWORD processId = 0;
					getWindowThreadProcessId(hWnd, &processId);
					(*lpProcId) = processId;
					ret = FALSE;
				}
			}
		}
	}

	VIRTUALIZER_END;
	return ret;
}

time_t g_ValidateRunningAppsCheckTime = 0;

bool CGuard::ValidateRunningApps()
{
	VIRTUALIZER_START;

	time_t currentTime = time(0);
	if(g_ValidateRunningAppsCheckTime < currentTime)
	{
		g_ValidateRunningAppsCheckTime = currentTime + 29;
		DWORD processId = 0;
		BOOL found = FALSE;
		if(!enumWindows(WindowsCallback, (LPARAM)&processId))
		{
			found = TRUE;
		}
		if(found)
		{
			CRequestExit requestExit;
			g_RequestExit = 5571;
		}
	}
	VIRTUALIZER_END;

	return true;
}

BOOL CALLBACK EnumerateLoadedModulesProc( PSTR moduleName, ULONG moduleBase, ULONG moduleSize, PVOID userContext )
{
	VIRTUALIZER_START;
	WCHAR moduleNameW[260];
	memset(moduleNameW, 0, sizeof(moduleNameW));

	for(UINT n=0;n<259;n++)
	{
		moduleNameW[n] = moduleName[n];
		if(moduleName[n] == 0)
		{
			break;
		}
	}
	HANDLE hDll = GetModuleHandle(moduleNameW);
	if(hDll)
	{
		FARPROC fnAddr = GetProcAddress((HMODULE)hDll, "lua_error");
		if(fnAddr)
		{
			CRequestExit requestExit;
			g_RequestExit = 5571;
		}
	}
	VIRTUALIZER_END;
    return TRUE;
}
UINT g_ValidateRunningAppsTimeoutEx = 0;

bool CGuard::ValidateRunningAppsEx()
{
	VIRTUALIZER_START;
	time_t currentTime = time(0);
	if(g_ValidateRunningAppsTimeoutEx < currentTime )
	{
		g_ValidateRunningAppsTimeoutEx = currentTime + 30;
		EnumerateLoadedModules( GetCurrentProcess(), EnumerateLoadedModulesProc, 0 );
	}
	VIRTUALIZER_END;
	return false;
}

CRequestExit::CRequestExit()
{
	VIRTUALIZER_START;
	BYTE arrAGIsRequestingExit[48] = { 63, 0, 71, 198, 73, 0, 115, 117, 82, 0,101, 40, 113, 0, 117, 0, 101, 0, 115, 242, 116, 31, 105, 231, 110, 74, 103, 5, 69, 235, 120, 42, 105, 0, 116, 114, 64, 0, 64, 34, 51, 73, 72, 0, 65, 237, 0, 0 };
	char sAGIsRequestingExit[24];
	UnpackString(arrAGIsRequestingExit, sAGIsRequestingExit, 48);
	LPBOOL lpExit = (LPBOOL)Utils::GetProcAddress(g_Core, sAGIsRequestingExit);

	if(lpExit)
	{
		(*lpExit) = 1;
	}

	g_RequestExit = TRUE;

	VIRTUALIZER_END;
}

CRequestExit::~CRequestExit()
{

}

CInitializeExt::CInitializeExt()
{
}

CInitializeExt::~CInitializeExt()
{
}

bool CInitializeExt::Init()
{
	VIRTUALIZER_START;

	wstring wTrash=L"23lk4a(*9pdfhkj320)($#)*(#@lkdsfa   asd)) -- 1243  4";
	char moduleName[256];
	Kernel::GetModuleFileNameA(Kernel::GetModuleHandle(NULL), moduleName, 255);

	BYTE arrCoded1[14] = { 108, 0, 50, 56, 46, 151, 101, 34, 120, 0, 101, 224, 0, 0 };
	char sL2_exe[7];
	UnpackString(arrCoded1, sL2_exe, 14);

	BYTE arrl2_bin[14] = { 108, 113, 50, 0, 46, 18, 98, 201, 105, 113, 110, 226, 0, 0 };
	char sl2_bin[7];
	UnpackString(arrl2_bin, sl2_bin, 14);

	if(strstr(moduleName, sL2_exe) || strstr(moduleName, sl2_bin))
	{

	}else
	{
		g_RequestExit = GetTickCount();
	}

	g_Network.Init();

	VIRTUALIZER_END;

	return false;
}

map<int, int>& CInitializeExt::GetMap()
{
	return mData;
}