#pragma once
#include "Faction.h"

class CBossFaction : public CFaction
{
	friend class CFactionSystem;
	IVector waitPosBlue;
	IVector waitPosRed;
	IVector startPosBlue;
	IVector startPosRed;
	UINT waitTime;
	UINT maxTime;
	UINT bossClassId;
	UINT bossServerId;
	INT32 rewardId;
	INT32 rewardCount;
	time_t startTime;
	void SendAskMessage(User *pUser = 0);
public:
	CBossFaction();
	~CBossFaction();
	void Init(UINT classId, UINT serverId, UINT messageId, UINT teleportTime, UINT maxEventTime, IVector waitPos1, IVector waitPos2, IVector startPos1, IVector startPos2, INT32 rewardItemId, INT32 rewardItemCount, BOOL IsCloseColiseum);
	void HandleNewUser(User *pUser);
	void EnterWorld(User *pUser);
	void LeaveWorld(User *pUser);
	void OnNpcDie(CNpc *pNpc, CCreature *pEnemy);
	void Start();
	void TimerExpired();
	void End();
};