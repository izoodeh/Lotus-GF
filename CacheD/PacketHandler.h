#pragma once
#include "Socket.h"
#include "..\\Shared\\CacheDPacket.h"

union UtB
{
	UINT full;
	BYTE part[4];
	UtB() : full(0) {};
};


class CPacketHandler
{
	SocketCallback handlerArray[256];
public:
	CPacketHandler();
	~CPacketHandler();
	void Init();
	bool Bind(UINT id, SocketCallback func);
	static bool Handle(CSocket* pSocket, const unsigned char* packet);
	static bool DummyPacket(CSocket* pSocket, const unsigned char* packet);
	static bool TestPacket(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadSchemeBuffList(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveSchemeBuffList(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadDonatePoints(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveDonatePoints(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadPvPData(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSavePvPPoints(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadVotePoints(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveVotePoints(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSavePvPStat(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadCertificationSkills(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadChampionshipTeams(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadChampionshipMatches(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveChampionshipTeam(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveChampionshipMatch(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveTransferredPvPPoints(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadPvPSeason(CSocket* pSocket, const unsigned char* packet);
	static bool RequestProcessPvPSeason(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSetPvPRewardFlag(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadPvPTopPlayers(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveGMLogin(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveGMLogout(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveHWID(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSetItemLockPassword(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSetItemLockState(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadItemLocks(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadBannedHardwareId(CSocket* pSocket, const unsigned char* packet);
	static bool RequestUnBanHardwareId(CSocket* pSocket, const unsigned char* packet);
	static bool RequestBanHardwareId(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveRealIp(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadAIO(CSocket* pSocket, const unsigned char* packet);
	static bool RequestSaveAIO(CSocket* pSocket, const unsigned char* packet);
};

extern CPacketHandler g_PacketHandler;