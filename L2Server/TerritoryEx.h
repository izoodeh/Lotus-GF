#pragma once

class CDispelAllTerritory;
class CPvPRewardTerritory;
class CBlockItemTerritory;
class CDispelAbnormalTerritory;

class CTerritoryEx
{
public:
	CDispelAllTerritory *SafeCastDispelAll();
	CPvPRewardTerritory *SafeCastPvPReward();
	CBlockItemTerritory *SafeCastBlockItem();
	CDispelAbnormalTerritory *SafeCastDispelAbnormal();
private:
	FVector m_MinPos;
	FVector m_MaxPos;
	TerritoryType m_Type;
	bool m_Extend;
	vector<FVector> m_Points;

	bool CrossHalfLine(FVector point, FVector p1, FVector p2);
public:
	CTerritoryEx();
	virtual ~CTerritoryEx() {};
	void Set(double x, double X, double y, double Y, double z, double Z, TerritoryType type);
	UINT inline GetType() { return m_Type; };
	bool IsInside(double x, double y, double z);
	bool IsInside(FVector point);
	void AddPoint(FVector point);
	void AddZRange(double z1, double z2);
	inline void SetType(TerritoryType value) { m_Type = value; };
};

class CDispelAbnormalTerritory : public CTerritoryEx
{
	UINT m_AbnormalType;
public:
	CDispelAbnormalTerritory() : m_AbnormalType(0) {};
	inline void SetAbnormalType(UINT type) { m_AbnormalType = type; };
	inline UINT GetAbnormalType() { return m_AbnormalType; };
};

class CBlockItemTerritory : public CTerritoryEx
{
	vector<INT32> m_Items;
public:
	CBlockItemTerritory() {};
	~CBlockItemTerritory() {};
	void AddItem(INT32 itemId)
	{
		m_Items.push_back(itemId);
	}
	bool IsBlocked(INT32 itemId)
	{
		for(UINT n=0;n<m_Items.size();n++)
		{
			if(m_Items[n] == itemId)
			{
				return true;
			}
		}
		return false;
	}
};

struct PvPRewardItem
{
	int itemId;
	int minAmount;
	int maxAmount;
	double chance;
	BOOL stealFromVictim;
	PvPRewardItem() : itemId(0), minAmount(0), maxAmount(0), chance(100), stealFromVictim(FALSE) {};
};

class CPvPRewardTerritory : public CTerritoryEx
{
	vector<PvPRewardItem> m_Items;
	BOOL m_CheckHWID;
	INT32 m_KillReuse;
	INT32 m_MinLevel;
	INT32 m_MaxLevel;

public:
	CPvPRewardTerritory();
	inline void SetMinLevel(int value) { m_MinLevel = value; };
	inline void SetMaxLevel(int value) { m_MaxLevel = value; };
	inline void SetCheckingHWID(BOOL value) { m_CheckHWID = value; };
	inline void SetKillReuse(int value) { m_KillReuse = value; };
	void AddReward(int itemId, int minAmount, int maxAmount, double chance, BOOL stealFromVictim);
	void Handle(User *pUser, User *pAttacker);
};


class CDispelAllTerritory : public CTerritoryEx
{
	BOOL m_OnEnterOnly;
public:
	CDispelAllTerritory() : m_OnEnterOnly(FALSE) {};
	void SetOnEnterOnly(BOOL value) { m_OnEnterOnly = value; };
	inline BOOL IsOnEnterOnly() { return m_OnEnterOnly; };
};
