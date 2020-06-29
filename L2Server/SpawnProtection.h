#pragma once

class CSpawnProtection : public CFeature
{
	BOOL enabled;
	BOOL affectPK;
	BOOL onEnterWorld;
	UINT activateMsg;
	UINT deactivateMsg;
	INT32 skillId;
	INT32 protectionTimeout;
public:
	CSpawnProtection();
	void Init();
	void Activate(User *pUser);
	void Deactivate(User *pUser);
	BOOL OnEnterWorld();
	~CSpawnProtection();
};

extern CSpawnProtection g_SpawnProtection;