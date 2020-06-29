#pragma once
#include "ChampionshipMatch.h"

namespace Championship
{
	class CChampionship : public CFeature
	{
		/* Config */
		BOOL m_enabled;
		INT32 m_teleportTime;
		IVector m_teleportPos[2][3];
		BOOL m_dispelAll;
		INT32 m_prepareTime;
		INT32 m_doorTime;
		list<wstring> m_doors;
		INT32 m_fightTime;
		INT32 m_rewardId;
		INT32 m_rewardCount;
		INT32 m_teleportBackTime;
		list<INT32> m_blockedSkill;
		list<INT32> m_blockedItem;
		bool m_blockedCrystalType[10];
		BOOL m_blockAugmentation;
		BOOL m_blockAttribute;
		INT32 m_maxArmorEnchant;
		INT32 m_maxWeaponEnchant;
		INT32 m_autoReloadDB;
		time_t m_reloadDbTime;
		/* Config End */

		TeamMap m_team;
		MatchMap m_match;
		BOOL m_loadedDB;

		CMatch *m_pMatch;

		void LoadINI();
	public:
		CChampionship();
		~CChampionship();
		bool IsBlockedSkill(INT32 skillId);
		bool IsBlockedItem(INT32 itemId);
		bool IsBlockedCrystalType(UINT crystalType);
		void Init();
		void AddTeam(UINT id, wstring name, UINT charId1, UINT charId2, UINT charId3, INT32 points, INT32 winCount,	INT32 loseCount, INT32 drawCount, INT32 killCount, INT32 deathCount, UINT teamCount);
		void AddMatch(UINT id, UINT state, UINT startTime, UINT teamId1, UINT teamId2, UINT winnerId, UINT matchCount);
		void TimerExpired();
		void OnEnterWorld(User *pUser);
		void OnLeaveWorld(User *pUser);
		void OnDie(User *pUser);
		void ValidateItems(User *pUser);
		bool CanUse(CItem *pItem);
	};
};

extern Championship::CChampionship g_Championship;