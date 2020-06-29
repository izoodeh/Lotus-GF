#pragma once

enum USER_COMMAND_EX_TYPE
{
	UCE_NONE = 0,
	UCE_VOTE = 1,
	UCE_DONATE = 2,
	UCE_PVP = 3,
	UCE_EXPSTOP = 4,
	UCE_EXPSTART = 5,
	UCE_OFFLINE_SHOP = 6,
	UCE_MENU = 7,
	UCE_ITEM_LOCK = 8
};

class CUserCommandEx : public CFeature
{
	BOOL enabled;
	map<wstring, USER_COMMAND_EX_TYPE> mData;
	void LoadINI();
public:
	CUserCommandEx();
	~CUserCommandEx();
	void Init();
	bool IsCommand(WCHAR* wStr);
	inline BOOL IsEnabled() { return enabled; };
	void Handle(CUserSocket* pSocket, User *pUser, WCHAR* wStr);
};

bool UserCommandExItemLock(User *pUser, const WCHAR* wParams);

extern CUserCommandEx g_UserCommandEx;