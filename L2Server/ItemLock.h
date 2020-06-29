#pragma once

struct ItemLockData
{
	wstring basePwd;
	wstring newPwd;
	BOOL isLocked;
	INT32 unlockAttempts;
	ItemLockData() : isLocked(FALSE), unlockAttempts(0) {};
};

class CItemLock
{
	CSLock m_Lock;
	BOOL m_Enabled;
	INT32 m_AttemptLimit;
	INT32 m_MinPasswordLength;
	INT32 m_MaxPasswordLength;
	BOOL m_AutoLockOnExit;
	BOOL m_PasswordOnLock;
	INT32 m_BanTime;
	map<UINT, ItemLockData> m_Data;
	void LoadINI();
public:
	CItemLock();
	~CItemLock();
	void Init();
	void DBLoad(UINT charId, wstring password, BOOL locked);
	bool RequestShowMenu(User *pUser);
	bool RequestSetPassword(User *pUser, wstring message = L"");
	bool OnSetPassword(User *pUser, wstring pwd);
	bool RequestConfirmPassword(User *pUser, wstring message = L"");
	bool OnConfirmPassword(User *pUser, wstring pwd);
	bool SetPasswordDone(User *pUser, wstring pwd);
	bool RequestLock(User *pUser, wstring message = L"");
	bool OnLock(User *pUser, wstring pwd);
	bool LockDone(User *pUser);
	bool RequestUnlock(User *pUser, wstring message = L"");
	bool OnUnlock(User *pUser, wstring pwd);
	bool UnlockDone(User *pUser);
	bool HandleBypass(User *pUser, wstring bypass);
	void OnEnterWorld(User *pUser);
	void OnLeaveWorld(User *pUser);
};

extern CItemLock g_ItemLock;