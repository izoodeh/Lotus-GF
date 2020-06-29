#include "StdAfx.h"
#include "NpcServer.h"
#include "ObjectDB.h"

CNpcServer g_NpcServer;

CNpcServer::CNpcServer()
{
	lpInstance = (CNpcServer*)0x1634170;
}

CNpcServer::~CNpcServer()
{

}

void CNpcServer::Send(const char *format, ...)
{
	va_list va;
	va_start(va, format);
	char buff[8192];
	int len = VAssemble(buff, 8192, format, va);
	typedef void(*f)(CNpcServer*, const char* format, UINT len, char*);
	f(0x72D09CL)(lpInstance, "b", len, buff);
	va_end(va);
}

void CNpcServer::SpawnNpc(UINT npcClassId, int x, int y, int z)
{
	const WCHAR* lpName = g_ObjectDB.GetName(npcClassId);
	if(lpName)
	{
		Send("cSddddSd", 0xB, lpName, x, y, z, 0, L"", 0);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Cannot find npc [%d]", __FUNCTION__, npcClassId);
	}
}