#include "stdafx.h"
#include "ItemLock.h"
#include "HTMLCacheManager.h"
#include "DB.h"
#include "CliExt.h"
#include "UserDb.h"

CItemLock g_ItemLock;

CItemLock::CItemLock()
{

}

CItemLock::~CItemLock()
{

}

void CItemLock::Init()
{
	LoadINI();
	if(m_Enabled)
	{
		g_Log.Add(CLog::Blue, "[%s] Feature is enabled.", __FUNCTION__);
		g_DB.RequestLoadItemLocks();
	}
}

/*
[ItemLock]
;enables feature .itemlock
Enabled=1
;Unlock/Change password attempt limit
AttemptLimit=3
MinPasswordLength=6
;Depends on password field size in mssql
MaxPasswordLength=8
;Automatically locks character on leave world
AutoLockOnExit=0
BanTime=900
*/

void CItemLock::LoadINI()
{
	const TCHAR* section = _T("ItemLock");
	m_Enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, g_ConfigFile);
	m_AttemptLimit = GetPrivateProfileInt(section, _T("AttemptLimit"), 3, g_ConfigFile);
	m_MinPasswordLength = GetPrivateProfileInt(section, _T("MinPasswordLength"), 6, g_ConfigFile);
	m_MaxPasswordLength = GetPrivateProfileInt(section, _T("MaxPasswordLength"), 6, g_ConfigFile);
	m_AutoLockOnExit = GetPrivateProfileInt(section, _T("AutoLockOnExit"), 0, g_ConfigFile);
	m_BanTime = GetPrivateProfileInt(section, _T("BanTime"), 900, g_ConfigFile);
	m_PasswordOnLock = GetPrivateProfileInt(section, _T("PasswordOnLock"), 0, g_ConfigFile);
}

void CItemLock::DBLoad(UINT charId, std::wstring password, BOOL locked)
{
	guard;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(charId);
	if(it!=m_Data.end())
	{
		it->second.basePwd = password;
		it->second.isLocked = locked;
		if(User *pUser = g_UserDb.FindByDatabaseId(charId))
		{
			pUser->pEUD->isItemDataLock = locked;
		}
	}else
	{
		ItemLockData ild;
		ild.basePwd = password;
		if(password.size() == 0)
		{
			locked = FALSE;
		}
		ild.isLocked = locked;

		if(User *pUser = g_UserDb.FindByDatabaseId(charId))
		{
			pUser->pEUD->isItemDataLock = locked;
		}
		m_Data.insert(pair<UINT, ItemLockData>(charId, ild));
	}
	m_Lock.Leave();
	unguard;
}

bool CItemLock::RequestShowMenu(User *pUser)
{
	guard;

	if(m_Enabled)
	{
		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_menu.htm", 0);
		if(wHtml)
		{
			pUser->ShowHTML(L"item_lock_menu.htm", wHtml, 0);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_menu.htm !", __FUNCTION__);
		}
	}
	unguard;
	return false;
}

bool CItemLock::HandleBypass(User *pUser, std::wstring bypass)
{
	guard;
	bool ret = false;
	if(m_Enabled)
	{
		if(bypass == L"il_r_s")
		{
			ret = RequestSetPassword(pUser);
		}else if(bypass.find(L"il_r_s_n?") == 0)
		{
			wstring pwd = GetBypassValueString(bypass.c_str(), L"pwd");
			ret = OnSetPassword(pUser, pwd);
		}else if(bypass.find(L"il_r_s_c?") == 0)
		{
			wstring pwd = GetBypassValueString(bypass.c_str(), L"pwd");
			ret = OnConfirmPassword(pUser, pwd);
		}else if(bypass == L"il_r_u")
		{
			ret = RequestUnlock(pUser);
		}else if(bypass.find(L"il_u?") == 0)
		{
			wstring pwd = GetBypassValueString(bypass.c_str(), L"pwd");
			ret = OnUnlock(pUser, pwd);
		}else if(bypass == L"il_r_l")
		{
			ret = RequestLock(pUser);
		}else if(bypass.find(L"il_l?") == 0)
		{
			wstring pwd = GetBypassValueString(bypass.c_str(), L"pwd");
			ret = OnLock(pUser, pwd);
		}
	}
	unguard;
	return ret;
}

bool CItemLock::RequestSetPassword(User *pUser, std::wstring message)
{
	guard;
	bool ret = false;
	BOOL isLocked = FALSE;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		isLocked = it->second.isLocked;
	}else
	{
		ItemLockData ild;
		m_Data.insert(pair<UINT, ItemLockData>(pUser->databaseId, ild));
	}
	m_Lock.Leave();

	if(isLocked)
	{
		ret = RequestUnlock(pUser, L"In order to continue, you have to unlock your character!");
	}else
	{
		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_set_password.htm", 0);
		if(wHtml)
		{
			wstring html(wHtml);
			html = Utils::ReplaceString(html, L"<?message?>", message, true);
			pUser->ShowHTML(L"item_lock_set_password.htm", html.c_str(), 0);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_set_password.htm !", __FUNCTION__);
		}
	}

	unguard;
	return ret;
}

bool CItemLock::OnSetPassword(User *pUser, std::wstring pwd)
{
	guard;
	bool ret = false;

	bool canContinue = false;
	bool lockedChar = false;
	wstring message;
	if(pwd.size() >= m_MinPasswordLength && pwd.size() <= m_MaxPasswordLength)
	{
		if(IsValidSqlString(pwd.c_str()))
		{
			m_Lock.Enter();
			map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
			if(it!=m_Data.end())
			{
				if(it->second.isLocked == FALSE)
				{
					it->second.newPwd = pwd;
					canContinue = true;
				}else
				{
					lockedChar = true;
				}
			}
			m_Lock.Leave();
		}else
		{
			ret = RequestSetPassword(pUser, L"Invalid character detected!");
		}
	}else
	{
		ret = RequestSetPassword(pUser, L"Incorrect password length!");
	}

	if(canContinue)
	{
		ret = RequestConfirmPassword(pUser);
	}else if(lockedChar)
	{
		g_Log.Add(CLog::Error, L"[%S] Character is locked - User[%s] !", __FUNCTION__, pUser->pSD->wName);
	}

	unguard;
	return ret;
}

bool CItemLock::RequestConfirmPassword(User *pUser, std::wstring message)
{
	guard;
	bool ret = false;
	BOOL isLocked = FALSE;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		isLocked = it->second.isLocked;
	}else
	{
		ItemLockData ild;
		m_Data.insert(pair<UINT, ItemLockData>(pUser->databaseId, ild));
	}
	m_Lock.Leave();

	if(isLocked)
	{
		ret = RequestUnlock(pUser, L"In order to continue, you have to unlock your character!");
	}else
	{
		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_set_password_confirm.htm", 0);
		if(wHtml)
		{
			wstring html(wHtml);
			html = Utils::ReplaceString(html, L"<?message?>", message, true);
			pUser->ShowHTML(L"item_lock_set_password_confirm.htm", html.c_str(), 0);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_set_password_confirm.htm !", __FUNCTION__);
		}
	}

	unguard;
	return ret;
}

bool CItemLock::OnConfirmPassword(User *pUser, wstring pwd)
{
	guard;
	bool ret = false;
	bool canContinue = false;
	bool charLocked = false;
	bool invalidConfirmPwd = false;
	wstring message;
	if(pwd.size() >= m_MinPasswordLength && pwd.size() <= m_MaxPasswordLength)
	{
		m_Lock.Enter();
		map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
		if(it!=m_Data.end())
		{
			if(it->second.isLocked == FALSE)
			{
				if(it->second.newPwd == pwd)
				{
					canContinue = true;
					it->second.basePwd = pwd;
					it->second.newPwd = L"";
					it->second.isLocked = TRUE;
				}else
				{
					it->second.newPwd = L"";
					invalidConfirmPwd = true;
				}
			}else
			{
				it->second.newPwd = L"";
				charLocked = true;
			}
		}
		m_Lock.Leave();
	}else
	{
		ret = RequestSetPassword(pUser, L"Incorrect confirm password length!");
	}
	if(canContinue)
	{
		g_DB.RequestSetItemLockPassword(pUser->databaseId, pwd.c_str(), TRUE);
		pUser->pEUD->isItemDataLock = TRUE;
		ret = SetPasswordDone(pUser, pwd);
	}else if(charLocked)
	{
		ret = RequestUnlock(pUser, L"In order to continue, you have to unlock your character!");
	}else if(invalidConfirmPwd)
	{
		ret = RequestSetPassword(pUser, L"Incorrect confirm password!");
	}
	unguard;
	return ret;
};

bool CItemLock::SetPasswordDone(User *pUser, wstring pwd)
{
	guard;
	const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_set_password_done.htm", 0);
	if(wHtml)
	{
		wstring html(wHtml);
		html = Utils::ReplaceString(html, L"<?password?>", pwd, true);
		pUser->ShowHTML(L"item_lock_set_password_done.htm", html.c_str(), 0);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_set_password_done.htm !", __FUNCTION__);
	}
	unguard;
	return false;
}

bool CItemLock::RequestUnlock(User *pUser, std::wstring message)
{
	guard;
	bool ret = false;
	INT32 attempts = 0;
	bool canContinue = false;
	bool alreadyUnlocked = false;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		if(it->second.basePwd.size() > 0)
		{
			canContinue = true;
			if(it->second.isLocked == FALSE)
			{
				alreadyUnlocked = true;
			}
			attempts = it->second.unlockAttempts;
		}
	}
	m_Lock.Leave();
	if(canContinue)
	{
		if(alreadyUnlocked && message.size() == 0)
		{
			message = L"Your character is already unlocked!";
		}
		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_unlock.htm", 0);
		if(wHtml)
		{
			wstring html(wHtml);
			html = Utils::ReplaceString(html, L"<?message?>", message, true);
			WCHAR temp[9];
			_itow_s(attempts, temp, 8, 10);
			html = Utils::ReplaceString(html, L"<?attempts?>", temp, true);
			_itow_s(m_AttemptLimit, temp, 8, 10);
			html = Utils::ReplaceString(html, L"<?attempt_limit?>", temp, true);

			pUser->ShowHTML(L"item_lock_unlock.htm", html.c_str(), 0);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_unlock.htm !", __FUNCTION__);
		}
	}else
	{
		ret = RequestSetPassword(pUser, L"In order to continue, you have to set the password!");
	}

	unguard;
	return ret;
}

bool CItemLock::OnUnlock(User *pUser, std::wstring pwd)
{
	guard;
	bool ret = false;
	bool canContinue = false;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		if(it->second.basePwd.size() > 0 && it->second.basePwd == pwd)
		{
			canContinue = true;
			it->second.isLocked = FALSE;
			it->second.unlockAttempts = 0;
		}else
		{
			it->second.unlockAttempts++;
			if(it->second.unlockAttempts >= m_AttemptLimit)
			{
				ret = true;
				it->second.unlockAttempts = 0;
			}
		}
	}
	m_Lock.Leave();

	if(canContinue)
	{
		g_DB.RequestSetItemLockState(pUser->databaseId, FALSE);
		pUser->pEUD->isItemDataLock = FALSE;
		ret = UnlockDone(pUser);
	}else if(!ret)
	{
		ret = RequestUnlock(pUser, L"Incorrect password!");
	}

	if(ret && m_BanTime > 0)
	{
		pUser->pEUD->kickTick = 2;
		g_CliExt.Ban(pUser->pUserSocket->hardwareId.hash, pUser->pUserSocket->accountNameW, L"ItemLock", (static_cast<UINT>(time(0)) + m_BanTime));
	}
	unguard;
	return ret;
}

bool CItemLock::UnlockDone(User *pUser)
{
	guard;
	const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_unlock_done.htm", 0);
	if(wHtml)
	{
		pUser->ShowHTML(L"item_lock_unlock_done.htm", wHtml, 0);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_unlock_done.htm", __FUNCTION__);
	}
	unguard;
	return false;
}

bool CItemLock::RequestLock(User *pUser, std::wstring message)
{
	guard;
	bool ret = false;
	INT32 attempts = 0;
	bool canContinue = false;
	bool alreadyLocked = false;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		if(it->second.basePwd.size() > 0)
		{
			canContinue = true;
			if(it->second.isLocked == TRUE)
			{
				alreadyLocked = true;
			}
			attempts = it->second.unlockAttempts;
			if(m_PasswordOnLock == FALSE)
			{
				it->second.isLocked = TRUE;
			}
		}
	}
	m_Lock.Leave();
	if(canContinue)
	{
		if(m_PasswordOnLock)
		{
			if(alreadyLocked && message.size() == 0)
			{
				message = L"Your character is already locked!";
			}
			const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_lock.htm", 0);
			if(wHtml)
			{
				wstring html(wHtml);
				html = Utils::ReplaceString(html, L"<?message?>", message, true);
				WCHAR temp[9];
				_itow_s(attempts, temp, 8, 10);
				html = Utils::ReplaceString(html, L"<?attempts?>", temp, true);
				_itow_s(m_AttemptLimit, temp, 8, 10);
				html = Utils::ReplaceString(html, L"<?attempt_limit?>", temp, true);

				pUser->ShowHTML(L"item_lock_lock.htm", html.c_str(), 0);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_lock.htm !", __FUNCTION__);
			}
		}else
		{
			g_DB.RequestSetItemLockState(pUser->databaseId, TRUE);
			pUser->pEUD->isItemDataLock = TRUE;
			ret = LockDone(pUser);
		}
	}else
	{
		ret = RequestSetPassword(pUser, L"In order to continue, you have to set the password!");
	}
	unguard;
	return ret;
}

bool CItemLock::OnLock(User *pUser, std::wstring pwd)
{
	guard;
	bool ret = false;
	bool canContinue = false;
	m_Lock.Enter();
	map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
	if(it!=m_Data.end())
	{
		if(it->second.basePwd.size() > 0 && it->second.basePwd == pwd)
		{
			canContinue = true;
			it->second.isLocked = TRUE;
			it->second.unlockAttempts = 0;
		}else
		{
			it->second.unlockAttempts++;
			if(it->second.unlockAttempts >= m_AttemptLimit)
			{
				ret = true;
				it->second.unlockAttempts = 0;
			}
		}
	}
	m_Lock.Leave();

	if(canContinue)
	{
		g_DB.RequestSetItemLockState(pUser->databaseId, TRUE);
		pUser->pEUD->isItemDataLock = TRUE;
		ret = LockDone(pUser);
	}else if(!ret)
	{
		ret = RequestLock(pUser, L"Incorrect password!");
	}

	if(ret && m_BanTime > 0)
	{
		pUser->pEUD->kickTick = 2;
		g_CliExt.Ban(pUser->pUserSocket->hardwareId.hash, pUser->pUserSocket->accountNameW, L"ItemLock", (static_cast<UINT>(time(0)) + m_BanTime));
	}
	unguard;
	return ret;
}

bool CItemLock::LockDone(User *pUser)
{
	guard;
	const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"item_lock_lock_done.htm", 0);
	if(wHtml)
	{
		pUser->ShowHTML(L"item_lock_lock_done.htm", wHtml, 0);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Cannot find item_lock_lock_done.htm", __FUNCTION__);
	}
	unguard;
	return false;
}

void CItemLock::OnEnterWorld(User *pUser)
{
	guard;
	if(m_Enabled)
	{
		BOOL isLocked = FALSE;
		m_Lock.Enter();
		map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
		if(it!=m_Data.end())
		{
			isLocked = it->second.isLocked;
		}
		m_Lock.Leave();
		if(isLocked)
		{
			pUser->pEUD->isItemDataLock = isLocked;
			pUser->pUserSocket->SendSystemMessage(L"Your character is protected with Item Lock.");
		}
	}
	unguard;
}

void CItemLock::OnLeaveWorld(User *pUser)
{
	guard;
	if(m_Enabled)
	{
		BOOL setLock = FALSE;
		m_Lock.Enter();
		map<UINT, ItemLockData>::iterator it = m_Data.find(pUser->databaseId);
		if(it!=m_Data.end())
		{
			if(m_AutoLockOnExit && it->second.isLocked == FALSE && it->second.basePwd.size() > 0)
			{
				it->second.isLocked = TRUE;
				setLock = TRUE;
			}
		}
		m_Lock.Leave();
		if(setLock)
		{
			g_DB.RequestSetItemLockState(pUser->databaseId, TRUE);
		}
	}
	unguard;
}