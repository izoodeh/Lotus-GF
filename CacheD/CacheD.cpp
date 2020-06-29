// CacheD.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PostLoad.h"
#include "AdminPacketHandler.h"

const WCHAR * wItemData = L"..\\Script\\itemdata.txt";
const WCHAR * wWatchList = L"..\\Script\\watch_list.txt";
const WCHAR * wItemCache = L"..\\Script\\itemcache.txt";
const WCHAR * wGMSkillData = L"..\\Script\\GMskilldata.txt";
const WCHAR * wQuestComp = L"..\\Script\\questcomp.txt";

#ifdef _MANAGED
#pragma managed(push, off)
#endif

DLLEXPORT BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			PostLoad::Init();
			WriteNOP(0x461C6D, 5);
			WriteNOP(0x461EC4, 5);
			WriteNOP(0x40E7B4, 5); //SetQuestPacket
			wcscpy_s((PWCHAR)0x62DC28, 28, wItemData);
			wcscpy_s((PWCHAR)0x663B20, 30, wWatchList);
			wcscpy_s((PWCHAR)0x5ECCE8, 29, wItemCache);
			wcscpy_s((PWCHAR)0x5ECE50, 31, wGMSkillData);
			wcscpy_s((PWCHAR)0x5ECF98, 29, wQuestComp);
			g_AdminPacketHandler.Init();
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

