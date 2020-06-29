#include "stdafx.h"
#include "ItemDecay.h"

CItemDecay g_ItemDecay;

CItemDecay::CItemDecay() : enabled(FALSE), defaultDecay(60000)
{
	SetName(L"Item Decay");
}

CItemDecay::~CItemDecay()
{
}

void CItemDecay::Init()
{
	defaultDecay = 60000;
	enabled = FALSE;
	lNotAffected.clear();

	LoadINI();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. NotAffected[%d] CustomTime[%d]", GetName(), lNotAffected.size(), mData.size());
	}
}

void CItemDecay::LoadINI()
{
	const TCHAR* section = _T("ItemDecay");
	enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, g_ConfigFile);
	defaultDecay = GetPrivateProfileInt(section, _T("DefaultTime"), 60, g_ConfigFile);
	TCHAR temp[8192];
	memset(temp, 0, sizeof(temp));
	if(GetPrivateProfileString(section, _T("NotAffectedItemList"), 0, temp, 8192, g_ConfigFile))
	{
		wstringstream str;
		str << temp;
		INT32 itemId = 0;
		while(str >> itemId)
		{
			lNotAffected.push_back(itemId);
		}
		lNotAffected.sort();
		lNotAffected.unique();
	}

	memset(temp, 0, sizeof(temp));
	if(GetPrivateProfileString(section, _T("CustomTimeItemList"), 0, temp, 8192, g_ConfigFile))
	{
		wstringstream str;
		str << temp;
		INT32 itemId = 0;
		while(str >> itemId)
		{
			wstringstream sKey;
			sKey << L"Time_" << itemId;
			UINT decay = GetPrivateProfileInt(section, sKey.str().c_str(), 60, g_ConfigFile);
			mData.insert(pair<INT32, UINT>(itemId, decay));
		}
	}
}

UINT CItemDecay::GetItemDecay(INT32 classId)
{
	map<INT32, UINT>::iterator Iter = mData.find(classId);
	if(Iter!=mData.end())
	{
		return Iter->second;
	}
	return defaultDecay;
}

BOOL CItemDecay::IsAffected(INT32 classId)
{
	for(list<INT32>::iterator Iter = lNotAffected.begin();Iter!=lNotAffected.end();Iter++)
	{
		if((*Iter) == classId)
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CItemDecay::HandleDroppedItem(CItem *pItem)
{
	guard;

	if(enabled)
	{
		if(pItem->ValidItem())
		{
			if(pItem->pII->etcItemType != ETCITEM_CASTLE_GUARD)
			{
				if(IsAffected(pItem->pSID->itemId))
				{
					UINT decayTime = GetItemDecay(pItem->pSID->itemId);
					ItemDecayData idd;
					idd.pItem = pItem;
					idd.decayTimestmap = time(NULL) + decayTime;
					Lock();
					lItems.push_back(idd);
					Unlock();
				}
			}
		}
	}
	unguard;
}



void CItemDecay::TimerExpired()
{
	guard;

	if(enabled)
	{
		time_t currentTime = time(NULL);
		Lock();
		for(list<ItemDecayData>::iterator Iter = lItems.begin();Iter!=lItems.end();)
		{
			if(Iter->pItem->ValidItem())
			{
				if(Iter->decayTimestmap < currentTime)
				{
					Iter->pItem->AddTimer(100, ITEM_DECAY_TIMER);
					lItems.erase(Iter++);
				}else
				{
					Iter++;
				}
			}else
			{
				lItems.erase(Iter++);
			}			
		}
		Unlock();
	}
	unguard;
}

void CItemDecay::OnPickUp(CItem *pItem)
{
	guard;

	if(enabled)
	{
		Lock();
		for(list<ItemDecayData>::iterator Iter = lItems.begin();Iter!=lItems.end();Iter++)
		{
			if(Iter->pItem == pItem)
			{
				lItems.erase(Iter);
				break;
			}
		}
		Unlock();
	}
	unguard;
}