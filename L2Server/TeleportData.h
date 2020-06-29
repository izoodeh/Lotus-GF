#pragma once


class CTeleportData
{
	BOOL m_Enabled;
	BOOL m_AllowPK;
	BOOL m_AllowGuilty;
	BOOL m_AllowCombat;
	BOOL m_AllowRemote;

	map<UINT, TeleportInfo> m_Teleports;
	void LoadData();
public:
	CTeleportData();
	~CTeleportData();
	void Init();
	void Handle(User *pUser, UINT teleportId, bool execute = false);
	bool HandleReply(User *pUser, UINT messageId, UINT reply);
	inline BOOL IsEnabled() { return m_Enabled; };
	inline BOOL IsAllowRemote() { return m_AllowRemote; };
};

extern CTeleportData g_TeleportData;