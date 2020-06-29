#include "stdafx.h"
#include "BossFaction.h"
#include "PCTrap.h"

CBossFaction::CBossFaction()
{
	started = false;
	deleteWaiting = false;
	type = FactionBoss;
}

CBossFaction::~CBossFaction()
{

}

void CBossFaction::Init(UINT classId, UINT serverId, UINT messageId, UINT teleportTime, UINT maxEventTime, IVector waitPos1, IVector waitPos2, IVector startPos1, IVector startPos2, INT32 rewardItemId, INT32 rewardItemCount, BOOL IsCloseColiseum)
{
	SetName(L"Boss Faction");
	this->bossClassId = classId;
	this->bossServerId = serverId;
	this->askMessageId = messageId;
	this->waitTime = teleportTime;
	this->maxTime = maxEventTime;
	this->waitPosBlue = waitPos1;
	this->waitPosRed = waitPos2;
	this->started = false;
	this->startPosBlue = startPos1;
	this->startPosRed = startPos2;
	this->startTime = time(NULL);
	this->rewardId = rewardItemId;
	this->rewardCount = rewardItemCount;
	this->closeColiseum = IsCloseColiseum;
	this->SendAskMessage();
	if(IsCloseColiseum)
	{
		this->CloseColiseum();
	}
}

void CBossFaction::SendAskMessage( User *pUser )
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->level >= this->minPlayerLevel)
		{
			pUser->pUserSocket->Send("cdddd", 0xF3, askMessageId, 0, 18000, 0);
		}
	}else
	{
		char buff[8190];
		int len = Assemble(buff, 8190, "cdddd", 0xF3, askMessageId, 0, 9000, 0);
		BroadcastToAllUser("b", len, buff);
	}
}

void CBossFaction::EnterWorld(User *pUser)
{
	if(pUser->ValidUser())
	{
		if(!this->started && !this->deleteWaiting)
		{
			SendAskMessage(pUser);
		}
	}
}

void CBossFaction::LeaveWorld(User *pUser)
{
	if(pUser->ValidUser())
	{
		this->DelUser(pUser);
	}
}

void CBossFaction::HandleNewUser(User *pUser)
{
	if(pUser->pEUD->faction.team != TeamNone)
	{
		IVector teleportPos = this->waitPosBlue;
		if(pUser->pEUD->faction.team == TeamRed)
			teleportPos = this->waitPosRed;

		if(teleportPos.x != 0 || teleportPos.y != 0 || teleportPos.z != 0)
		{
			pUser->TeleportToLocation(teleportPos.x, teleportPos.y, teleportPos.z);
		}
	}
}

void CBossFaction::TimerExpired()
{
	guard;
	if(!deleteWaiting)
	{
		this->Lock();
		if(!this->started)
		{
			if(this->waitTime > 0)
			{
				if(startPosBlue.x != 0 || startPosBlue.y != 0 || startPosBlue.z != 0)
				{
					if(startPosRed.x != 0 || startPosRed.y != 0 || startPosRed.z != 0)
					{
						time_t currentTime = time(NULL);
						INT64 diff = (this->startTime + this->waitTime) - currentTime;

						if(diff <= 0)
						{
							this->Start();
						}else if(diff == 60)
						{
							CSystemMessage msg(1983);
							msg.AddText(L"Prepare yourself the fight will start in 60 second(s).");
							this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 30)
						{
							CSystemMessage msg(1983);
							msg.AddText(L"Prepare yourself the fight will start in 30 second(s).");
							this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}
						else if(diff == 10)
						{
							CSystemMessage msg(1983);
							msg.AddText(L"Prepare yourself the fight will start in 10 second(s).");
							this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}else if(diff == 5)
						{
							CSystemMessage msg(1983);
							msg.AddText(L"Prepare yourself the fight will start in 5 second(s).");
							this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
						}
					}
				}
			}
		}else
		{
			if((this->maxTime + this->startTime) < time(NULL))
			{
				CSystemMessage msg(1983);
				msg.AddText(L"Boss Faction event reached time limit.");
				this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
				deleteWaiting = true;
			}else if(this->mUsers.size() == 0)
			{
				deleteWaiting  = true;
			}
		}
		this->Unlock();
	}
	unguard;
}

void CBossFaction::Start()
{
	guard;
	this->started = true;
	if(this->mUsers.size() > 0 && this->minPlayerCount <= this->mUsers.size())
	{
		if(startPosBlue.x != 0 || startPosBlue.y != 0 || startPosBlue.z != 0)
		{
			if(startPosRed.x != 0 || startPosRed.y != 0 || startPosRed.z != 0)
			{
				CSystemMessage msg(1983);
				msg.AddText(L"You're gonna be teleported to battle ground.");
				this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());

				for(map<UINT, FactionUser>::iterator Iter = mUsers.begin();Iter!=mUsers.end();Iter++)
				{
					if(User *pUser = Iter->second.pUser->CastUser())
					{
						if(pUser->pEUD->faction.team == TeamBlue)
						{
							pUser->TeleportToLocation(startPosBlue.x, startPosBlue.y, startPosBlue.z);
						}else if(pUser->pEUD->faction.team = TeamRed)
						{
							pUser->TeleportToLocation(startPosRed.x, startPosRed.y, startPosRed.z);
						}
					}
				}
			}
		}
	}else
	{
		CSystemMessage msg(1983);
		msg.AddText(L"There were not enough participants Boss Faction event will be cancelled.");
		this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
		deleteWaiting = true;
	}
	unguard;
}

void CBossFaction::End()
{
	guard;
	if(closeColiseum)
	{
		this->OpenColiseum();
	}
	//check if npc is alive
	CNpc *pNpc = CNpc::GetNpc(&this->bossServerId);
	if(pNpc->ValidCreature())
	{
		if(pNpc->pSD->alive)
		{
			pNpc->KillNpc();
		}
	}

	if(winnerTeam != TeamNone)
	{
		if(winnerTeam == TeamBlue)
		{
			CSystemMessage msg(1983);
			msg.AddText(L"Congratulations! The Blue Team won the fight!");
			this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
		}else if(winnerTeam == TeamRed)
		{
			CSystemMessage msg(1983);
			msg.AddText(L"Congratulations! The Blue Team won the fight!");
			this->Broadcast(TeamNone, "b", msg.GetSize(), msg.GetBuff());
		}
		if(rewardId > 0 && rewardCount > 0)
		{
			for(map<UINT, FactionUser>::iterator Iter = mUsers.begin();Iter!=mUsers.end();Iter++)
			{
				if(User *pUser = Iter->second.pUser->CastUser())
				{
					if(pUser->pEUD->faction.team == winnerTeam)
					{
						if(pUser->inventory.CheckAddable(rewardId, rewardCount))
						{
							pUser->AddItemToInventory(rewardId, rewardCount);
						}
					}
				}
			}
		}
	}
	for(map<UINT, FactionUser>::iterator Iter = mUsers.begin();Iter!=mUsers.end();Iter++)
	{
		if(User *pUser = Iter->second.pUser->CastUser())
		{
			pUser->pEUD->faction.factionId = 0;
			pUser->pEUD->faction.team = TeamNone;
			pUser->SendCharInfo();
			pUser->SendUserInfo();
		}
	}
	mUsers.clear();
	unguard;
}

void CBossFaction::OnNpcDie(CNpc *pNpc, CCreature *pEnemy)
{
	guard;
	if(!this->deleteWaiting)
	{
		if(pNpc->ValidCreature() && pEnemy->ValidCreature())
		{
			if(pNpc->pSD->npc_class_id == this->bossClassId && !this->deleteWaiting)
			{
				User *pKiller = 0;
				if(pEnemy->IsUser())
				{
					pKiller = pEnemy->CastUser();
				}else if(pEnemy->IsPCTrap())
				{
					pKiller = pEnemy->CastPCTrap()->GetUserOrMaster();
				}else if(pEnemy->IsSummon())
				{
					pKiller = pEnemy->CastSummon()->GetMaster();
				}else if(pEnemy->IsPet())
				{
					pKiller = pEnemy->CastPet()->GetMaster();
				}
				if(pKiller->ValidUser())
				{
					if(pKiller->pEUD->faction.factionId == factionId)
					{
						winnerTeam = pKiller->pEUD->faction.team;
					}
				}
				this->deleteWaiting = true;
			}
		}
	}
	unguard;
}