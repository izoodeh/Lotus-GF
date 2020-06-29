#include "StdAfx.h"
#include "UserCommandEx.h"
#include "VoteSystem.h"
#include "DonateSystem.h"
#include "PvPSystem.h"
#include "OfflineShop.h"
#include "HTMLCacheManager.h"
#include "ItemLock.h"

CUserCommandEx g_UserCommandEx;

extern BOOL g_EnableExpStopCommand;

CUserCommandEx::CUserCommandEx()
{
	SetName(L"User Command Ex");
}

CUserCommandEx::~CUserCommandEx()
{
}

void CUserCommandEx::Init()
{
	LoadINI();
}

void CUserCommandEx::LoadINI()
{
	const TCHAR* sectionName = _T("UserCommandEx");
	enabled = GetPrivateProfileInt(sectionName, _T("Enabled"), 0, g_ConfigFile);

	TCHAR sTmp[2048];
	if(GetPrivateProfileString(sectionName, _T("VoteCommand"), _T("vote"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_VOTE));
	}

	if(GetPrivateProfileString(sectionName, _T("DonateCommand"), _T("donate"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_DONATE));
	}

	if(GetPrivateProfileString(sectionName, _T("PvPCommand"), _T("pvp"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_PVP));
	}

	if(GetPrivateProfileString(sectionName, _T("ExpStop"), _T("expstop"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_EXPSTOP));
	}

	if(GetPrivateProfileString(sectionName, _T("ExpStart"), _T("expstart"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_EXPSTART));
	}

	if(GetPrivateProfileString(sectionName, _T("OfflineShop"), _T("offline"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_OFFLINE_SHOP));
	}
	if(GetPrivateProfileString(sectionName, _T("Menu"), _T("menu"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_MENU));
	}
	if(GetPrivateProfileString(sectionName, _T("ItemLock"), _T("itemlock"), sTmp, 2048, g_ConfigFile))
	{
		mData.insert(pair<wstring, USER_COMMAND_EX_TYPE>(sTmp, UCE_ITEM_LOCK));
	}
}

bool CUserCommandEx::IsCommand(WCHAR *wStr)
{
	if(wStr[0] == L'.')
	{
		for(map<wstring, USER_COMMAND_EX_TYPE>::iterator Iter = mData.begin();Iter!=mData.end();Iter++)
		{
			if(!_wcsnicmp(&wStr[1], Iter->first.c_str(), Iter->first.size()))
			{
				return true;
			}
		}
	}
	return false;
}

void CUserCommandEx::Handle(CUserSocket *pSocket, User *pUser, WCHAR *wStr)
{
	for(map<wstring, USER_COMMAND_EX_TYPE>::iterator Iter = mData.begin();Iter!=mData.end();Iter++)
	{
		if(!_wcsnicmp(&wStr[1], Iter->first.c_str(), Iter->first.size()))
		{
			const WCHAR* wParams = &wStr[2+Iter->first.size()];
			switch(Iter->second)
			{
			case UCE_VOTE:
				{
					g_VoteSystem.HandleCmd(pUser, wParams);
					return;
				}
			case UCE_DONATE:
				{
					g_DonateSystem.HandleCmd(pUser, wParams);
					return;
				}
			case UCE_PVP:
				{
					g_PvPSystem.HandleCmd(pUser, wParams);
					return;
				}
			case UCE_EXPSTOP:
				{
					if( g_EnableExpStopCommand )
					{
						if(pUser->pEUD->blockExp == FALSE)
						{
							pUser->pEUD->blockExp = TRUE;
							pSocket->SendSystemMessage(L"System", L"Ability to gain exp has been disabled.");
						}
					}else
					{
						pSocket->SendSystemMessage(L"System", L".expstop command has been disabled.");
					}
					return;
				}
			case UCE_EXPSTART:
				{
					if(pUser->pEUD->blockExp == TRUE)
					{
						pUser->pEUD->blockExp = FALSE;
						pSocket->SendSystemMessage(L"System", L"Ability to gain exp has been enabled.");
					}
					return;
				}
			case UCE_OFFLINE_SHOP:
				{
					g_OfflineShop.RequestActivate(pUser);
					return;
				}
			case UCE_MENU:
				{
					const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"menu.htm", pUser->pSD->language);
					if(wHtml)
					{
						pUser->ShowHTML(L"menu.htm", wHtml);
					}
					return;
				}
			case UCE_ITEM_LOCK:
				{
					UserCommandExItemLock(pUser, wParams);
					return;
				}
			default:
				{
					g_Log.Add(CLog::Error, "[%S] Unknown type[%S]", GetName(), wStr);
					break;
				}
			};
			return;
		}
	}
}

bool UserCommandExItemLock(User *pUser, const WCHAR* wParams)
{
	guard;
	g_ItemLock.RequestShowMenu(pUser);
	unguard;
	return false;
}