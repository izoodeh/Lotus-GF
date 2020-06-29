// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

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

#ifndef _WIN64
#define _WIN64
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "wbemuuid.lib")
#define _CRT_RAND_S
// Windows Header Files:
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <bitset>
#include <algorithm>
#include <tchar.h>
#include <comdef.h>
#include <Wbemidl.h>
using namespace std;

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

// A TCHAR based std::string
#if defined UNICODE || defined _UNICODE
typedef wstring tstring;
typedef wstringstream   tstringstream;
#else
typedef string tstring;
typedef stringstream   tstringstream;
#endif

#define USE_CALL_TRACK 1
#define NPC_CLASS_COUNT 15000
#define DLL_SIZE	  2150400

class MemoryObject;
class CBaseObject;
class CObject;
class CTrade;
class CCreature;
class User;
class CUserSocket;
class CSkillInfo;
class CAlliance;

typedef vector<User*> UserVector;
typedef bool (*UserCallback)(CUserSocket*, const unsigned char*);
typedef bitset<sizeof(int)> IntBits;

extern WCHAR g_ConfigFile[260];
extern HMODULE g_DllModule;
extern time_t g_ServerStartTime;
extern UINT g_ServerVersion;
extern UINT g_DropGetReuse;

#include "BaseEnum.h"
#include "BaseStruct.h"
#include "Parser.h"
#include "Feature.h"
#include "Log.h"
#include "Random.h"
#include "Utils.h"
#include "Logger.h"
#include "CallStack.h"
#include "SharedFactory.h"
#include "UserSocket.h"
#include "User.h"
#include "Pet.h"
#include "Pledge.h"
#include "Item.h"
#include "SkillInfo.h"
#include "xwstring.h"