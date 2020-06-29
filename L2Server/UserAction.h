#pragma once

class UserAction
{
public:
	static void Init();
	static void OnInit(User *pUser); /* inside constructor */
	static User* CreateUser(UINT size);
	static void Destructor(User *pUser);
	static bool OnRestartPointPacket(User *pUser, RestartPoint restartPoint);
	static void EnterWorld(User *pUser);
	static void LeaveWorld(User *pUser);
	static void OnChangeSubjobEnd(User* pUser, bool param);
	static void OnChangeSubjobBegin(User* pUser);
	static void OnTimerExpired(User *pUser, int id);
	static void OnLevelChanged(User* pUser);
	static bool OnTeleportToLocation(User *pUser, int x, int y, int z, UINT instantZoneId, UINT param);
	static bool Die(User *pUser, CCreature *pEnemy);
	static INT64 ExpInc(User *pUser, INT64 newExp, bool notAffectKarma);
	static UINT GetRelationTo(User *pUser, User *pOther);
	static bool CanAcquirePledgeSkill(LPVOID lpInstance, int skillId, int level, User *pUser);
	static void StealInfectedSkills(User *pUser, int slotType, int count);
	static bool EquipItem(User *pUser, CItem *pItem, SlotType slotType);
	static bool BasicAction(User *pUser, int id, bool ctrl, bool shift, UINT stopType);
	static void SetKarma(User *pUser, int karma);
	static char GetEventMatchTeamId(User *pUser);
	static void DeleteYongmaAbnormalStatus(User *pUser, bool param);
	static bool UseItem(User *pUser, CItem *pItem, int param1, bool param2);
	static double ComputeExpBoost(User *pUser);
	static double ComputeSpBoost(User *pUser);
	static double ComputeDropBoost(User *pUser);
	static double ComputeSpoilBoost(User *pUser);
	static bool ChangeClass(User *pUser, int classId);
	static void ValidateHeroSkills(User *pUser);
	static void EscapeFromOlympiad(COlympiadUser *pOU);
	static void ReadyForMatch(COlympiadUser *pOU, FVector *lpPos);
	static UINT CheckDuelStartCondition(User *pUser, bool party);
	static void OnStartOlympiadMatch(User *pUser, const WCHAR* wSound);
	static void IncreaseKarma(User *pUser, bool withPk);
	static bool OnNpcCreateSubJobPacket(User *pUser, UINT npcId, UINT newClass);
	static bool OnNpcRenewSubJobPacket(User *pUser, UINT npcId, UINT subjobId, UINT newClass);
	static void IncreaseDuelCount(User *pUser);
	static bool IsCallPCEnabled(User *pUser, User *pTarget, PUINT lpResult);

};