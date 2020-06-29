#include "stdafx.h"
#include "Faction.h"
#include "UserDb.h"
#include "Door.h"

CFaction::CFaction() : minPlayerLevel(0), maxPlayerCount(UINT_MAX), closeColiseum(false), minPlayerCount(2), winnerTeam(TeamNone), factionId(0), type(FactionNone), askMessageId(0), deleteWaiting(false)
{
	InitializeCriticalSection(&_lock);
	static UINT uniqueId = 1;
	factionId = uniqueId;
	started = false;
	uniqueId++;
}

CFaction::~CFaction()
{
	DeleteCriticalSection(&_lock);
}

CBossFaction* CFaction::CastBossFaction()
{
	if(type == FactionBoss)
	{
		return (CBossFaction*)this;
	}
	return 0;
}

void CFaction::SetName(std::wstring wName)
{
	factionName = wName;
}

wstring CFaction::GetName()
{
	return factionName;
}

UINT CFaction::GetId()
{
	return factionId;
}

FactionType CFaction::GetType()
{
	return type;
}

void CFaction::SetWinner(TeamType team)
{
	winnerTeam = team;
}

bool CFaction::AddUser(User *pUser, TeamType team)
{
	if(pUser->ValidUser())
	{
		bool update = false;
		Lock();
		if(mUsers.size() < maxPlayerCount)
		{
			if(team == TeamNone)
			{
				UINT blueCount = 0;
				UINT redCount = 0;
				for(map<UINT, FactionUser>::iterator Iter = mUsers.begin();Iter!=mUsers.end();Iter++)
				{
					if(Iter->second.team == TeamBlue)
					{
						blueCount++;
					}else if(Iter->second.team == TeamRed)
					{
						redCount++;
					}
				}
				if(blueCount <= redCount)
				{
					team = TeamBlue;
				}
				else
				{
					team = TeamRed;
				}
			}

			FactionUser fu;
			fu.pUser = pUser;
			fu.team = team;
			mUsers.insert(pair<UINT, FactionUser>(pUser->serverId, fu));
			pUser->pEUD->faction.factionId = factionId;
			pUser->pEUD->faction.team = team;
			update = true;
			if(team == TeamRed)
			{
				pUser->pUserSocket->SendSystemMessage(factionName.c_str(), L"You've been added to the red team.");
			}else if (team == TeamBlue)
			{
				pUser->pUserSocket->SendSystemMessage(factionName.c_str(), L"You've been added to the blue team.");
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(factionName.c_str(), L"Faction reached member limit and you cannot participe.");
		}
		Unlock();
		if(update)
		{
			pUser->SendUserInfo();
			pUser->SendCharInfo();
			return true;
		}
	}

	return false;
}

void CFaction::DelUser(User *pUser)
{
	if(pUser->ValidUser())
	{
		bool update = false;
		Lock();
		map<UINT, FactionUser>::iterator Iter = mUsers.find(pUser->serverId);
		if(Iter!=mUsers.end())
		{
			mUsers.erase(Iter);
			pUser->pEUD->faction.factionId = 0;
			pUser->pEUD->faction.team = TeamNone;
			update = true;
		}
		Unlock();
		if(update)
		{
			pUser->SendUserInfo();
			pUser->SendCharInfo();			
		}
	}
}

void CFaction::Broadcast(TeamType team, const char *format, ...)
{
	char buff[16382];
	va_list va;
	va_start(va, format);
	int len = VAssemble(buff, 16382, format, va);
	va_end(va);

	if(len > 0)
	{
		Lock();
		for(map<UINT, FactionUser>::iterator Iter = mUsers.begin();Iter!=mUsers.end();Iter++)
		{
			if(team == TeamNone || team == Iter->second.team)
			{
				Iter->second.pUser->pUserSocket->Send("b", len, buff);
			}
		}
		Unlock();
	}
}

bool KickOutFromColiseum(User *pUser)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->pos.x > 147534 && pUser->pSD->pos.x < 151470)
		{
			if(pUser->pSD->pos.y > 45274 && pUser->pSD->pos.y < 48164)
			{
				pUser->StopMove();
				pUser->TeleportToLocation(146918, 46723, -3400);
			}
		}
	}
	return false;
}

void CFaction::CloseColiseum()
{
	CDoor *pDoor = g_DoorDB.GetDoor(L"aden_colosseum_001_002");
	if(pDoor)
		pDoor->Close();

	pDoor = g_DoorDB.GetDoor(L"aden_colosseum_002_001");
	if(pDoor)
		pDoor->Close();

	g_UserDb.ExecuteForAllUsers(KickOutFromColiseum);
}

void CFaction::OpenColiseum()
{
	CDoor *pDoor = g_DoorDB.GetDoor(L"aden_colosseum_001_002");
	if(pDoor)
		pDoor->Open();

	pDoor = g_DoorDB.GetDoor(L"aden_colosseum_002_001");
	if(pDoor)
		pDoor->Open();
}