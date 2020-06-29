#pragma once

class CNpcServer
{
	CNpcServer* lpInstance;
public:
	CNpcServer();
	~CNpcServer();
	void Send(const char* format, ...);
	void SpawnNpc(UINT npcClassId, int x, int y, int z);
};

extern CNpcServer g_NpcServer;