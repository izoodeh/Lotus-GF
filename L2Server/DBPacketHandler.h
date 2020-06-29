#pragma once
#include "DBSocket.h"
#include "..\\Shared\\CacheDPacket.h"

typedef bool (*DBCallback)(CDBSocket*, const unsigned char*);

class CDBPacketHandler : public CFeature
{
	DBCallback handlerArray[256];
public:
	CDBPacketHandler();
	~CDBPacketHandler();
	void Init();
	bool Bind(UINT id, DBCallback func);
	static bool Handle(CDBSocket* pSocket, const unsigned char* packet);
	static bool DummyPacket(CDBSocket* pSocket, const unsigned char* packet);
	static bool TestPacket(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadSchemeBuffList(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadDonatePoints(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadPvPData(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadPvPSeason(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadVotePoints(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadCertificationSkills(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadChampionshipTeam(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadChampionshipMatch(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadPvPTopPlayers(CDBSocket* pSocket, const unsigned char* packet);
	static bool ReplyLoadItemLock(CDBSocket *pSocket, const unsigned char* packet);
	static bool ReplyLoadBannedHardwareId(CDBSocket *pSocket, const unsigned char* packet);
	static bool ReplyLoadAIO(CDBSocket *pSocket, const unsigned char* packet);
};

extern CDBPacketHandler g_DBPacketHandler;