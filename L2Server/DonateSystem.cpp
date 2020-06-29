#include "StdAfx.h"
#include "DonateSystem.h"
#include "HTMLCacheManager.h"
#include "DB.h"

CDonateSystem g_DonateSystem;

CDonateSystem::CDonateSystem() : enabled(0), rewardId(57), donateCmdAsConvert(FALSE)
{
	SetName(L"Donate System");
}

CDonateSystem::~CDonateSystem()
{
}

void CDonateSystem::Init()
{
	LoadINI();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled - rewardId[%d]", GetName(), rewardId);
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
	}
}

void CDonateSystem::LoadINI()
{
	const TCHAR* sectionName = _T("DonateSystem");
	enabled = GetPrivateProfileInt(sectionName, _T("Enabled"), 0, g_ConfigFile);
	rewardId = GetPrivateProfileInt(sectionName, _T("RewardId"), 57, g_ConfigFile);
	donateCmdAsConvert = GetPrivateProfileInt(sectionName, _T("DonateCmdAsConvert"), 0, g_ConfigFile);
}

BOOL CDonateSystem::IsEnabled()
{
	return enabled;
}

void CDonateSystem::ConvertPoints(User *pUser)
{
	if(pUser->ValidUser())
	{
		pUser->pEUD->Lock();
		if(pUser->pEUD->donatePoints > 0)
		{
			if(pUser->inventory.CheckAddable(rewardId, pUser->pEUD->donatePoints))
			{
				g_Log.Add(CLog::Audit, "[%S] User[%S] converting donate points[%d]", GetName(), pUser->pSD->wName, pUser->pEUD->donatePoints);
				pUser->AddItemToInventory(rewardId, pUser->pEUD->donatePoints);
				pUser->pEUD->donatePoints = 0;
				g_DB.RequestSaveDonatePoints(pUser->pUserSocket->accountId, 0);
				pUser->pUserSocket->SendSystemMessage(GetName(), L"All donate points have been converted.");
			}else
			{
				pUser->pUserSocket->SendSystemMessage(GetName(), L"Your inventory is full!");
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(GetName(), L"Not enough donate points!");
		}
		pUser->pEUD->Unlock();
	}
}

bool CDonateSystem::HandleCmd(User *pUser, const WCHAR *wParam)
{
	if(pUser->ValidUser())
	{
		size_t len = wcslen(wParam);
		if(len > 0)
		{
			if(!wcscmp(wParam, L"convert"))
			{
				ConvertPoints(pUser);
				return true;
			}else if(!wcscmp(wParam, L"points"))
			{
				wstringstream str;
				str << L"You have " << pUser->pEUD->donatePoints << L" donate point(s).";
				pUser->pUserSocket->SendSystemMessage(GetName(), str.str().c_str());
				return true;
			}
		}
		
		if(donateCmdAsConvert)
		{
			ConvertPoints(pUser);
			return true;
		}

		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"donate.htm", pUser->pSD->language);
		if(wHtml)
		{
			pUser->ShowHTML(L"donate.htm", wHtml);
			return true;
		}else
		{
			g_Log.Add(CLog::Error, "[%S] Cannot load donate.htm! - check HTML directory!", GetName());
		}
	}
	return false;
}