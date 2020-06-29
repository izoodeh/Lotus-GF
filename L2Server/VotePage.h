#ifndef VOTEPAGE_H
#define VOTEPAGE_H

class CVotePage
{
protected:
	CRITICAL_SECTION m_lock;
private:	
	inline void Lock() { EnterCriticalSection(&m_lock); }
	inline void Unlock() { LeaveCriticalSection(&m_lock); }
	UINT m_id;
	BOOL m_enabled;
	INT32 m_requiredPoints;
	INT32 m_rewardId;
	INT32 m_rewardCount;
	BOOL m_multiplyReward;
	BOOL m_rewardHwidCheck;
	UINT m_interval;
	string m_urlHost;
	string m_urlPath;
	string m_startPointString;
	string m_endPointString;
	TCHAR m_section[8192];
	tstring m_intervalAnnounce;
	tstring m_intervalAnnounceEx;
	tstring m_rewardAnnounce;

	BOOL m_setBasePoints;
	INT32 m_basePoints;
	INT32 m_newPoints;
	time_t m_checkTime;
	void LoadINI(const TCHAR* section);
public:
	CVotePage();
	~CVotePage();
	void Init(const TCHAR* section);
	void TimerExpired();
	void HandleNewPoints(INT32 newPoints);
	static void CheckVotePoints(LPVOID param);
	INT32 GetVotePoints();
};

#endif