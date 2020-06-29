#include "stdafx.h"
#include "ItemReuseManager.h"

CItemReuseManager g_ItemReuseManager;

UINT UserItemReuse::GetReuse(INT32 itemId, UINT subjobIndex)
{
	guard;
	UINT reuse = 0;
	if(subjobIndex < 4)
	{
		m_Lock.Enter();
		map<INT32, UINT>::iterator it = m_ItemReuse[subjobIndex].find(itemId);
		if(it != m_ItemReuse[subjobIndex].end())
		{
			reuse = it->second;
		}
		m_Lock.Leave();
	}

	unguard;
	return reuse;
}

void UserItemReuse::SetReuse(INT32 itemId, UINT reuse, UINT subjobIndex)
{
	guard;

	if(subjobIndex < 4)
	{
		reuse += GetTickCount();
		m_Lock.Enter();
		m_ItemReuse[subjobIndex][itemId] = reuse;
		m_Lock.Leave();
	}

	unguard;
}

CItemReuseManager::CItemReuseManager() : m_Enabled(FALSE)
{

}

CItemReuseManager::~CItemReuseManager()
{
}

void CItemReuseManager::Init()
{
	m_Enabled = GetPrivateProfileInt(_T("Setting"), _T("ItemReuseManager"), 1, g_ConfigFile);
}

void CItemReuseManager::OnEnterWorld(User *pUser)
{
	guard;

	UserItemReuse* pUIR = 0;
	m_Lock.Enter();
	map<UINT, UserItemReuse*>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		pUIR = it->second;
	}else
	{
		pUIR = new UserItemReuse;
		m_Data.insert(pair<UINT, UserItemReuse*>(pUser->databaseId, pUIR));
	}
	m_Lock.Leave();

	
	if(pUIR)
	{
		pUIR->m_Lock.Enter();
		map<INT32, UINT>& itemReuse = pUIR->m_ItemReuse[pUser->pSD->subjob_id];
		map<INT32, UINT>::iterator it = itemReuse.begin();
		UINT currentTick = GetTickCount();
		for(;it!=itemReuse.end();it++)
		{
			if(it->second > currentTick)
			{
				UINT reuse = it->second - currentTick;
				pUser->SetItemUsableTime(it->first, 0, reuse);
			}
		}
		pUIR->m_Lock.Leave();
	}
	unguard;
}

void CItemReuseManager::SetReuse(User *pUser, INT32 itemId, UINT reuse)
{
	guard;

	UserItemReuse* pUIR = 0;
	m_Lock.Enter();
	map<UINT, UserItemReuse*>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		pUIR = it->second;
	}
	m_Lock.Leave();
	
	if(pUIR)
	{
		pUIR->SetReuse(itemId, reuse, pUser->pSD->subjob_id);		
	}

	unguard;
}

UINT CItemReuseManager::GetReuse(User *pUser, INT32 itemId)
{
	guard;
	UINT reuse = 0;
	UserItemReuse* pUIR = 0;
	m_Lock.Enter();
	map<UINT, UserItemReuse*>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		pUIR = it->second;
	}
	m_Lock.Leave();
	
	if(pUIR)
	{
		reuse = pUIR->GetReuse(itemId, pUser->pSD->subjob_id);
	}
	unguard;
	return reuse;
}