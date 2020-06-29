#pragma once

struct AIOSkill
{
	INT32 skillId;
	INT32 level;
	INT32 customTime;
	bool partyBuff;
	AIOSkill() : skillId(0), level(0), customTime(0), partyBuff(false) {}
};

enum AIOType
{
	AIONone = 0,
	AIOTimeLimited = 1,
	AIOInfinity = 2
};

struct AIOData
{
	UINT id;
	AIOType type;
	UINT expireTime;
	INT32 requiredLevel;
	ItemReq requiredItem;
	ItemReq giftItem;
	UINT nicknameColor;
	vector<AIOSkill> skills;
};

class CAllInOne
{
	bool m_Enabled;
	bool m_UseOutsidePeacezone;
	bool m_LeavePeacezone;
	bool m_CanUseSubclass;
	UINT m_TotalSkills;
	vector<AIOData*> m_AIO;
	void LoadData();
	AIOData* Get(UINT aioId);
	void AcquireSkills(User *pUser, UINT aioId);
	void DeleteSkills(User *pUser);
public:
	CAllInOne();
	void Init();
	inline bool Enabled() { return m_Enabled; }
	inline bool UseOutsidePeacezone() { return m_UseOutsidePeacezone; }
	inline bool LeavePeacezone() { return m_LeavePeacezone; }
	inline bool CanUseSubclass() { return m_CanUseSubclass; }
	bool CanUse(User *pUser, int skillId);
	INT32 GetAbnormalTime(User *pUser, INT32 skillId);
	void Acquire(User *pUser, UINT aioId);
	void Remove(User *pUser);
	void OnTimerExpired(User *pUser);
	void OnDBLoad(User *pUser, UINT aioId, time_t startTime);

};

extern CAllInOne g_AllInOne;