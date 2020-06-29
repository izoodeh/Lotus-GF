#pragma once

typedef bool (*UserFunction)(User*);

class CUserDb: public CFeature
{
	map<UINT, User*> mByDatabaseId;
	map<UINT, User*> mByServerId;
	map<wstring, User*> mByName;
public:
	CUserDb();
	~CUserDb();
	void Add(User *pUser);
	void Del(User *pUser);
	User* FindByDatabaseId(UINT databaseId);
	User* FindByServerId(UINT serverId);
	User* FindByName(wstring wName);	/* names are stored in lowercase - towlower */
	void ExecuteForAllUsers(UserFunction callback);
	void KickOutFromColiseum(IVector pos);
	void KickOutFromRange(IVector pos, int minX, int maxX, int minY, int maxY);
	INT32 GetUserAmountByHWID(LPBYTE lpHWID);
	INT32 GetOlympiadUsersByHWID(LPBYTE lpHWID);
	INT32 GetTvTUsersByHWID(LPBYTE lpHWID);
	void GiveItemToAll(int itemId, int amount, bool checkHwid);
	void KickUsersByHWID(LPBYTE lpHWID);
};

extern CUserDb g_UserDb;
