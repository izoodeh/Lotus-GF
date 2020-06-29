#include "stdafx.h"
#include "VoteReward.h"

CVoteReward g_VoteReward;

CVoteReward::CVoteReward() : m_enabled(FALSE)
{
	InitializeCriticalSection(&m_lock);
}

CVoteReward::~CVoteReward()
{
	DeleteCriticalSection(&m_lock);
}

void CVoteReward::Init()
{
	guard;
	Lock();
	m_votePages.clear();
	m_enabled = FALSE;
	LoadINI();
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "Vote Reward is enabled - loaded [%d] vote pages", m_votePages.size());
	}
	Unlock();
	unguard;
}

void CVoteReward::LoadINI()
{
	const TCHAR* section = _T("VoteSystem");
	TCHAR file[260];
	GetCurrentDirectory(MAX_PATH,file);
	lstrcat(file, TEXT("\\Vote.ini"));
	m_enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, file);
	TCHAR temp[8192];
	if(GetPrivateProfileString(section, _T("SectionList"), 0, temp, 8192, file))
	{
		tstringstream sstr;
		sstr << temp;
		TCHAR section[8192];
		while( sstr >> section)
		{
			CVotePage *lpVotePage = new CVotePage();
			if(lpVotePage)
			{
				lpVotePage->Init(section);
				m_votePages.push_back(lpVotePage);
			}
		}
	}
}

void CVoteReward::TimerExpired()
{
	guard;
	if(m_enabled)
	{
		Lock();
		for(list<CVotePage*>::iterator Iter = m_votePages.begin(); Iter != m_votePages.end(); Iter++)
		{
			CVotePage *pPage = (*Iter);
			if(pPage)
			{
				pPage->TimerExpired();
			}
		}
		Unlock();
	}
	unguard;
}