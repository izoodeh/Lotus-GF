#pragma once
#include "DBSocket.h"
#include "..\\Shared\\CacheDPacket.h"

class CDB
{
	/* 00 */ CDB* lpInstance;	//vTable in real instance
	/* 08 */ LPVOID _unkn08[4];
	/* 28 */ CDBSocket* pSocket[2];

public:
	CDB();
	void Send(const char* format, ...);
	void RequestSendTestPacket();
	void RequestEnchantItem(CItem* pScroll, CItem* pBonusItem, CItem* pItem, UINT newEnchantLevel, User *pUser, INT32 chance);
	void RequestEnchantItemFail(CItem *pScroll, CItem *pBonusItem, CItem *pItem, User *pUser, INT32 chance);
	void RequestLoadPledge(UINT pledgeId);
	void RequestLoadSchemeBuffList(User *pUser);
	void RequestSaveSchemeBuffList(User *pUser);
	void RequestLoadVotePoints(UINT charId, UINT accountId);
	void RequestLoadDonatePoints(UINT charId, UINT accountId);
	void RequestLoadPvPData(UINT charId);
	void RequestLoadCertificationSkills(UINT charId);
	void RequestSaveVotePoints(UINT accountId, UINT points);
	void RequestSaveDonatePoints(UINT accountId, UINT points);
	void RequestSavePvPPoints(UINT charId, UINT points);
	void RequestSaveTransferredPvPPoints(UINT charId, UINT points);
	void SendAcquireSkillToDB(UINT charId, int skillId, int level, bool mayDown);
	void SendDeleteSkill(UINT charId, INT32 skillId);
	void RequestSavePvPStat(UINT victimId, INT32 victimLevel, INT32 victimClassId, UINT killerId, INT32 killerClassId, INT32 killerLevel, INT32 killerHp, INT32 killerMaxHp, INT32 killerCp, INT32 killerMaxCp, UINT pvpType, INT32 x, INT32 y, INT32 z, INT32 skillId, INT32 skillLevel, UINT deathTime);
	void RequestSaveHeroType(User *pUser, int heroType);
	void RequestSaveNoblessType(User *pUser, int type);
	void SaveQuestInfo(User *pUser);
	void RequestLoadChampionshipTeams();
	void RequestLoadChampionshipMatches();
	void RequestSaveChampionshipTeam(UINT id, INT32 points, INT32 winCount, INT32 loseCount, INT32 drawCount, INT32 killCount, INT32 deathCount);
	void RequestSaveChampionshipMatch(UINT id, UINT state, UINT winnerId);
	void RequestLoadPvPSeason();
	void RequestProcessPvPSeason(__time32_t currentTime, UINT topPlayer);
	void RequestSetPvPRewardFlag(UINT charId);
	void RequestLoadPvPTopPlayers();
	void RequestSaveGMLogin(UINT charId);
	void RequestSaveGMLogout(UINT charId);
	void RequestSaveHardwareId(UINT charId, LPBYTE lpHardwareId);
	void RequestSaveRealIp(UINT charId, UINT ip);
	void RequestSetItemLockPassword(UINT charId, const WCHAR* wPassword, UINT lockState);
	void RequestSetItemLockState(UINT charId, UINT lockState);
	void RequestLoadItemLocks();
	void RequestLoadBannedHardwareId();
	void RequestUnBanHardwareId(LPBYTE lpHwid);
	void RequestBanHardwareId(LPBYTE lpHwid, const WCHAR* wAccount, const WCHAR* wGMName, UINT timeout);
	void RequestLoadAIO(UINT charId);
	void RequestSaveAIO(UINT charId, UINT aioId, UINT startTime);
	~CDB();
};

extern CDB g_DB;