#pragma once

class CAutoLoot : public CFeature
{
	BOOL m_enabled;
	BOOL m_affectParty;
	BOOL m_affectRaidBoss;
	BOOL m_activeByDefault;
public:
	CAutoLoot();
	~CAutoLoot();
	void Init();
	bool Handle(CCreature* pCreature, CCreature* pEnemy, int itemId, INT64 count);
	inline BOOL IsEnabled() { return m_enabled; };
	inline BOOL IsActiveByDefault() { return m_activeByDefault; };
};

extern CAutoLoot g_AutoLoot;