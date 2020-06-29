#pragma once
#include "KillReuseDb.h"

enum PVP_TYPE
{
	PT_PK = 0,
	PT_PVP = 1,
	PT_CLAN_WAR = 2,
	PT_KARMA = 3,
	PT_BOTH_KARMA = 4
};

struct PvPSeason
{
	UINT id;
	__time32_t startTime;
	UINT state;
};

class CPvPSystem : public CFeature
{
	BOOL enabled;
	BOOL statistics;
	INT32 requiredLevel;
	INT32 levelGap;
	BOOL checkIPnMAC;
	UINT killReuse;
	BOOL pvpCommand;
	UINT topMonthWinners;
	UINT topMonthRewardId;
	UINT topMonthRewardCount;
	INT32 pvpTokenId;
	UINT pointIncreasedMsgId;

	CKillReuseDb killReuseDb;
	void LoadINI();

	PvPSeason season;
	list<wstring> topPlayers;
	wstring topPlayersHtml;
	INT32 loadTopPlayersTick;
public:
	CPvPSystem();
	~CPvPSystem();
	void Init();
	BOOL IsEnabled();
	BOOL IsStatistics();
	void AddTopPlayer(wstring wName, bool first=false);
	void ReportDieStat(User *pUser, User *pEnemy);
	void OnDie(User *pUser, CCreature *pEnemy);
	bool HandleCmd(User *pUser, const WCHAR *wParam);
	void ConvertTransferredPoints(User *pUser);
	void OnLoadSeason(BOOL initialized, UINT id, UINT state, UINT startTime);
	void OnLoadData(User *pUser, INT32 points, INT32 lastSeasonPoints, INT32 transferredPoints, BOOL topPlayer, BOOL receivedReward);
	void TimerExpired();
	void RequestShowPoints(User *pUser);
	void RequestShowTopPlayers(User *pUser);
};

extern CPvPSystem g_PvPSystem;