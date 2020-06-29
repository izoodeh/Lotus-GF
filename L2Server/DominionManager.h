#pragma once
#include "DBSocket.h"

class CDominionManager
{
	BOOL m_Enabled;
	PVOID m_Instance;
	PUINT m_GlobalState;
	PUINT m_PrevEndTime;
	PUINT m_StartTime;
	PUINT m_Period;
	PUINT m_ElapsedTime;
	tm m_FirstSiegeTM;
	UINT m_FirstSiegeTime;
	UINT m_CycleTime;
	BOOL m_CheckForRegistration;
	void LoadINI();
public:
	CDominionManager();
	void Init();
	void ChangeState(UINT state, bool dbSave);
	void SetSiegeEndTime(UINT value, bool dbSave);
	void SetNextSiegeTime(UINT value, bool dbSave);
	void StartTimer();
	UINT CheckStartDominionRegistration(UINT param = 0, bool paramEx = true);
	static UINT AdjustNextSiegeTime();
	inline UINT GetPrevEndTime() { return (*m_PrevEndTime); };
	inline void SetPeriod(UINT value) { (*m_Period) = value; };
	inline UINT GetPeriod() { return (*m_Period); };
	inline void SetElapsedTime(UINT value) { (*m_ElapsedTime) = value; };
	inline UINT GetElapsedTime() { return (*m_ElapsedTime); };
	inline void SetStartTime(UINT value) { (*m_StartTime) = value; };
	inline UINT GetStartTime() { return (*m_StartTime); };
	inline void SetGlobalState(UINT value) { (*m_GlobalState) = value; };
	inline UINT GetGlobalState() { return (*m_GlobalState); };
	static bool ReplyLoadDominionSiege(CDBSocket *pSocket, const unsigned char* packet);
	static void TimerExpired(PVOID lpInstance, UINT id);
};

extern CDominionManager g_DominionManager;