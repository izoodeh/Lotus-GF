#include "StdAfx.h"
#include "Party.h"

UserVector CParty::GetAllMembers()
{
	typedef void (*f)(CParty*, void*);

	BYTE pVectorBuff[64];
	memset(pVectorBuff, 0x00, 64);

	f(0x77E9D8L)(this, pVectorBuff);
	return *(UserVector*)&pVectorBuff;
}

void CParty::Dismiss(bool param)
{
	typedef void(*f)(CParty*, bool);
	f(0x0783A14L)(this, param);
}

void CParty::Withdraw(User *pUser, bool param)
{
	typedef void(*f)(CParty*, User*, bool);
	f(0x7844B8L)(this, pUser, param);
}

void CParty::GetAllMembersEx(UserVector& members, bool useLock)
{
	guard;
	if(useLock)
	{
		pLock->Enter(__FILEW__, __LINE__);
	}

	for(UINT n=0;n<memberCount;n++)
	{
		if(n<9)
		{
			if(User *pUser = User::GetUser(&memberId[n]))
			{
				if(pUser->ValidUser())
				{
					members.push_back(pUser);
				}
			}
		}
	}

	if(useLock)
	{
		pLock->Leave();
	}
	unguard;
}

void CParty::GetAllMembersInRange(UserVector &members, FVector &pos, double range)
{
	if(this)
	{
		double x1 = pos.x;
		double y1 = pos.y;
		double z1 = pos.z;

		pLock->Enter(__FILEW__, __LINE__);
		for(UINT n=0;n<memberCount;n++)
		{
			if(n < 9)
			{
				if(User *pUser = User::GetUser(&memberId[n]))
				{
					if(pUser->ValidUser() && pUser->pSD->active)
					{
						double x2 = pUser->pSD->pos.x;
						double y2 = pUser->pSD->pos.y;
						double z2 = pUser->pSD->pos.z;

						double dist = sqrt(pow((x2-x1), 2) + pow((y2-y1), 2) + pow((z2-z1), 2));
						if(dist < range)
						{
							members.push_back(pUser);
						}
					}
				}
			}
		}
		pLock->Leave();
	}
}