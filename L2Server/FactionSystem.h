#pragma once
#include "BossFaction.h"

struct FactionConfig
{
	FactionType type;
	UINT npcClassId;
	UINT minPlayerCount;
	UINT maxPlayerCount;
	INT32 minPlayerLevel;
	UINT messageId;
	UINT waitTime;
	UINT maxTime;
	BOOL closeColiseum;
	INT32 rewardId;
	INT32 rewardCount;
	IVector waitPosBlue;
	IVector waitPosRed;
	IVector startPosBlue;
	IVector startPosRed;
};

class CFactionSystem : public CFeature
{
	BOOL enabled;
	map<UINT, CFaction*> mFaction;
	list<FactionConfig> lConfig;
	void LoadData();
public:
	CFactionSystem();
	void Init();
	void OnNpcDie(CNpc *pNpc, CCreature *pEnemy);
	void OnEnterWorld(CNpc *pNpc);
	void OnEnterWorld(User *pUser);
	void OnLeaveWorld(User *pUser);
	bool HandleReply(User *pUser, UINT messageId, UINT answer);
	void TimerExpired();
	~CFactionSystem();
};

extern CFactionSystem g_FactionSystem;