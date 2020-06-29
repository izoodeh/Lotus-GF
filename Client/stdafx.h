// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma comment(lib,"ws2_32.lib")
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#pragma comment(lib,"../Lib/x86/ntdll.lib")
#pragma comment(lib, "Dbghelp.lib")
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include <math.h>
#include <time.h>
#include <tchar.h>
#include <dbghelp.h>
#include "resource.h"
using namespace std;

#define DllExport __declspec(dllexport)
//87 protocol
#define PACKET_HANDLER_TABLE 0x663B50
#define PACKET_EX_HANDLER_TABLE 0x665F50
#define PACKET_HANDLER_OFFSET 0x493D04
#define PACKET_CRYPTION_OFFSET 0x493C70
//83 protocol
//#define PACKET_HANDLER_TABLE 0x66CF80
//#define PACKET_EX_HANDLER_TABLE 0x66F380
//#define PACKET_HANDLER_OFFSET 0x49D4B4
//#define PACKET_CRYPTION_OFFSET 0x49D420
#define KEY_PACKET_OPCODE 0x2E
//#define WIN_MAIN_ADDRESS 0x109264F0L
//#define WIN_MAIN_HOOK_ADDRESS 0x10917788
#define CUSTOM_PACKET_IN_OPCODE 0x8D
#define CUSTOM_PACKET_OUT_OPCODE 0xA

typedef UINT64 QWORD;
typedef UINT64 near          *PQWORD;

//#define RUSSIAN_VERSION 1

extern HANDLE g_NtDll;
extern HANDLE g_Kernel32;
extern HANDLE g_Engine;
extern HANDLE g_Core;
extern HANDLE g_Client;

enum ModuleType
{
	ModuleNone = 0,
	ModuleEngine = 1,
	ModuleCore = 2,
	ModuleAntiBot = 3,
	ModuleHWID = 4,
	ModuleConfirmation1 = 5,
	ModuleConfirmation2 = 6
};

#include "Utils.h"
#include "Packet.h"
#include "ClientSocket.h"