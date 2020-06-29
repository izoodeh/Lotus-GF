#include "StdAfx.h"
#include "UserDb.h"

CUserDb g_UserDb;

CUserDb::CUserDb()
{
	SetName(L"User Db");
}

CUserDb::~CUserDb()
{
}

void CUserDb::Add(User *pUser)
{
	if(pUser->ValidUser())
	{
		Lock();
		try
		{
			mByDatabaseId.insert(pair<UINT, User*>(pUser->databaseId, pUser));
			mByServerId.insert(pair<UINT, User*>(pUser->serverId, pUser));
			wstring wName = pUser->pSD->wName;
			transform(wName.begin(), wName.end(), wName.begin(), towlower);
			mByName.insert(pair<wstring, User*>(wName, pUser));
		}catch(...)
		{
			EXCEPTION_LOG;
		}
		Unlock();

	//	g_Log.Add(CLog::Blue, "[%S] User[%S] added to DB.", GetName(), pUser->pSD->wName);
	}
}

void CUserDb::Del(User *pUser)
{
	if(pUser->ValidUser())
	{
		Lock();
		try
		{
			map<UINT, User*>::iterator Iter = mByDatabaseId.find(pUser->databaseId);
			if(Iter!=mByDatabaseId.end())
			{
				mByDatabaseId.erase(Iter);
			}
			Iter = mByServerId.find(pUser->serverId);
			if(Iter!=mByServerId.end())
			{
				mByServerId.erase(Iter);
			}

			wstring wName = pUser->pSD->wName;
			transform(wName.begin(), wName.end(), wName.begin(), towlower);

			map<wstring, User*>::iterator nameIter = mByName.find(wName);
			if(nameIter!=mByName.end())
			{
				mByName.erase(nameIter);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
		Unlock();

	//	g_Log.Add(CLog::Blue, "[%S] User[%S] deleted from DB.", GetName(), pUser->pSD->wName);
	}
}

User* CUserDb::FindByDatabaseId(UINT databaseId)
{
	User* pUser = 0;
	Lock();
	map<UINT, User*>::iterator Iter = mByDatabaseId.find(databaseId);
	if(Iter!=mByDatabaseId.end())
	{
		pUser = Iter->second;
	}
	Unlock();
	return pUser;
}

User* CUserDb::FindByServerId(UINT serverId)
{
	User* pUser = 0;
	Lock();
	map<UINT, User*>::iterator Iter = mByServerId.find(serverId);
	if(Iter!=mByServerId.end())
	{
		pUser = Iter->second;
	}
	Unlock();
	return pUser;
}

User* CUserDb::FindByName(std::wstring wName)
{
	User* pUser = 0;
	transform(wName.begin(), wName.end(), wName.begin(), towlower);
	Lock();
	map<wstring, User*>::iterator nameIter = mByName.find(wName);
	if(nameIter!=mByName.end())
	{
		pUser = nameIter->second;
	}
	Unlock();
	return pUser;
}

void CUserDb::ExecuteForAllUsers(UserFunction callback)
{
	Lock();
	for(map<UINT, User*>::iterator Iter = mByServerId.begin();Iter!=mByServerId.end();Iter++)
	{
		if(User *pUser = Iter->second->CastUser())
		{
			callback(pUser);
		}
	}
	Unlock();
};

void CUserDb::KickOutFromColiseum(IVector pos)
{
	Lock();
	try
	{
		for(map<UINT, User*>::iterator Iter = mByServerId.begin();Iter!=mByServerId.end();Iter++)
		{
			User *pUser = Iter->second;
			if(pUser->ValidUser())
			{
				if(pUser->pSD->pos.x > 147534 && pUser->pSD->pos.x < 151470)
				{
					if(pUser->pSD->pos.y > 45274 && pUser->pSD->pos.y < 48164)
					{
						pUser->TeleportToLocation(pos.x, pos.y, pos.z, false);
					}
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	Unlock();
}

void CUserDb::KickOutFromRange(IVector pos, int minX, int maxX, int minY, int maxY)
{
	double x1 = static_cast<double>(minX);
	double x2 = static_cast<double>(maxX);
	double y1 = static_cast<double>(minY);
	double y2 = static_cast<double>(maxY);
	
	Lock();
	try
	{
		for(map<UINT, User*>::iterator Iter = mByServerId.begin();Iter!=mByServerId.end();Iter++)
		{
			User *pUser = Iter->second;
			if(pUser->ValidUser())
			{
				if(pUser->pSD->pos.x >= x1 && pUser->pSD->pos.x <= x2)
				{
					if(pUser->pSD->pos.y >= y1 && pUser->pSD->pos.y <= y2)
					{
						pUser->TeleportToLocation(pos.x, pos.y, pos.z);
					}
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	Unlock();
}

void CUserDb::GiveItemToAll(int itemId, int amount, bool checkHwid)
{
	guard;

	vector<LPBYTE> usedHwid;

	Lock();
	for(map<UINT, User*>::iterator it = mByServerId.begin(); it!=mByServerId.end();it++)
	{
		if(User *pUser = it->second->CastUser())
		{
			if(pUser->pEUD->offlineShopTime > 0)
			{
				continue;
			}
			bool canReceive = true;
			if(checkHwid)
			{
				for(UINT n=0;n<usedHwid.size();n++)
				{
					try
					{
						if(!memcmp(usedHwid[n], pUser->pUserSocket->hardwareId.hash, 32))
						{
							canReceive = false;
							break;
						}
					}catch(...)
					{
					}
				}
			}
			if(canReceive)
			{
				usedHwid.push_back(pUser->pUserSocket->hardwareId.hash);
				pUser->AddItemToInventory(itemId, amount);
				g_Logger.Add(L"User[%s] received vote reaward[%d][%d]", pUser->pSD->wName, itemId, amount);
			}
		}
	}


	Unlock();

	unguard;
}

void CUserDb::KickUsersByHWID(LPBYTE lpHWID)
{
	guard;
	Lock();
	try
	{
		for(map<UINT, User*>::iterator it = mByServerId.begin(); it!=mByServerId.end();it++)
		{
			if(User *pUser = it->second->CastUser())
			{
				if(!memcmp(lpHWID, pUser->pUserSocket->hardwareId.hash, 32))
				{
					pUser->pEUD->kickTick = 3;
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	Unlock();
	unguard;
}

INT32 CUserDb::GetUserAmountByHWID(LPBYTE lpHWID)
{
	UINT amount = 0;
	Lock();
	try
	{
		for(map<UINT, User*>::iterator it = mByServerId.begin(); it!=mByServerId.end();it++)
		{
			if(User *pUser = it->second->CastUser())
			{
				if(pUser->pEUD->offlineShopTime == 0)
				{
					if(!memcmp(lpHWID, pUser->pUserSocket->hardwareId.hash, 32))
					{
						amount++;
					}
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	Unlock();
	return amount;
}

INT32 CUserDb::GetOlympiadUsersByHWID(LPBYTE lpHWID)
{
	UINT amount = 0;
	Lock();
	try
	{
		for(map<UINT, User*>::iterator it = mByServerId.begin(); it!=mByServerId.end();it++)
		{
			if(User *pUser = it->second->CastUser())
			{
				if(pUser->olympiadUser.status != 0)
				{
					if(!memcmp(lpHWID, pUser->pUserSocket->hardwareId.hash, 32))
					{
						amount++;
					}
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	Unlock();
	return amount;
}

INT32 CUserDb::GetTvTUsersByHWID(LPBYTE lpHWID)
{
	UINT amount = 0;
	Lock();
	try
	{
		for(map<UINT, User*>::iterator it = mByServerId.begin(); it!=mByServerId.end();it++)
		{
			if(User *pUser = it->second->CastUser())
			{
				if(pUser->pEUD->tvtUser.status != TvT::UserNone)
				{
					if(!memcmp(lpHWID, pUser->pUserSocket->hardwareId.hash, 32))
					{
						amount++;
					}
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	Unlock();
	return amount;
}