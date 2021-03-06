#include "StdAfx.h"
#include "PostLoad.h"
#include "PacketHandler.h"
#include "DBConn.h"
#include "Action.h"

void PostLoad::Init()
{
	WriteCall(0x44BD28, PostLoad::FirstLoad);
}

void PostLoad::FirstLoad()
{
	typedef void(*f)();
	f(0x485C2CL)();

	g_Log.Add(CLog::Blue, L"ーInitializing Packet Handler");
	g_PacketHandler.Init();
	MovLogFiles(TEXT("LogBackup"));
	Action::Init();

	
	INT32 points = 0;
	CDBConn db;

	const WCHAR* stm = L"SELECT COUNT(*) FROM [dbo].[user_data]";
	db.Bind(&points);
	db.PrepareStatement(stm);
	if(db.ExecutePrepared(stm))
	{
		db.Fetch();
	}
	db.UnBind();
	db.ResetParam();

	INT32 items = 0;
	const WCHAR* stm2 = L"SELECT COUNT(*) FROM [dbo].[user_item]";
	db.Bind(&items);
	db.PrepareStatement(stm2);
	if(db.ExecutePrepared(stm2))
	{
		db.Fetch();
	}

	g_Log.Add(CLog::Blue, L"ーUsers in database: %d ", points);
	g_Log.Add(CLog::Blue, L"ーItems in database: %d ", items);
}