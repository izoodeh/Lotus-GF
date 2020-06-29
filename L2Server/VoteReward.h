#pragma once

#include "VotePage.h"

typedef list<CVotePage*> VotePageList;

class CVoteReward
{
protected:
	CRITICAL_SECTION m_lock;
private:
	inline void Lock() { EnterCriticalSection(&m_lock); }
	inline void Unlock() { LeaveCriticalSection(&m_lock); }

	BOOL m_enabled;
	VotePageList m_votePages;
	void LoadINI();
public:
	CVoteReward();
	~CVoteReward();
	void Init();
	void TimerExpired();
};

extern CVoteReward g_VoteReward;

