#include "StdAfx.h"
#include "BuilderCmd.h"
#include "HookManager.h"
#include "SkillDB.h"
#include "DB.h"
#include "IOBufferPool.h"
#include "ObjectDB.h"
#include "NpcServer.h"
#include "EventMatchMgr.h"
#include "Championship.h"
#include "CliExt.h"
#include "UserDb.h"
#include "SkillAcquireDB.h"
#include "TerritoryData.h"
#include "DominionManager.h"

extern BYTE g_RestartExe[26112];
extern UINT g_ShutdownTime;

CBuilderCmd g_BuilderCmd;

BuilderCmdInfo builderCmdInfoArray[BUILDER_CMD_LIMIT];

CBuilderCmd::CBuilderCmd() : count(0)
{
	SetName(L"Builder Cmd");
	memset(builderCmdInfoArray, 0, sizeof(builderCmdInfoArray));
}

CBuilderCmd::~CBuilderCmd()
{
}

void CBuilderCmd::LoadData()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//BuilderCmd.txt", lpData);
	if(lpData && loaded > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		int lineCount = 0;
		wstring line;
		while(getline(file, line))
		{
			lineCount++;
			if(line.size() > 4)
			{
				if( line[0] == L'/' || line[0] == L';' )
					continue;

				if( line.find(L"cmd_begin") == 0 )
				{
					//cmd_begin	name=stopsay	required_builder_level=9	cmd_end
					wstring wName = ParseOptionString(line, L"name");
					int requiredLevel = ParseOptionInt(line, L"required_builder_level", 1);
					
					if(wName.size() > 0 && requiredLevel > 0)
					{
						mData.insert(pair<wstring, int>(wName, requiredLevel));
					}else
					{
						g_Log.Add(CLog::Error, "[%S] Parser error line[%d]!", lineCount);
					}
				}
			}
		}
		delete [] lpData;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//BuilderCmd.txt !", GetName());
	}
}

void CBuilderCmd::AddHandler(const WCHAR* wName, INT64 requiredLevel, BuilderCmdCallback func)
{
	if(count < BUILDER_CMD_LIMIT)
	{
		for(int n = 0; n < count;n++)
		{
			map<wstring, int>::iterator Iter = mData.find(wName);
			if(Iter!=mData.end())
			{
				requiredLevel = Iter->second;
			}
			if(!wcscmp(builderCmdInfoArray[n].lpName, wName))
			{
				if( count >= 540 )
				{
					g_Log.Add(CLog::Error, "[%S] Handler[%S] already exist - cannot add!", GetName(), wName);
				}
				return;
			}
		}

		builderCmdInfoArray[count].lpName = wName;
		builderCmdInfoArray[count].func = func;
		builderCmdInfoArray[count].requiredLevel = requiredLevel;
		count++;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] handlerCount reached the limit[%d]!", GetName(), BUILDER_CMD_LIMIT);
	}
}

void CBuilderCmd::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	LoadData();

	//Load l2server's cmd
	BuilderCmdInfo* lpOrgTable = (BuilderCmdInfo*)0xE4E9D0;
	for(int n=0;n<540;n++)
	{
	//	g_Log.Add(CLog::Blue, "[%S] loading [%S] builder cmd index[%d] requiredLevel[%d]", GetName(), lpOrgTable[n].lpName, n, lpOrgTable[n].requiredLevel);
		AddHandler(lpOrgTable[n].lpName, lpOrgTable[n].requiredLevel, lpOrgTable[n].func);
	}

	//Add new one
	AddHandler(L"test_cmd", 1, BuilderCmd_TestCmd);
	AddHandler(L"set_vote_points", 1, BuilderCmd_SetVotePoints);
	AddHandler(L"set_pvp_points", 1, BuilderCmd_SetPvPPoints);
	AddHandler(L"set_donate_poins", 1, BuilderCmd_SetDonatePoints);
	AddHandler(L"giveitem2", 1, BuilderCmd_GiveItem);
	AddHandler(L"deleteitem", 1, BuilderCmd_DeleteItem);
	AddHandler(L"add_to_npcpos", 1, BuilderCmd_AddToNpcPos);
	AddHandler(L"set_hero_type", 1, BuilderCmd_SetHeroType);
	AddHandler(L"set_nobless_type", 1, BuilderCmd_SetNoblessType);
	AddHandler(L"set_transferred_pvp_points", 1, BuilderCmd_SetTransferredPvPPoints);
	AddHandler(L"record_range", 1, BuilderCmd_RecordRange);
	AddHandler(L"ban_hwid", 1, BuilderCmd_BanHWID);
	AddHandler(L"unban_hwid", 1, BuilderCmd_UnbanHWID);
	AddHandler(L"reload_banned_hwid", 1, BuilderCmd_ReloadBannedHWID);
	AddHandler(L"shutdown", 1, BuilderCmd_Shutdown);
	AddHandler(L"restart", 1, BuilderCmd_Restart);
	AddHandler(L"reload_territorydata", 1, BuilderCmd_ReloadTerritoryData);
	AddHandler(L"set_dominion_siege_state", 1, BuilderCmd_SetDominionSiegeState);
	AddHandler(L"check_critical_damage", 1, BuilderCmd_CheckCriticalDamage);

//replace old table
//	.text:00000000004E2823                 lea     rsi, BuilderCmd_DataTable
//	.text:00000000004E282A
	INT64 relative = (INT64)&builderCmdInfoArray[0] - 0x4E282A;
	WriteDWORD(0x4E2826, (DWORD)relative);

//.text:00000000004E2966                 lea     rax, dword_E51C70
//.text:00000000004E296D                 cmp     rsi, rax
	relative = (INT64)&builderCmdInfoArray[count] - 0x4E296D;
	WriteDWORD(0x4E2969, (DWORD)relative);
}

void CBuilderCmd::OnLeaveWorld(User *pUser)
{
	guard;
	if(pUser->pSD->builder_level > 0)
	{
		//remove from gm list
		typedef LPVOID (*getInstance)();
		LPVOID lpInstance = getInstance(0x7BEB8CL)();
		typedef bool (*f)(LPVOID, const WCHAR*);
		//CGMListMgr::Delete
		f(0x7BB7A4L)(lpInstance, pUser->pSD->wName);
	}
	unguard;
}

bool BuilderCmd_CheckCriticalDamage(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	if(User *pTarget = User::GetUser(&pUser->targetId))
	{
		wstringstream msg;
		msg << L"Target: " << pTarget->pSD->wName << endl;
		msg << L"CriticalDamageM: " << pTarget->criticalDamageM << endl;
		msg << L"CriticalDamageA: " << pTarget->criticalDamageA << endl;
		pSocket->SendSystemMessage(msg.str().c_str());
	}else
	{
		pSocket->SendSystemMessage(L"Invalid target!");
	}
	return false;
}

bool BuilderCmd_SetDominionSiegeState(CUserSocket *pSocket, User *pUser, WCHAR *wParams)
{
	guard;

	wstringstream str(wParams);
	UINT state = 0;
	if(str >> state)
	{
		if(state < 5)
		{
			g_DominionManager.ChangeState(state, true);
			g_Log.Add(CLog::Blue, "[%s] User[%S] changed state to [%d]", __FUNCTION__, pUser->pSD->wName, state);
		}
	}else
	{
		pUser->pUserSocket->SendSystemMessage(L"format: //set_dominion_siege_state [state]   - 0 - finished siege, 1 - castle siege, 2 - registration, 3 - stand_by, 4 - siege");
	}

	unguard;
	return false;
}


bool BuilderCmd_ReloadTerritoryData(CUserSocket *pSocket, User *pUser, WCHAR *wParams)
{
	guard;
	g_TerritoryChecker.Init();
	pUser->pUserSocket->SendSystemMessage(L"TerritoryData has been reloaded!");
	unguard;
	return false;
}

bool BuilderCmd_Restart(CUserSocket *pSocket, User *pUser, WCHAR *wParams)
{
	guard;

	wstringstream str(wParams);
	int seconds = 0;
	if(str >> seconds)
	{
		if(seconds > 0)
		{
			g_ShutdownTime = seconds;
			LPBYTE lpShutDown = (LPBYTE)0xE54EE0L;
			if(*lpShutDown)
			{
				typedef CShutDownMessage* (*f)(int);
				CShutDownMessage *pMessage = f(0x4173DCL)(20);
				if(pMessage)
				{
					(*lpShutDown) = 0;
					typedef void (*f2)(CShutDownMessage*);
					f2(0x68E46CL)(pMessage);	//MemoryObject constructor
					(*(PUINT64)pMessage) = 0xB6C4D8;	//insert CShutDownMessage vt

					typedef void(*f3)(CShutDownMessage*, UINT);
					f3(0x691080L)(pMessage, 0);

					Utils::WriteFileBinary("Restart.exe", g_RestartExe, 26112);

					//start Restart.exe
					STARTUPINFOA si;
					memset(&si, 0, sizeof(si));
					si.cb = sizeof(si);
					PROCESS_INFORMATION pi;
					memset(&pi, 0, sizeof(pi));

					CHAR path[260];
					CHAR directory[260];
					GetCurrentDirectoryA(MAX_PATH, path);
					GetCurrentDirectoryA(MAX_PATH, directory);
					lstrcatA(path, "\\Restart.exe");
					CreateProcessA(0, path, 0, 0, FALSE, 0, 0, 0, &si, &pi);
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"Server is already restarting...");
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"format: //restart [time_in_seconds]");
		}
	}else
	{
		pUser->pUserSocket->SendSystemMessage(L"format: //restart [time_in_seconds]");
	}

	unguard;
	return false;
}

bool BuilderCmd_Shutdown(CUserSocket *pSocket, User *pUser, WCHAR *wParams)
{
	guard;

	wstringstream str(wParams);
	int seconds = 0;
	if(str >> seconds)
	{
		if(seconds > 0)
		{
			g_ShutdownTime = seconds;
			LPBYTE lpShutDown = (LPBYTE)0xE54EE0L;
			if(*lpShutDown)
			{
				typedef CShutDownMessage* (*f)(int);
				CShutDownMessage *pMessage = f(0x4173DCL)(20);
				if(pMessage)
				{
					(*lpShutDown) = 0;
					typedef void (*f2)(CShutDownMessage*);
					f2(0x68E46CL)(pMessage);	//MemoryObject constructor
					(*(PUINT64)pMessage) = 0xB6C4D8;	//insert CShutDownMessage vt

					typedef void(*f3)(CShutDownMessage*, UINT);
					f3(0x691080L)(pMessage, 0);
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"Server is already shutting down...");
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"format: //shutdown [time_in_seconds]");
		}
	}else
	{
		pUser->pUserSocket->SendSystemMessage(L"format: //shutdown [time_in_seconds]");
	}

	unguard;
	return false;
}

bool BuilderCmd_ReloadBannedHWID(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
//	g_BannedHardwareId.Init();
	pSocket->SendSystemMessage(L"Unused command!");
	return false;
}

bool BuilderCmd_UnbanHWID(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	wstring opt;
	if(str >> opt)
	{
		if(opt.size() == 32)
		{
			BYTE hardwareId[32];
			for(UINT n=0;n<32;n++)
			{
				hardwareId[n] = (BYTE)opt[n];
			}
			g_CliExt.UnBan(hardwareId, pUser->pSD->wName);
			pSocket->SendSystemMessage(L"Command accepted!");
		}else
		{
			pSocket->SendSystemMessage(L"Invalid hardware id!");
		}
	}
	return false;
}

bool BuilderCmd_BanHWID(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	UINT timeout;
	wstring hwid;
	if(str >> timeout)
	{
		timeout += static_cast<UINT>(time(0));
		if(str >> hwid)
		{
			HWID hw(hwid);
			g_CliExt.Ban(hw.hash, L"None", pUser->pSD->wName, timeout);
			g_UserDb.KickUsersByHWID(hw.hash);
			pSocket->SendSystemMessage(L"Command accepted!");		
		}else
		{
			if(User *pTarget = User::GetUser(&pUser->targetId))
			{
				g_CliExt.Ban(pTarget->pUserSocket->hardwareId.hash, pTarget->pUserSocket->accountNameW, pUser->pSD->wName, timeout);
				g_UserDb.KickUsersByHWID(pTarget->pUserSocket->hardwareId.hash);
				pSocket->SendSystemMessage(L"Command accepted!");		
			}else
			{
				pSocket->SendSystemMessage(L"Invalid target!");
			}
		}
	}else
	{
		pSocket->SendSystemMessage(L"Usage: //ban_hwid [timeout_in_seconds] [hwid - optional]");
	}
	return false;
}

bool BuilderCmd_TestCmd(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{	
	wstringstream str(wParams);
	int classType, level;
	if(str >> classType >> level)
	{
		stringstream fn;
		fn << classType << "_" << level << ".txt";
		ofstream file(fn.str().c_str());
		const vector<SkillAcquireInfo*>& skills = g_SkillAcquireDB.GetAcquirableSkills(classType);
		for(UINT n=0;n<skills.size();n++)
		{
			SkillAcquireInfo *pInfo = skills[n];
			if(pInfo->requiredLevel <= level)
			{
				file << "skillId=" << pInfo->skillId << "\tlevel=" << level << "\trequired_level=" << pInfo->requiredLevel << endl;
			}
		}
		file.close();
	}

	return false;
}

bool BuilderCmd_RecordRange(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	wstring opt;
	if(str >> opt)
	{	
		if(opt == L"on")
		{
			pUser->pEUD->builderUser.isRecordingRange = TRUE;
			pUser->pEUD->builderUser.rangeZone[0][0] = pUser->pSD->pos.x;
			pUser->pEUD->builderUser.rangeZone[0][1] = pUser->pSD->pos.x;
			pUser->pEUD->builderUser.rangeZone[1][0] = pUser->pSD->pos.y;
			pUser->pEUD->builderUser.rangeZone[1][1] = pUser->pSD->pos.y;
			pUser->pEUD->builderUser.rangeZone[2][0] = pUser->pSD->pos.z;
			pUser->pEUD->builderUser.rangeZone[2][1] = pUser->pSD->pos.z;

			pUser->pUserSocket->SendSystemMessage(L"Recording range mode: on");
		}else if(opt == L"show")
		{
			wstringstream rangeStream;
			rangeStream << "Min: " << pUser->pEUD->builderUser.rangeZone[0][0] << "/" << pUser->pEUD->builderUser.rangeZone[1][0] << "/" << pUser->pEUD->builderUser.rangeZone[2][0] << "\n";
			rangeStream << "Max: " << pUser->pEUD->builderUser.rangeZone[0][1] << "/" << pUser->pEUD->builderUser.rangeZone[1][1] << "/" << pUser->pEUD->builderUser.rangeZone[2][1] << "\n";
			pUser->pUserSocket->SendSystemMessage(rangeStream.str().c_str());
		}else if(opt == L"off")
		{
			pUser->pEUD->builderUser.isRecordingRange = FALSE;
			pUser->pUserSocket->SendSystemMessage(L"Recording range mode: off");
		}
	}else
		pSocket->SendSystemMessage(L"//record_range [param]  (param: on/off/show)");

	return false;
}

bool BuilderCmd_SetNoblessType(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	INT32 type = 0;
	str >> type;
	if(type == 0 || type == 1)
	{
		User *pTarget = User::GetUser(&pUser->targetId);
		if(pTarget == 0)
			pTarget = pUser;

		if(pTarget->ValidUser())
		{
			g_DB.RequestSaveHeroType(pUser, type);
			g_Log.Add(CLog::Blue, "GM[%S] set nobless type[%d] for user[%S]", pUser->pSD->wName, type, pTarget->pSD->wName);
		}
	}else
		pSocket->SendSystemMessage(L"Invalid nobless type ( can be 0 ~ 1 )!");

	return false;
}

bool BuilderCmd_SetHeroType(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	INT32 type = 0;
	str >> type;
	if(type == 0 || type == 1 || type == 2)
	{
		User *pTarget = User::GetUser(&pUser->targetId);
		if(pTarget == 0)
			pTarget = pUser;

		if(pTarget->ValidUser())
		{
			g_DB.RequestSaveHeroType(pUser, type);
			g_Log.Add(CLog::Blue, "GM[%S] set hero type[%d] for user[%S]", pUser->pSD->wName, type, pTarget->pSD->wName);
		}
	}else
		pSocket->SendSystemMessage(L"Invalid hero type ( can be 0 ~ 2 )!");

	return false;
}

bool BuilderCmd_AddToNpcPos(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstring npcName, territoryName;
	INT32 count = 0, range = 0;

	wstringstream str(wParams);
	if( str >> territoryName >> npcName >> count >> range )
	{
		int classId = g_ObjectDB.GetClassIdFromName(npcName.c_str());
		if(classId > 1000000)
		{
			IVector iPos(pUser->pSD->pos);
			wstring wData = CreateSpawnEntry(territoryName, range, iPos, pUser->pSD->heading, npcName, count);
			AppendToFileW(L"..//Script//NpcPos.txt", wData);
			pSocket->SendSystemMessage(L"AddNpcPos", L"Npc has been added to npcpos.txt");
			g_Log.Add(CLog::Black, "AddToNpcPos: GM[%S] npcName[%S][%d] pos(%d, %d, %d) count(%d) range(%d)", pUser->pSD->wName, npcName.c_str(), classId, iPos.x, iPos.y, iPos.z, count, range);
		}else
		{
			pSocket->SendSystemMessage(L"AddToNpcPos", L"Invalid npc name!");
		}
	}else
	{
		pSocket->SendSystemMessage(L"AddToNpcPos", L"Format: //add_to_npcpos [territory_name] [npc_name] [count] [range]");
	}
	return false;
}

bool BuilderCmd_DeleteItem(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	INT32 itemId = 0, itemCount = 0;
	str >> itemId >> itemCount;
	if(itemId > 0 && itemCount > 0)
	{
		User *pTarget = User::GetUser(&pUser->targetId);
		if(pTarget == 0)
			pTarget = pUser;

		if(pTarget->ValidUser())
		{
			if(pTarget->inventory.HaveItemByClassId(itemId, itemCount))
			{
				pTarget->DeleteItemInInventory(itemId, itemCount);
				g_Log.Add(CLog::Audit, "GM[%S] delete item[%d][%d] user[%S]", pUser->pSD->wName, itemId, itemCount, pTarget->pSD->wName);
				pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"Item(s) have been deleted.");
			}else
				pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"Cannot delete item!");
		}
	}

	return false;
}

bool BuilderCmd_GiveItem(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	INT32 itemId = 0, itemCount = 0;
	str >> itemId >> itemCount;
	if(itemId > 0 && itemCount > 0)
	{
		User *pTarget = User::GetUser(&pUser->targetId);
		if(pTarget == 0)
			pTarget = pUser;

		if(pTarget->ValidUser())
		{
			if(pTarget->inventory.CheckAddable(itemId, itemCount))
			{
				pTarget->AddItemToInventory(itemId, itemCount);
				g_Log.Add(CLog::Audit, "GM[%S] give item[%d][%d] to user[%S]", pUser->pSD->wName, itemId, itemCount, pTarget->pSD->wName);
				pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"Item(s) have been added.");
			}else
				pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"Cannot add item!");
		}
	}

	return false;
}

bool BuilderCmd_SetVotePoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	UINT points = 0;
	str >> points;

	User *pTarget = User::GetUser(&pUser->targetId);
	if(pTarget == 0)
		pTarget = pUser;

	if(pTarget->ValidUser())
	{
		pTarget->pEUD->Lock();
		UINT oldPoints = pTarget->pEUD->votePoints;
		pTarget->pEUD->votePoints = points;
		pTarget->pEUD->Unlock();
		g_DB.RequestSaveVotePoints(pTarget->pUserSocket->accountId, points);
		g_Log.Add(CLog::Audit, "GM[%S] set vote points[%d] to [%d] for user[%S]", pUser->pSD->wName, oldPoints, points, pTarget->pSD->wName);
		pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"Vote points have been changed.");
	}

	return false;
}

bool BuilderCmd_SetTransferredPvPPoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	UINT points = 0;
	str >> points;

	User *pTarget = User::GetUser(&pUser->targetId);
	if(pTarget == 0)
		pTarget = pUser;

	if(pTarget->ValidUser())
	{
		UINT oldPoints = pTarget->pEUD->pvpData.transferredPoints;
		pTarget->pEUD->pvpData.transferredPoints = points;
		g_DB.RequestSaveTransferredPvPPoints(pTarget->databaseId, points);
		g_Log.Add(CLog::Audit, "GM[%S] set transferred PvP points[%d] to [%d] for user[%S]", pUser->pSD->wName, oldPoints, points, pTarget->pSD->wName);
		pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"PvP points have been changed.");
	}

	return false;
}

bool BuilderCmd_SetPvPPoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	UINT points = 0;
	str >> points;

	User *pTarget = User::GetUser(&pUser->targetId);
	if(pTarget == 0)
		pTarget = pUser;

	if(pTarget->ValidUser())
	{
		UINT oldPoints = pTarget->pEUD->pvpData.points;
		pTarget->pEUD->pvpData.points = points;
		g_DB.RequestSavePvPPoints(pTarget->databaseId, points);
		g_Log.Add(CLog::Audit, "GM[%S] set PvP points[%d] to [%d] for user[%S]", pUser->pSD->wName, oldPoints, points, pTarget->pSD->wName);
		pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"PvP points have been changed.");
	}

	return false;
}

bool BuilderCmd_SetDonatePoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams)
{
	wstringstream str(wParams);
	UINT points = 0;
	str >> points;

	User *pTarget = User::GetUser(&pUser->targetId);
	if(pTarget == 0)
		pTarget = pUser;

	if(pTarget->ValidUser())
	{
		pTarget->pEUD->Lock();
		UINT oldPoints = pTarget->pEUD->donatePoints;
		pTarget->pEUD->donatePoints = points;
		pTarget->pEUD->Unlock();
		g_DB.RequestSaveDonatePoints(pTarget->pUserSocket->accountId, points);
		g_Log.Add(CLog::Audit, "GM[%S] set donate points[%d] to [%d] for user[%S]", pUser->pSD->wName, oldPoints, points, pTarget->pSD->wName);
		pUser->pUserSocket->SendSystemMessage(L"BuilderCmd", L"Donate points have been changed.");
	}

	return false;
}