#pragma once

enum FactionType
{
	FactionNone = 0,
	FactionBoss = 1
};

struct FactionUser
{
	User *pUser;
	TeamType team;
};

class CBossFaction;

class CFaction
{
	friend class CFactionSystem;
	friend class CBossFaction;

	UINT factionId;
	FactionType type;
	wstring factionName;
	CRITICAL_SECTION _lock;
	map<UINT, FactionUser> mUsers;
	UINT minPlayerCount;
	UINT maxPlayerCount;
	INT32 minPlayerLevel;
	UINT askMessageId;
	TeamType winnerTeam;
	bool deleteWaiting;
	bool started;
	BOOL closeColiseum;
	inline void Lock() { EnterCriticalSection(&_lock); };
	inline void Unlock() { LeaveCriticalSection(&_lock); };
public:
	CFaction();
	virtual ~CFaction();
	CBossFaction* CastBossFaction();
	void SetName(wstring wName);
	wstring GetName();
	UINT GetId();
	FactionType GetType();
	void SetWinner(TeamType team);
	bool AddUser(User *pUser, TeamType team);
	void DelUser(User *pUser);
	void Broadcast(TeamType team, const char* format, ...);
	void CloseColiseum();
	void OpenColiseum();
};