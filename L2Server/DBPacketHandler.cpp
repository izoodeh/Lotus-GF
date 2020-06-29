#include "StdAfx.h"
#include "DBPacketHandler.h"
#include "UserDb.h"
#include "SkillAcquireDB.h"
#include "Championship.h"
#include "PvPSystem.h"
#include "ItemLock.h"
#include "CliExt.h"
#include "AllInOne.h"

extern BOOL g_CertificationSkillsOnSubJob;

CDBPacketHandler g_DBPacketHandler;

CDBPacketHandler::CDBPacketHandler()
{
	SetName(L"DB Packet Handler");
	memset(handlerArray, 0, sizeof(handlerArray));
}

CDBPacketHandler::~CDBPacketHandler()
{

}

void CDBPacketHandler::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());

	typedef bool (*orgBind)(INT64, UINT, DBCallback);
	orgBind(0x5B3114L)(0x1633880, 0x1E, CDBPacketHandler::Handle);

	for(UINT n=0;n<256;n++)
	{
		Bind(n, CDBPacketHandler::DummyPacket);
	}

	Bind(CO_TEST, CDBPacketHandler::TestPacket);
	Bind(CO_REPLY_LOAD_BUFF_LIST, CDBPacketHandler::ReplyLoadSchemeBuffList);
	Bind(CO_REPLY_LOAD_DONATE_POINTS, CDBPacketHandler::ReplyLoadDonatePoints);
	Bind(CO_REPLY_LOAD_PVP_DATA, CDBPacketHandler::ReplyLoadPvPData);
	Bind(CO_REPLY_LOAD_VOTE_POINTS, CDBPacketHandler::ReplyLoadVotePoints);
	Bind(CO_REPLY_LOAD_CERTIFICATION_SKILLS, CDBPacketHandler::ReplyLoadCertificationSkills);
	Bind(CO_REPLY_LOAD_CHAMPIONSHIP_TEAM, CDBPacketHandler::ReplyLoadChampionshipTeam);
	Bind(CO_REPLY_LOAD_CHAMPIONSHIP_MATCH, CDBPacketHandler::ReplyLoadChampionshipMatch);
	Bind(CO_REPLY_LOAD_PVP_SEASON, CDBPacketHandler::ReplyLoadPvPSeason);
	Bind(CO_REPLY_LOAD_PVP_TOP_PLAYERS, CDBPacketHandler::ReplyLoadPvPTopPlayers);
	Bind(CO_REPLY_LOAD_ITEM_LOCK, CDBPacketHandler::ReplyLoadItemLock);
	Bind(CO_REPLY_LOAD_BANNED_HWID, CDBPacketHandler::ReplyLoadBannedHardwareId);
	Bind(CO_REPLY_LOAD_AIO, CDBPacketHandler::ReplyLoadAIO);
}

bool CDBPacketHandler::Bind(UINT id, DBCallback func)
{
	if(id < 256)
	{
		handlerArray[id] = func;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Tried to add callback with too high id[%d]!", GetName(), id);
	}
	return false;
}

bool CDBPacketHandler::Handle(CDBSocket *pSocket, const unsigned char *packet)
{
	WORD opCode = 0;
	packet = Disassemble(packet, "h", &opCode);

	if(opCode < 256)
	{
		return g_DBPacketHandler.handlerArray[opCode](pSocket, packet);
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Invalid opCode[%x]", g_DBPacketHandler.GetName(), opCode);
	}

	return false;
}

bool CDBPacketHandler::DummyPacket(CDBSocket *pSocket, const unsigned char *packet)
{
	g_Log.Add(CLog::Error, "[%S] Dummy packet called with opCode[%x]", g_DBPacketHandler.GetName(), packet[-1]);
	return false;
}

bool CDBPacketHandler::TestPacket(CDBSocket *pSocket, const unsigned char *packet)
{
	DWORD value = 0;
	Disassemble(packet, "d", &value);

	g_Log.Add(CLog::Blue, "[%S] received test packet value[%d]", g_DBPacketHandler.GetName(), value);
	return false;
}

bool CDBPacketHandler::ReplyLoadAIO(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, aioId = 0, startTime = 0;
	if(g_AllInOne.Enabled())
	{
		Disassemble(packet, "ddd", &charId, &aioId, &startTime);

		if(User *pUser = g_UserDb.FindByDatabaseId(charId)->CastUser())
		{
			g_AllInOne.OnDBLoad(pUser, aioId, startTime);
		}
	}
	return false;
}

bool CDBPacketHandler::ReplyLoadBannedHardwareId(CDBSocket *pSocket, const unsigned char* packet)
{
	guard;
	BYTE hwid[32];
	WCHAR account[25] = { 0 };
	WCHAR gm[25] = { 0 };
	UINT timeout = 0;
	for(UINT n=0;n<32;n++)
	{
		hwid[n] = (*packet);
		packet++;
	}

	Disassemble(packet, "SSd", sizeof(account), account, sizeof(gm), gm, &timeout);

	g_CliExt.OnBanDBLoad(hwid, account, gm, timeout);

	unguard;
	return false;
}

bool CDBPacketHandler::ReplyLoadItemLock(CDBSocket *pSocket, const unsigned char* packet)
{
	guard;

	UINT charId = 0, lockState = 0;
	WCHAR wPassword[32];
	wPassword[0] = 0;

	Disassemble(packet, "dSd", &charId, sizeof(wPassword), wPassword, &lockState);
	if(charId > 0)
	{
		g_ItemLock.DBLoad(charId, wPassword, lockState);
	}
	unguard;
	return false;
}

bool CDBPacketHandler::ReplyLoadPvPTopPlayers(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT count  = 0;
	packet = Disassemble(packet, "d", &count);

	for(UINT n = 0; n < count; n++)
	{
		WCHAR wName[25];
		packet = Disassemble(packet, "S", sizeof(wName), wName);
		g_PvPSystem.AddTopPlayer(wName, !n);
	}

	return false;
}

bool CDBPacketHandler::ReplyLoadPvPSeason(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT id = 0, state = 0, startTime = 0;
	BOOL initialized = FALSE;
	Disassemble(packet, "dddd", &initialized, &id, &state, &startTime);
	g_PvPSystem.OnLoadSeason(initialized, id, state, startTime);
	return false;
}

bool CDBPacketHandler::ReplyLoadChampionshipMatch(CDBSocket *pSocket, const unsigned char *packet)
{
	//ddddddd", 0x1E, CO_REPLY_LOAD_CHAMPIONSHIP_MATCH, matchCount, id, state, startTime, teamId1, teamId2, winnerId);
	UINT matchCount = 0, state = 0, startTime = 0, id = 0, teamId1, teamId2, winnerId = 0;
	Disassemble(packet, "ddddddd", &matchCount, &id, &state, &startTime, &teamId1, &teamId2, &winnerId);
	if(matchCount > 0)
	{
		g_Championship.AddMatch(id, state, startTime, teamId1, teamId2, winnerId, matchCount);
	}
	return false;
}

bool CDBPacketHandler::ReplyLoadChampionshipTeam(CDBSocket *pSocket, const unsigned char *packet)
{
	//("chddSddddddddd", 0x1E, CO_REPLY_LOAD_CHAMPIONSHIP_TEAM, teamCount, id, wName, charId1, charId2, charId3, points, winCount, loseCount, drawCount, killCount, deathCount);
	UINT teamCount = 0, id = 0;
	WCHAR wName[25];
	INT32 charId1, charId2, charId3, points, winCount, loseCount, drawCount, killCount, deathCount;
	Disassemble(packet, "ddSddddddddd", &teamCount, &id, sizeof(wName), wName, &charId1, &charId2, &charId3, &points, &winCount, &loseCount, &drawCount, &killCount, &deathCount);
	if(teamCount > 0)
	{
		g_Championship.AddTeam(id, wName, charId1, charId2, charId3, points, winCount, loseCount, drawCount, killCount, deathCount, teamCount);
	}
	return false;
}

bool CDBPacketHandler::ReplyLoadCertificationSkills(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT databaseId = 0, count = 0;
	packet = Disassemble(packet, "dd", &databaseId, &count);

	if(databaseId > 0 && count > 0 && g_CertificationSkillsOnSubJob)
	{
		User *pUser = g_UserDb.FindByDatabaseId(databaseId);
		if(pUser->ValidUser())
		{
			if(count < 500)
			{
				vector<SkillAcquireInfo*> vData = g_SkillAcquireDB.GetAcquirableEtcSkills(EtcSkillAcquireSubJob);
				if(vData.size() > 0 )
				{
					for(UINT n=0;n<count;n++)
					{
						int skillId = 0, level = 0;
						packet = Disassemble(packet, "dd", &skillId, &level);

						for(vector<SkillAcquireInfo*>::iterator Iter = vData.begin();Iter!= vData.end();Iter++)
						{
							SkillAcquireInfo* pInfo = (*Iter);
							if(pInfo)
							{
								if(pInfo->skillId == skillId)
								{
									pUser->AcquireSkill(skillId, level);
									break;
								}
							}
						}
					}
					pUser->ValidateParameters();
				}
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid user[%d]!", __FUNCTION__, databaseId);
		}
	}

	return false;
}

bool CDBPacketHandler::ReplyLoadSchemeBuffList(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT databaseId = 0, subjobId = 0, summon = 0, count = 0;
	packet = Disassemble(packet, "dddd", &databaseId, &subjobId, &summon, &count);

	if(databaseId > 0)
	{
		User *pUser = g_UserDb.FindByDatabaseId(databaseId);
		if(pUser->ValidUser())
		{
			pUser->lpYieldLock->Enter(__FILEW__, __LINE__);

			for(UINT n=0;n<count;n++)
			{
				UINT skillClassId = 0;
				packet = Disassemble(packet, "d", &skillClassId);
				if(summon)
				{
					pUser->pEUD->schemeBuffList.Add(skillClassId, true);
				}else
				{
					pUser->pEUD->schemeBuffList.Add(skillClassId, false);
				}
			}

			pUser->lpYieldLock->Leave();
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid user[%d]!", __FUNCTION__, databaseId);
		}
	}

	return false;
}

bool CDBPacketHandler::ReplyLoadDonatePoints(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, points = 0;
	Disassemble(packet, "dd", &charId, &points);
	if(charId > 0)
	{
		User *pUser = g_UserDb.FindByDatabaseId(charId);
		if(pUser->ValidUser())
		{
			pUser->pEUD->Lock();
			pUser->pEUD->donatePoints = points;
			pUser->pEUD->Unlock();
		}
	}
	return false;
}

bool CDBPacketHandler::ReplyLoadPvPData(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0;
	INT32 points = 0, lastSeasonPoints = 0, transferredPoints = 0;
	BOOL topPlayer = 0, receivedReward = 0;
	Disassemble(packet, "dddddd", &charId, &points, &lastSeasonPoints, &transferredPoints, &topPlayer, &receivedReward);
	if(charId > 0)
	{
		User *pUser = g_UserDb.FindByDatabaseId(charId);
		if(pUser->ValidUser())
		{
			g_PvPSystem.OnLoadData(pUser, points, lastSeasonPoints, transferredPoints, topPlayer, receivedReward);
		}
	}
	return false;
}

bool CDBPacketHandler::ReplyLoadVotePoints(CDBSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, points = 0;
	Disassemble(packet, "dd", &charId, &points);
	if(charId > 0)
	{
		User *pUser = g_UserDb.FindByDatabaseId(charId);
		if(pUser->ValidUser())
		{
			pUser->pEUD->Lock();
			pUser->pEUD->votePoints = points;
			pUser->pEUD->Unlock();
		}
	}
	return false;
}