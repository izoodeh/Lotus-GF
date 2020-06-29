#pragma once

#define BUILDER_CMD_LIMIT 1000

typedef bool (*BuilderCmdCallback)(CUserSocket*, User*, WCHAR*);

struct BuilderCmdInfo
{
	const WCHAR* lpName;
	INT64 requiredLevel; /* builder level */
	BuilderCmdCallback func;
};

class CBuilderCmd : public CFeature
{
	int count;
	map<wstring, int> mData;	//<name, requiredLevel>
	void LoadData();
	void AddHandler(const WCHAR* wName, INT64 requiredLevel, BuilderCmdCallback func);
public:
	CBuilderCmd();
	~CBuilderCmd();
	void Init();
	void OnLeaveWorld(User *pUser);
};

static bool BuilderCmd_TestCmd(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_AddToNpcPos(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_SetDonatePoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_SetPvPPoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_SetTransferredPvPPoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_SetVotePoints(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_GiveItem(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_DeleteItem(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_SetHeroType(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_SetNoblessType(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_RecordRange(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_BanHWID(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_UnbanHWID(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_ReloadBannedHWID(CUserSocket* pSocket, User* pUser, WCHAR* wParams);
static bool BuilderCmd_Shutdown(CUserSocket *pSocket, User *pUser, WCHAR* wParams);
static bool BuilderCmd_Restart(CUserSocket *pSocket, User *pUser, WCHAR* wParams);
static bool BuilderCmd_ReloadTerritoryData(CUserSocket *pSocket, User *pUser, WCHAR* wParams);
static bool BuilderCmd_SetDominionSiegeState(CUserSocket *pSocket, User *pUser, WCHAR* wParams);
static bool BuilderCmd_CheckCriticalDamage(CUserSocket *pSocket, User *pUser, WCHAR* wParams);

extern CBuilderCmd g_BuilderCmd;