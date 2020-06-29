#pragma once

namespace Championship
{
	enum TeamType
	{
		TeamNone = 0,
		TeamBlue = 1,
		TeamRed = 2
	};

	struct TeamData
	{
		UINT id;
		wstring name;
		TeamType type;
		UINT charId[3];
		INT32 points;
		INT32 winCount;
		INT32 loseCount;
		INT32 drawCount;
		INT32 killCount;
		INT32 deathCount;
	};
	typedef map<UINT, TeamData*> TeamMap;

	enum MatchState
	{
		MatchWaiting = 0,
		MatchTeleporting = 1,
		MatchPreparing = 2,
		MatchFighting = 3,
		MatchDone = 4,
		MatchTeleportBack = 5,
		MatchRelease = 6
	};

	enum UserState
	{
		UserNone = 0,
		UserTeleporting = 1,
		UserPreparing = 2,
		UserFighting = 3,
		UserTeleportingBack = 4
	};

	class CMatch
	{
		friend class CChampionship;
		UINT id;
		MatchState state;
		time_t startTime;
		UINT teamId[2];
		TeamData* teamData[2];
		UINT winnerId;

		UINT m_killCount[2];
		UINT m_deathCount[2];
		BOOL m_openDoor;
		time_t m_stateTime;
		map<UINT, User*> m_users;
		User *AddUser(UINT charId, TeamType team);
	public:
		CMatch();
		void Broadcast(TeamType team, const char* format, ...);
		void Init(INT32 teleportTime);
		~CMatch();
		void OnFinish(INT32 rewardId, INT32 rewardCount);
		void OnRelease();
		bool ValidateWinner(bool timeout = false);
	};

	typedef map<UINT, CMatch*> MatchMap;
};