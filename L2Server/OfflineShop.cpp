#include "StdAfx.h"
#include "OfflineShop.h"
#include "HookManager.h"

COfflineShop g_OfflineShop;


COfflineShop::COfflineShop() : enabled(FALSE), requiredItemId(0), requiredItemCount(0), shopDuration(0), limitPerIPnMAC(0), notEnoughRequiredItemMsgId(0), notMeetingRequirementsMsgId(0), deactivateMsgId(0), activateMsgId(0), disabledMsgId(0)
{
	SetName(L"Offline Shop");
}

COfflineShop::~COfflineShop()
{
}

void COfflineShop::Init()
{
	LoadINI();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled", GetName());
		WriteJump(0x93D388, (INT64)OfflineShopCheck, 9);
		WriteCall(0x93D3C7, COfflineShop::ReleaseSocket);
		WriteCall(0x4D5E01, COfflineShop::OnKick);
		WriteCall(0x6B3ED9, COfflineShop::StatusWnd);

	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
	}
}

void COfflineShop::LoadINI()
{
	const TCHAR* sectionName = TEXT("OfflineShop");
	enabled = GetPrivateProfileInt(sectionName, TEXT("Enabled"), 0, g_ConfigFile);
	requiredItemId = GetPrivateProfileInt(sectionName, TEXT("RequiredItemId"), 57, g_ConfigFile);
	requiredItemCount = GetPrivateProfileInt(sectionName, TEXT("RequiredItemCount"), 10000, g_ConfigFile);
	shopDuration = GetPrivateProfileInt(sectionName, TEXT("ShopDuration"), 43200, g_ConfigFile);
	limitPerIPnMAC = GetPrivateProfileInt(sectionName, TEXT("LimitPerIPnMAC"), 1, g_ConfigFile);
	limitMsgId = GetPrivateProfileInt(sectionName, TEXT("LimitMsgId"), 3011, g_ConfigFile);
	notEnoughRequiredItemMsgId = GetPrivateProfileInt(sectionName, TEXT("NotEnoughRequiredItemMsgId"), 3012, g_ConfigFile);
	notMeetingRequirementsMsgId = GetPrivateProfileInt(sectionName, TEXT("NotMeetingRequirementsMsgId"), 3013, g_ConfigFile);
	activateMsgId = GetPrivateProfileInt(sectionName, TEXT("ActivateMsgId"), 3014, g_ConfigFile);
	deactivateMsgId = GetPrivateProfileInt(sectionName, TEXT("DeactivateMsgId"), 3015, g_ConfigFile);
	disabledMsgId = GetPrivateProfileInt(sectionName, TEXT("DisabledMsgId"), 3016, g_ConfigFile);
	m_IdleRewardId = GetPrivateProfileInt(sectionName, TEXT("IdleRewardId"), 3016, g_ConfigFile);
	m_IdleRewardCount = GetPrivateProfileInt(sectionName, TEXT("IdleRewardCount"), 3016, g_ConfigFile);
	m_IdleRewardTimeout = GetPrivateProfileInt(sectionName, TEXT("IdleRewardTimeout"), 3016, g_ConfigFile);
}

bool COfflineShop::CloseShop(User *pUser)
{
	bool ret = false;
	Lock();
	OfflineShopMap::iterator Iter = mData.find(pUser->pUserSocket);
	if(Iter!=mData.end())
	{
		if(Iter->second.pUSRV)
		{
			typedef void(*f)(CUserSocketReleaseVerifier*, UINT, UINT);
			f(0x68E54CL)(Iter->second.pUSRV, 100, 3);
		}else
		{
			//user in game
			pUser->pUserSocket->SendSystemMessage(deactivateMsgId);
			pUser->pEUD->offlineShopTime = 0;
			pUser->pEUD->offlineShop = FALSE;
		}

		mData.erase(Iter);
		ret = true;
	}else
	{
	//	g_Log.Add(CLog::Error, "[%S] Cannot find offline shop data for user[%S]!", GetName(), pUser->pSD->wName);
		ret = false;
	}
	Unlock();
	return ret;
}

void COfflineShop::TimerExpired(User *pUser)
{
	if(enabled)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pEUD->offlineShop)
			{
				if(pUser->pSD->storeMode == 0)
				{
					CloseShop(pUser);
					g_Log.Add(CLog::Audit, "[%S] user[%S] no store - close!", GetName(), pUser->pSD->wName);
					return;
				}
				if(!pUser->pSD->in_peacezone)
				{
					CloseShop(pUser);
					g_Log.Add(CLog::Audit, "[%S] user[%S] not in peace zone - close!", GetName(), pUser->pSD->wName);
					return;
				}

				if(pUser->pEUD->offlineShopTime)
				{
					if((pUser->pEUD->offlineShopTime + shopDuration) < time(NULL))
					{
						CloseShop(pUser);
						g_Log.Add(CLog::Audit, "[%S] user[%S] shop expired - close!", GetName(), pUser->pSD->wName);
						return;
					}
					if(m_IdleRewardId > 0 && m_IdleRewardCount > 0 && m_IdleRewardTimeout > 0)
					{
						time_t timePassed = time(0) - pUser->pEUD->offlineShopTime;
						if((timePassed % m_IdleRewardTimeout) == 0)
						{
							//give reward
							pUser->AddItemToInventory(m_IdleRewardId, m_IdleRewardCount);
						}
					}
				}
			}
		}
	}
}

UINT COfflineShop::GetIPnMACCount(UINT ip, INT64 mac)
{
	UINT count = 0;
	Lock();
	for(OfflineShopMap::iterator Iter = mData.begin();Iter!=mData.end();Iter++)
	{
		if(Iter->first->ValidUserSocket())
		{
			if(ip == Iter->first->address.S_un.S_addr && mac == Iter->first->macAddress)
			{
				count++;
			}
		}
	}
	Unlock();
	return count;
}

bool COfflineShop::RequestActivate(User *pUser)
{
	guard;

	if(pUser->ValidUser())
	{
		if(enabled)
		{
			if(pUser->pSD->in_peacezone)
			{
				if( pUser->pSD->storeMode == 0)
				{
					pUser->pUserSocket->SendSystemMessage(notMeetingRequirementsMsgId);
					unguard;
					return false;
				}

				if(!pUser->pEUD->offlineShop)
				{
					if(CParty* pParty = pUser->GetParty())
					{
						pUser->ProcessMyParty();
					}

					pUser->olympiadUser.OnLeaveWorld();

					if(pUser->IsNowTrade())
					{
						pUser->TradeCancel();
					}

					if(limitPerIPnMAC > 0 && limitPerIPnMAC <= GetIPnMACCount(pUser->pUserSocket->address.S_un.S_addr, pUser->pUserSocket->macAddress) )
					{
						pUser->pUserSocket->SendSystemMessage(limitMsgId);
						unguard;
						return false;
					}

					if(requiredItemId > 0 && requiredItemCount > 0)
					{
						if(pUser->inventory.HaveItemByClassId(requiredItemId, requiredItemCount))
						{
							pUser->DeleteItemInInventory(requiredItemId, requiredItemCount);
						}else
						{
							pUser->pUserSocket->SendSystemMessage(notEnoughRequiredItemMsgId);
							unguard;
							return false;
						}
					}

					OfflineShopData osd;
					memset(&osd, 0, sizeof(osd));
					osd.pos = pUser->pSD->pos;
					osd.pUser = pUser;
					osd.pUSRV = 0;
					osd.databaseId = pUser->databaseId;
					osd.serverId = pUser->serverId;
					osd.createTime = time(NULL);

					Lock();
					mData.insert(pair<CUserSocket*, OfflineShopData>(pUser->pUserSocket, osd));
					Unlock();
					pUser->pEUD->offlineShop = TRUE;
					pUser->pUserSocket->SendSystemMessage(activateMsgId);
					unguard;
					return true;
				}
			}else
			{
				g_Log.Add(CLog::Blue, "Not in peace zone");
				pUser->pUserSocket->SendSystemMessage(notMeetingRequirementsMsgId);
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(disabledMsgId);
		}
	}
	unguard;
	return false;
}

void COfflineShop::ReleaseSocket(CUserSocketReleaseVerifier *pUSRV, UINT timestamp, UINT id)
{
	guard;

	try
	{
		if(pUSRV->pSocket)
		{
			if(User *pUser = pUSRV->pSocket->pUser->CastUser())
			{
				if(pUser->pEUD->offlineShop)
				{
					bool found = false;
					g_OfflineShop.Lock();
					OfflineShopMap::iterator Iter = g_OfflineShop.mData.find(pUSRV->pSocket);
					if(Iter!=g_OfflineShop.mData.end())
					{
						Iter->second.pUSRV = pUSRV;
						found = true;
					}
					g_OfflineShop.Unlock();
					if(found)
					{
						pUser->pEUD->offlineShopTime = time(NULL);
						pUser->CheckAbnormalVisualEffect();
						pUser->SendCharInfo();

						g_Log.Add(CLog::Audit, "[%S] user[%S] is offline shopping", g_OfflineShop.GetName(), pUser->pSD->wName);
						unguard;
						return;
					}
				}
			}
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}

	typedef void(*f)(CUserSocketReleaseVerifier*, UINT, UINT);
	f(0x68E54CL)(pUSRV, timestamp, id);
	unguard;
}

void COfflineShop::OnKick(CUserSocket *pSocket)
{
	if(pSocket->ValidUserSocket())
	{
		if(User *pUser = pSocket->pUser->CastUser())
		{
			if(pUser->pEUD->offlineShopTime)
			{
				g_OfflineShop.CloseShop(pUser);
				return;
			}
		}
	}

	typedef void(*f)(CUserSocket*);
	f(0x4564D8L)(pSocket);
}

void COfflineShop::StatusWnd(LPVOID lpInstance, INT32 line, const WCHAR *format, int value)
{
	value = 0;
	g_OfflineShop.Lock();
	value = (INT32)g_OfflineShop.mData.size();
	g_OfflineShop.Unlock();

	typedef void(*f)(LPVOID, INT32, const WCHAR*, INT32);
	f(0x86258CL)(lpInstance, line, L"Offline Shops: %d", value);
}