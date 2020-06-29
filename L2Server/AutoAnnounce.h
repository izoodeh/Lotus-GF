#pragma once

struct AutoAnnounceData
{
	wstring announce;
	BOOL onEnterWorld;
	time_t interval;	//in seconds
	time_t nextAnnounceTime;
};

class CAutoAnnounce
{
	CSLock m_lock;
	BOOL m_enabled;
	BOOL m_reload;
	vector<AutoAnnounceData> m_data;
	wstring m_PvPMessage;
	INT32 m_PvPMessageType;
	wstring m_PvPMessenger;
	wstring m_PKMessage;
	INT32 m_PKMessageType;
	wstring m_PKMessenger;
	void LoadINI();
public:
	CAutoAnnounce();
	~CAutoAnnounce();
	inline void SetReload() { m_reload = TRUE; };
	void Init();
	void TimerExpired();
	void OnEnterWorld(User *pUser);
	void PKMessage(User *pUser, User *pEnemy);
	void PvPMessage(User *pUser, User *pEnemy);
};

extern CAutoAnnounce g_AutoAnnounce;