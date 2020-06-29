#pragma once

struct NpcModData
{
	INT32 itemId;
	INT32 requiredItemId;
	double normalChance;
	INT32 normalMinCount;
	INT32 normalMaxCount;
	double raidBossChance;
	INT32 raidBossMinCount;
	INT32 raidBossMaxCount;
	BOOL affectAllNpcs;
	vector<INT32> affectedNpc;
	NpcModData() : itemId(0), requiredItemId(0), normalChance(0.0), normalMinCount(0), normalMaxCount(0), raidBossChance(0), raidBossMinCount(0), raidBossMaxCount(0), affectAllNpcs(FALSE) {}
};

class CNpcMod : public CFeature
{
	BOOL m_Enabled;
	BOOL m_DisableLevelPenalty;
	vector<NpcModData> m_DropList;
	void LoadINI();
public:
	CNpcMod();
	~CNpcMod();
	void Init();
	void OnDie(CNpc *pNpc, CCreature *pEnemy);
};

extern CNpcMod g_NpcMod;