#pragma once

#define HTML_CMD_LIMIT 255

typedef bool (*HtmlCmdCallback)(CUserSocket *pSocket, User *pUser, const WCHAR *wParams, const WCHAR* wBypass);

struct HtmlCmdInfo
{
	WCHAR wName[64];
	size_t len;
	HtmlCmdCallback func;
};

class CHtmlCmd : public CFeature
{
	int handlerCount;
public:
	CHtmlCmd();
	~CHtmlCmd();
	void Init();
	void AddHandler(const WCHAR* wName, HtmlCmdCallback func);
	bool Handle(CUserSocket *pSocket, User *pUser, WCHAR* wBypass, const unsigned char* packet);
	static bool UseValidTalkTarget(User *pUser);
	bool IsSchemeBufferBypass(WCHAR* wBypass);
};
extern CHtmlCmd g_HtmlCmd;

static bool HtmlCmdItemLock(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR* wBypass);
static bool HtmlCmdBufferDispelAll(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdBufferSummonDispelAll(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdBufferHealthy(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdBufferSummonHealthy(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdBuffPlayer(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdBuffSummon(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdAddBuff(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdAddSummonBuff(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdUseBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdUseSummonBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdClearBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdClearSummonBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdDelevelPlayer(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdTvTRegister(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdTvTUnregister(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);

static bool HtmlCmdOfflineShop(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);

static bool HtmlCmdPvPPoints(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdPvPConvertPoints(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdPvPTopPlayers(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);

static bool HtmlCmdDungeonsEnter(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdExpMode(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);

static bool HtmlCmdClassManagerShow(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdClassManagerChange(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdClassManagerNobless(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdClassManagerSubClass(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdAutoLoot(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdSetLevel(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdTeleportPlayer(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
static bool HtmlCmdAcquireAiO(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass);
