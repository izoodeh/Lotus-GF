#include "stdafx.h"
#include "NpcAction.h"
#include "HookManager.h"
#include "NpcMod.h"
#include "EventName.h"
#include "FactionSystem.h"
#include "TvT.h"
#include "AutoLoot.h"
#include "UserDB.h"
#include "AllInOne.h"
#include "SPointer.h"

extern BOOL g_CustomTraitSystem;

//This is also used in i_collecting!
CCreature* g_NpcOnDieEnemy[16];

void NpcAction::Init()
{
	WriteCall(0x7B772F, NpcAction::Die);
	WriteCall(0x880BA7, NpcAction::Die);

	WriteToVMT(VT_CBOSS, 0x890, NpcAction::Die);
	WriteToVMT(VT_CCOLLECTION, 0x890, NpcAction::Die);
	WriteToVMT(VT_CDOPPELGANGER, 0x890, NpcAction::Die);
	WriteToVMT(VT_CMERCHANT, 0x890, NpcAction::Die);
	WriteToVMT(VT_CNPC, 0x890, NpcAction::Die);
	WriteToVMT(VT_CTRAP, 0x890, NpcAction::Die);
	WriteToVMT(VT_CPCTRAP, 0x890, NpcAction::Die);
	WriteToVMT(VT_CZZOLDAGU, 0x890, NpcAction::Die);

	WriteToVMT(VT_CBOSS, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CCOLLECTION, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CDOPPELGANGER, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CMERCHANT, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CNPC, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CTRAP, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CPCTRAP, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CWORLD_TRAP, 0x20, NpcAction::TimerExpired);
	WriteToVMT(VT_CZZOLDAGU, 0x20, NpcAction::TimerExpired);

	WriteCall(0x7B4804, NpcAction::TimerExpired);	//CSummon::TimerExpired

	WriteCall(0x8804D3, NpcAction::EnterWorld);
	WriteCall(0x883557, NpcAction::EnterWorld);

	WriteToVMT(VT_CBOSS, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CCOLLECTION, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CDOPPELGANGER, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CMERCHANT, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CNPC, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CSUMMON, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CPET, 0xAC0, NpcAction::EnterWorld);
	WriteToVMT(VT_CTRAP, 0xAC0, NpcAction::EnterWorld);	
	WriteToVMT(VT_CZZOLDAGU, 0xAC0, NpcAction::EnterWorld);

	WriteCall(0x71FB7D, NpcAction::OnAddItemToInventoryDrop);
	WriteDWORD(0xCF9F90, static_cast<UINT>(reinterpret_cast<UINT64>(NpcAction::OnICollecting)));

	WriteRelative(0xA27AE3L, NpcAction::OnNpcCreateSubJob);
	WriteRelative(0xA27B0BL, NpcAction::OnNpcChangeSubJob);


}

bool NpcAction::OnNpcChangeSubJob(PVOID pSocket, const unsigned char *packet)
{
	CTL;

	UINT npcId = 0, creatureIndex = 0, subjobId = 0;
	Disassemble(packet, "ddd", &npcId, &creatureIndex, &subjobId);

	CCreatureSP creatureSp;
	CCreature::GetCreature(creatureSp, creatureIndex);

	if(User *pUser = creatureSp.get()->CastUser())
	{
		if(g_AllInOne.Enabled() && !g_AllInOne.CanUseSubclass())
		{
			if(pUser->pEUD->aioUser.aioId > 0 && pUser->pSD->subjob_id == 0)
			{
				pUser->pUserSocket->SendSystemMessage(L"The AiO characters are not allowed to use a subclass!");
				return false;
			}
		}
	}

	typedef bool (*f)(PVOID, const unsigned char*);
	return f(0x740230L)(pSocket, packet);
}

bool NpcAction::OnNpcCreateSubJob(PVOID pSocket, const unsigned char *packet)
{
	CTL;

	UINT npcId = 0, creatureIndex = 0, subjobId = 0;
	Disassemble(packet, "ddd", &npcId, &creatureIndex, &subjobId);

	CCreatureSP creatureSp;
	CCreature::GetCreature(creatureSp, creatureIndex);

	if(User *pUser = creatureSp.get()->CastUser())
	{
		if(g_AllInOne.Enabled() && !g_AllInOne.CanUseSubclass())
		{
			if(pUser->pEUD->aioUser.aioId > 0)
			{
				pUser->pUserSocket->SendSystemMessage(L"The AiO characters are not allowed to use a subclass!");
				return false;
			}
		}
	}

	typedef bool (*f)(PVOID, const unsigned char*);
	return f(0x740048L)(pSocket, packet);
}

void NpcAction::OnAddItemToInventoryDrop(CNpc *pNpc, INT32 itemId, INT64 amount, UINT reason, INT32 enchanted, INT32 blessed, INT32 eroded, ItemOptionKey* pOption, int param5, BaseAttribute* pAttribute)
{
	guard;

	if(g_AutoLoot.IsEnabled())
	{
		UINT threadIndex = GetThreadIndex();
		if(threadIndex < 16)
		{
			CCreature *pEnemy = g_NpcOnDieEnemy[threadIndex];
			if(pEnemy->ValidCreature())
			{
				if(g_AutoLoot.Handle(pNpc, pEnemy, itemId, amount))
				{
					unguard;
					return;
				}
			}
		}
	}

	pNpc->AddItemToInventory(itemId, amount, reason > 0 ? true : false, enchanted, blessed, eroded, pOption, param5, pAttribute);
	unguard;
}

bool NpcAction::OnICollecting(CSkillEffect *pEffect, CCreature *pCreature, CObject *pTarget, CSkillInfo *pSI, CSkillAction2 *pAction, double distance)
{
	CTL;
	bool ret = false;

	UINT threadIndex = GetThreadIndex();
	if(threadIndex < 16)
	{
		g_NpcOnDieEnemy[threadIndex] = pCreature;
	}

	typedef bool (*f)(CSkillEffect*, CCreature*, CObject*, CSkillInfo*, CSkillAction2*, double);
	ret = f(0x84D7B8L)(pEffect, pCreature, pTarget, pSI, pAction, distance);

	if(threadIndex < 16)
	{
		g_NpcOnDieEnemy[threadIndex] = 0;
	}

	return ret;
}

void NpcAction::EnterWorld(CNpc* pNpc, bool restoreStat, int hp, int mp, UINT serverId)
{
	if(pNpc->ValidCreature())
	{
		g_FactionSystem.OnEnterWorld(pNpc);
		g_TvT.OnNpcSpawn(pNpc);
	}
	typedef void (*f)(CNpc*, bool, int, int, UINT);
	f(0x728EF4)(pNpc, restoreStat, hp, mp, serverId);
}

void NpcAction::TimerExpired(CNpc *pNpc, int id)
{
	guard;

	typedef void (*f)(CNpc*, int);
	f(0x72281CL)(pNpc, id);

	if(id==0)
	{
		if(g_CustomTraitSystem && pNpc->pECD)
		{
			if(pNpc->IsPet() == false)
			{
				if(pNpc->pSD->alive == 0)
				{
					if(pNpc->pECD->leftWorld == FALSE && pNpc->pECD->deathTime != 0)
					{
						if((pNpc->pECD->deathTime + 30) < time(0))
						{
							g_Log.Add(CLog::Blue, "[%s] Npc[%S] leaving world - death time[%d] current[%d] pos(%f, %f, %f)", __FUNCTION__, pNpc->pSD->wName, pNpc->pECD->deathTime, time(0), pNpc->pSD->pos.x, pNpc->pSD->pos.y, pNpc->pSD->pos.z);
							pNpc->pECD->leftWorld = TRUE;
							pNpc->LeaveWorld();
						}
					}
				}
			}
		}
	}
	unguard;
}

bool NpcAction::Die(CNpc *pNpc, CCreature *pEnemy)
{
	guard;

	UINT threadIndex = GetThreadIndex();
	if(threadIndex < 16)
	{
		g_NpcOnDieEnemy[threadIndex] = pEnemy;
	}

	g_NpcMod.OnDie(pNpc, pEnemy);
	g_EventName.OnNpcDie(pNpc);
	if(!pNpc->killedByGM)
	{
		g_FactionSystem.OnNpcDie(pNpc, pEnemy);
	}

	typedef bool (*f)(CNpc*, CCreature*);
	if(f(0x72972CL)(pNpc, pEnemy))
	{
		if(g_CustomTraitSystem && pNpc->pECD)
		{
			pNpc->pECD->deathTime = static_cast<UINT>(time(0));
		}

		if(threadIndex < 16)
		{
			g_NpcOnDieEnemy[threadIndex] = 0;
		}

		unguard;
		return true;
	}

	if(threadIndex < 16)
	{
		g_NpcOnDieEnemy[threadIndex] = 0;
	}

	unguard;
	return false;
}