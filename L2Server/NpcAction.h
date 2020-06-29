#pragma once

class NpcAction
{
public:
	static void Init();
	static bool Die(CNpc *pNpc, CCreature *pEnemy);
	static void TimerExpired(CNpc *pNpc, int id);
	static void EnterWorld(CNpc* pNpc, bool restoreStat, int hp, int mp, UINT serverId);
	static void OnAddItemToInventoryDrop(CNpc *pNpc, INT32 itemId, INT64 amount, UINT reason, INT32 enchanted, INT32 blessed, INT32 eroded, ItemOptionKey* pOption, int param5, BaseAttribute* pAttribute);
	static bool OnICollecting(CSkillEffect *pEffect, CCreature *pCreature, CObject *pTarget, CSkillInfo *pSI, CSkillAction2 *pAction, double distance);
	static bool OnNpcCreateSubJob(PVOID pSocket, const unsigned char *packet);
	static bool OnNpcChangeSubJob(PVOID pSocket, const unsigned char *packet);
};