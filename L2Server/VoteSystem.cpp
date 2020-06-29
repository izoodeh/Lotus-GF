#include "StdAfx.h"
#include "VoteSystem.h"
#include "HTMLCacheManager.h"
#include "DB.h"

CVoteSystem g_VoteSystem;

CVoteSystem::CVoteSystem() : enabled(0), rewardId(57), voteCmdAsConvert(FALSE)
{
	SetName(L"Vote System");
}

CVoteSystem::~CVoteSystem()
{
}

void CVoteSystem::Init()
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

void CVoteSystem::LoadINI()
{
	const TCHAR* sectionName = _T("VoteSystem");
	enabled = GetPrivateProfileInt(sectionName, _T("Enabled"), 0, g_ConfigFile);
	rewardId = GetPrivateProfileInt(sectionName, _T("RewardId"), 57, g_ConfigFile);
	voteCmdAsConvert = GetPrivateProfileInt(sectionName, _T("VoteCmdAsConvert"), 0, g_ConfigFile);
}

BOOL CVoteSystem::IsEnabled()
{
	return enabled;
}

void CVoteSystem::ConvertPoints(User *pUser)
{
	if(pUser->ValidUser())
	{
		pUser->pEUD->Lock();
		if(pUser->pEUD->votePoints > 0)
		{
			if(pUser->inventory.CheckAddable(rewardId, pUser->pEUD->votePoints))
			{
				g_Log.Add(CLog::Audit, "[%S] User[%S] converting vote points[%d]", GetName(), pUser->pSD->wName, pUser->pEUD->votePoints);
				pUser->AddItemToInventory(rewardId, pUser->pEUD->votePoints);
				pUser->pEUD->votePoints = 0;
				g_DB.RequestSaveVotePoints(pUser->pUserSocket->accountId, 0);
				pUser->pUserSocket->SendSystemMessage(GetName(), L"All vote points have been converted.");
			}else
			{
				pUser->pUserSocket->SendSystemMessage(GetName(), L"Your inventory is full!");
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(GetName(), L"Not enough vote points!");
		}
		pUser->pEUD->Unlock();
	}
}

bool CVoteSystem::HandleCmd(User *pUser, const WCHAR *wParam)
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
				str << L"You have " << pUser->pEUD->votePoints << L" vote point(s).";
				pUser->pUserSocket->SendSystemMessage(GetName(), str.str().c_str());
				return true;
			}			
		}
		
		if(voteCmdAsConvert)
		{
			ConvertPoints(pUser);
			return true;
		}

		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"vote.htm", pUser->pSD->language);
		if(wHtml)
		{
			pUser->ShowHTML(L"vote.htm", wHtml);
			return true;
		}else
		{
			g_Log.Add(CLog::Error, "[%S] Cannot load vote.htm! - check HTML directory!", GetName());
		}
	}
	return false;
}