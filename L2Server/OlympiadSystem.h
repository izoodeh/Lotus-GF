#pragma once

struct OlympiadStatus
{
	UINT step;
	BOOL competitionEnabled;
};

struct CompetitionTime
{
	INT32 startHour;
	INT32 endHour;
};

class COlympiadSystem : public CFeature
{
	INT32 m_HWIDLimit;
	INT32 m_HWIDLimitMsgId;
	INT32 m_MaxArmorEnchant;
	INT32 m_MaxWeaponEnchant;
public:
	COlympiadSystem();
	~COlympiadSystem();
	void Init();
	static int GetTransferingPoint(LPVOID lpInstance, GameRuleType type, int team1Points, int team2Points);
	static int GetPenaltyPoint(LPVOID lpInstance, GameRuleType type, int points);
	static bool GetWantedStatus(LPVOID lpInstance, UINT seasonStart, OlympiadStatus& os);
	static bool CanAddOlympiadUser(User *pUser, LPVOID lpOlympiadEntry, int& param, int& messageId);
	void ValidateEquippedItems(User *pUser);
	bool CanUse(CItem *pItem);
	inline INT32 GetHWIDLimit() { return m_HWIDLimit; };
	inline INT32 GetHWIDLimitMsgId() { return m_HWIDLimitMsgId; };
};

extern COlympiadSystem g_OlympiadSystem;