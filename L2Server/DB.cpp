#include "StdAfx.h"
#include "DB.h"

CDB g_DB;

CDB::CDB()
{
	lpInstance = (CDB*)0x149AE80;
}

CDB::~CDB()
{
}

void CDB::Send(const char *format, ...)
{
	va_list va;
	va_start(va, format);
	lpInstance->pSocket[(rand() % 2)]->SendV(format, va);
	va_end(va);
}

void CDB::RequestSendTestPacket()
{
	Send("chd", 0xF9, CI_TEST, GetTickCount());
}

void CDB::SendAcquireSkillToDB(UINT charId, int skillId, int level, bool mayDown)
{
	typedef void (*f)(CDB*, int, int, int, bool);
	f(0x582774L)(lpInstance, charId, skillId, level, mayDown);
}

void CDB::SendDeleteSkill(UINT charId, INT32 skillId)
{
	typedef void(*f)(CDB*, UINT, INT32);
	f(0x582934L)(lpInstance, charId, skillId);
}

void CDB::RequestEnchantItem(CItem *pScroll, CItem *pBonusItem, CItem *pItem, UINT newEnchantLevel, User *pUser, INT32 chance)
{
	typedef void (*f)(CDB*, CItem*, CItem*, CItem*, UINT, User*, INT32);
	f(0x5833B8L)(lpInstance, pScroll, pBonusItem, pItem, newEnchantLevel, pUser, chance);
}

void CDB::RequestEnchantItemFail(CItem *pScroll, CItem *pBonusItem, CItem *pItem, User *pUser, INT32 chance)
{
	typedef void (*f)(CDB*, CItem*, CItem*, CItem*, User*, INT32);
	f(0x5835F4L)(lpInstance, pScroll, pBonusItem, pItem, pUser, chance);
}

void CDB::RequestLoadPledge(UINT pledgeId)
{
	typedef void (*f)(CDB*, UINT);
	f(0x581758L)(lpInstance, pledgeId);
}

void CDB::RequestLoadSchemeBuffList(User *pUser)
{
	if(pUser->ValidUser())
	{
		Send("chdd", 0xF9, CI_REQUEST_LOAD_BUFF_LIST, pUser->databaseId, pUser->pSD->subjob_id);
	}
}

void CDB::RequestSaveSchemeBuffList(User *pUser)
{
	if(pUser->ValidUser())
	{
		vector<UINT> buffs;
		pUser->pEUD->schemeBuffList.GetData(buffs, false);
		
		//user
		char buff[8192];
		int len = 0;
		UINT count = 0;
		SIZE_T limit = buffs.size();
		if(limit > 46)
		{
			limit = 46;
		}
		for(UINT n=0;n<limit;n++)
		{
			if(buffs[n] > 0)
			{
				len += Assemble(&buff[len], ( 8192 - len ), "d", buffs[n]);
				count++;
			}
		}

		if(count > 0)
		{
			Send("chddddb", 0xF9, CI_REQUEST_SAVE_BUFF_LIST, pUser->databaseId, pUser->pSD->subjob_id, 0, count, len, buff);
		}

		//summon
		buffs.clear();
		pUser->pEUD->schemeBuffList.GetData(buffs, true);
		len = 0;
		count = 0;
		limit = buffs.size();
		if(limit > 46)
		{
			limit = 46;
		}
		for(int n=0;n<limit;n++)
		{
			if(buffs[n] > 0)
			{
				len += Assemble(&buff[len], ( 8192 - len ), "d", buffs[n]);
				count++;
			}
		}

		if(count > 0)
		{
			Send("chddddb", 0xF9, CI_REQUEST_SAVE_BUFF_LIST, pUser->databaseId, pUser->pSD->subjob_id, 1, count, len, buff);
		}
	}
}

void CDB::RequestLoadDonatePoints(UINT charId, UINT accountId)
{
	if(charId)
	{
		Send("chdd", 0xF9, CI_REQUEST_LOAD_DONATE_POINTS, charId, accountId);
	}
}

void CDB::RequestLoadPvPData(UINT charId)
{
	if(charId)
	{
		Send("chd", 0xF9, CI_REQUEST_LOAD_PVP_DATA, charId);
	}
}


void CDB::RequestLoadVotePoints(UINT charId, UINT accountId)
{
	if(charId)
	{
		Send("chdd", 0xF9, CI_REQUEST_LOAD_VOTE_POINTS, charId, accountId);
	}
}

void CDB::RequestSaveDonatePoints(UINT accountId, UINT points)
{
	if(accountId)
	{
		Send("chdd", 0xF9, CI_REQUEST_SAVE_DONATE_POINTS, accountId, points);
	}
}

void CDB::RequestSavePvPPoints(UINT charId, UINT points)
{
	if(charId)
	{
		Send("chdd", 0xF9, CI_REQUEST_SAVE_PVP_POINTS, charId, points);
	}
}

void CDB::RequestSaveTransferredPvPPoints(UINT charId, UINT points)
{
	if(charId)
	{
		Send("chdd", 0xF9, CI_REQUEST_SAVE_TRANSFERRED_PVP_POINTS, charId, points);
	}
}

void CDB::RequestSaveVotePoints(UINT accountId, UINT points)
{
	if(accountId)
	{
		Send("chdd", 0xF9, CI_REQUEST_SAVE_VOTE_POINTS, accountId, points);
	}
}

void CDB::RequestSavePvPStat(UINT victimId, INT32 victimLevel, INT32 victimClassId, UINT killerId, INT32 killerClassId, INT32 killerLevel, INT32 killerHp, INT32 killerMaxHp, INT32 killerCp, INT32 killerMaxCp, UINT pvpType, INT32 x, INT32 y, INT32 z, INT32 skillId, INT32 skillLevel, UINT deathTime)
{
	if(victimId && killerId)
	{
		Send("chddddddddddddddddd", 0xF9, CI_REQUEST_SAVE_PVP_STAT, victimId, victimLevel, victimClassId, killerId, killerClassId, killerLevel, killerHp, killerMaxHp, killerCp, killerMaxCp, pvpType, x, y, z, skillId, skillLevel, deathTime);
	}
}

void CDB::RequestLoadCertificationSkills(UINT charId)
{
	if(charId)
	{
		Send("chd", 0xF9, CI_REQUEST_LOAD_CERTIFICATION_SKILLS, charId);
	}
}

void CDB::RequestSaveHeroType(User *pUser, int heroType)
{
	typedef void(*f)(CDB*, User*, int);
	f(0x5991A0L)(lpInstance, pUser, heroType);
}

void CDB::RequestSaveNoblessType(User *pUser, int type)
{
	typedef void(*f)(CDB*, User*, int);
	f(0x5990A8L)(lpInstance, pUser, type);
}

void CDB::SaveQuestInfo(User *pUser)
{
	typedef void (*f)(CDB*, User*);
	f(0x598680L)(lpInstance, pUser);
}

void  CDB::RequestLoadChampionshipTeams()
{
	Send("ch", 0xF9, CI_REQUEST_LOAD_CHAMPIONSHIP_TEAMS);
}


void  CDB::RequestLoadChampionshipMatches()
{
	Send("ch", 0xF9, CI_REQUEST_LOAD_CHAMPIONSHIP_MATCHES);
}

void CDB::RequestSaveChampionshipTeam(UINT id, INT32 points, INT32 winCount, INT32 loseCount, INT32 drawCount, INT32 killCount, INT32 deathCount)
{
	g_Log.Add(CLog::Blue, "[%s] %d, %d, %d, %d, %d, %d, %d", __FUNCTION__, id, points, winCount, loseCount, drawCount, killCount, deathCount);
	Send("chddddddd", 0xF9, CI_REQUEST_SAVE_CHAMPIONSHIP_TEAM, id, points, winCount, loseCount, drawCount, killCount, deathCount);
}

void CDB::RequestSaveChampionshipMatch(UINT id, UINT state, UINT winnerId)
{
	Send("chddd", 0xF9, CI_REQUEST_SAVE_CHAMPIONSHIP_MATCH, id, state, winnerId);
}

void  CDB::RequestLoadPvPSeason()
{
	Send("ch", 0xF9, CI_REQUEST_LOAD_PVP_SEASON);
}

void  CDB::RequestProcessPvPSeason(__time32_t currentTime, UINT topPlayer)
{
	Send("chdd", 0xF9, CI_REQUEST_PROCESS_PVP_SEASON, currentTime, topPlayer);
}

void  CDB::RequestSetPvPRewardFlag(UINT charId)
{
	Send("chdd", 0xF9, CI_REQUEST_SET_PVP_REWARD_FLAG, charId);
}

void  CDB::RequestLoadPvPTopPlayers()
{
	Send("chd", 0xF9, CI_REQUEST_LOAD_PVP_TOP_PLAYERS);
}

void CDB::RequestSaveGMLogin(UINT charId)
{
	Send("chdd", 0xF9, CI_REQUEST_SAVE_GM_LOGIN, charId);
}

void CDB::RequestSaveGMLogout(UINT charId)
{
	Send("chdd", 0xF9, CI_REQUEST_SAVE_GM_LOGOUT, charId);
}

void CDB::RequestSaveHardwareId(UINT charId, LPBYTE lpHardwareId)
{
	Send("chdb", 0xF9, CI_REQUEST_SAVE_HWID, charId, 32, lpHardwareId);
}

void CDB::RequestSaveRealIp(UINT charId, UINT ip)
{
	Send("chdd", 0xF9, CI_REQUEST_SAVE_REAL_IP, charId, ip);
}

void CDB::RequestSetItemLockPassword(UINT charId, const WCHAR *wPassword, UINT lockState)
{
	Send("chdSd", 0xF9, CI_REQUEST_SET_ITEM_LOCK_PASSWORD, charId, wPassword, lockState);
}

void CDB::RequestSetItemLockState(UINT charId, UINT lockState)
{
	Send("chdd", 0xF9, CI_REQUEST_SET_ITEM_LOCK_STATE, charId, lockState);
}

void CDB::RequestLoadItemLocks()
{
	Send("ch", 0xF9, CI_REQUEST_LOAD_ITEM_LOCKS);
}

void CDB::RequestLoadBannedHardwareId()
{
	Send("ch", 0xF9, CI_REQUEST_LOAD_BANNED_HWID);
}

void CDB::RequestUnBanHardwareId(LPBYTE lpHwid)
{
	Send("chb", 0xF9, CI_REQUEST_UNBAN_HWID, 32, lpHwid);
}

void CDB::RequestBanHardwareId(LPBYTE lpHwid, const WCHAR* wAccount, const WCHAR* wGMName, UINT timeout)
{
	Send("chbSSd", 0xF9, CI_REQUEST_BAN_HWID, 32, lpHwid, wAccount, wGMName, timeout);
}

void CDB::RequestLoadAIO(UINT charId)
{
	Send("chd", 0xF9, CI_REQUEST_LOAD_AIO, charId);
}

void CDB::RequestSaveAIO(UINT charId, UINT aioId, UINT startTime)
{
	Send("chddd", 0xF9, CI_REQUEST_SAVE_AIO, charId, aioId, startTime);
}