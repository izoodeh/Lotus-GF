#pragma once

struct UserSkillReuse
{
	CSLock m_Lock;
	map<INT32, UINT> m_Reuse[5];	//0,1,2,3 - subjobId, 4 - static reuse (reuse_type - M or S [0/1]
	void Set(INT32 skillId, UINT endReuse, UINT subId, UINT reuseType);
	UINT Get(INT32 skillId, UINT subId);
};

struct BilingJob
{
	int productId;
	int count;
};

struct FactionData
{
	UINT factionId;
	TeamType team;
};

struct AutoConsumeInfo;

struct ItemAutoConsumeData
{
	INT32 itemId;
	UINT lastUseTick;
	AutoConsumeInfo *pInfo;
};

namespace TvT
{
	enum TeamType
	{
		TeamNone = 0,
		TeamBlue = 1,
		TeamRed = 2
	};

	enum UserStatus
	{
		UserNone = 0,
		UserRegistered = 1,
		UserPreparing = 2,
		UserFighting = 3,
		UserFinishing = 4
	};

	struct TvTUser
	{
		UserStatus status;
		TeamType team;
		IVector orgPos;
		IVector respawnPos;
		IVector fightPos;
		INT32 respawnTick;
		INT32 resetTimer;
		INT32 backToFightTick;
		BOOL friendlyFire;
		TvTUser()
		{
			Clear();
		}
		inline void Clear()
		{
			status = UserNone;
			team = TeamNone;
			respawnTick = 0;
			resetTimer = 0;
			friendlyFire = 0;
			orgPos.x = 0;
			orgPos.y = 0;
			orgPos.z = 0;
			backToFightTick = 0;
			fightPos.x = 0;
			fightPos.y = 0;
			fightPos.z = 0;
		}
	};
};

struct ChampionshipUser
{
	UINT matchId;
	UINT teamType;
	UINT state;
	IVector orgPos;
	void Clear()
	{
		matchId = 0;
		teamType = 0;
		state = 0;
	};
};

struct PvPData
{
	INT32 points;
	BOOL saveFlag;
	INT32 lastSeasonPoints;
	INT32 transferredPoints;
	BOOL topPlayer;
	BOOL receivedReward;
};

struct DungeonUser
{
	UINT id;
	time_t enterTime;
	INT32 kickoutTimer;
	INT32 resetTimer;
	BOOL canJoinInviteParty;
};

struct BuilderUser
{
	BOOL isRecordingRange;
	double rangeZone[3][2];
};

struct TeleportUser
{
	TeleportInfo ti;
	UINT timeoutTick;
};

struct AIOData;

struct AIOUser
{
	UINT aioId;
	time_t aioAcquireTime;
	AIOData *pData;
	AIOUser() : aioId(0), aioAcquireTime(0), pData(0) {}
};

class CExtUserData
{
public:
	BOOL offlineShop;
	DWORD chatTimestamp[18];
	CSchemeBuffList schemeBuffList;
	BOOL spawnProtection;
	BOOL activateSpawnProtection;
	time_t offlineShopTime;
	StoredAbnormalDataList lStoredAbnormal;
	DWORD deathTick;
	BOOL blockBuffEx;
	BOOL blockBuffExPump;
	UINT votePoints;
	UINT donatePoints;
	PvPData pvpData;
	BOOL blockExp;
	BOOL blockExpPump;
	DWORD updateDbTick;
	INT32 currentActivatedSkillId;
	INT32 currentActivatedSkillLevel;
	CRITICAL_SECTION _lock;
	BilingJob bilingJob;
	FactionData faction;
	ItemAutoConsumeData autoConsumeHp;
	ItemAutoConsumeData autoConsumeMp;
	ItemAutoConsumeData autoConsumeCp;
	double bonusExp;
	double bonusSp;
	double bonusDrop;
	double bonusSpoil;
	UINT kickTick;
	UINT teleportTick;
	BOOL autoLoot;

	ChampionshipUser championshipUser;
	TvT::TvTUser tvtUser;
	DungeonUser dungeonUser;
	BuilderUser builderUser;
	BOOL isItemDataLock;
	INT32 isCrystalEnchantFail;
	//Territory data helpers
	TerritoryData territoryData;
	BOOL dispelAllDone;
	BOOL unequipDone;
	BOOL inBattleZone;

	//Update data helpers
	volatile LONG userInfoChanged;
	volatile LONG charInfoChanged;

	TeleportUser teleportUser;
	UserSkillReuse *pUSR;
	AIOUser aioUser;
	IVector lastPeaceZonePos;
	time_t enterWorldTime;

	inline void Lock() { EnterCriticalSection(&_lock); };
	inline void Unlock() { LeaveCriticalSection(&_lock); };
	CExtUserData();
	~CExtUserData();
};