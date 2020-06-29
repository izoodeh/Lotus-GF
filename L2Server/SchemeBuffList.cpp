#include "StdAfx.h"
#include "SchemeBuffList.h"
#include "SkillDB.h"

void CSchemeBuffList::Init( int max , UINT databaseId)
{
	Clear();
	Clear(true);
	m_Dbid = databaseId;
	m_Limit = max;
};

void CSchemeBuffList::Add(UINT skillClassId, bool summon)
{
	guard;
	if(skillClassId > 0)
	{
		for(vector<UINT>::iterator it = m_Buffs[summon].begin(); it!=m_Buffs[summon].end();it++)
		{
			if((*it) == skillClassId)
			{
				m_Buffs[summon].erase(it);
				break;
			}
		}
		m_Buffs[summon].push_back(skillClassId);
		while(m_Buffs[summon].size() > m_Limit && m_Limit > 0)
		{
			m_Buffs[summon].erase(m_Buffs[summon].begin());
		}
	}
	unguard;
};

void CSchemeBuffList::Clear(bool summon)
{
	m_Buffs[summon].clear();
}

void CSchemeBuffList::GetData(vector<UINT>& buffs, bool summon)
{
	for(UINT n=0;n<m_Buffs[summon].size();n++)
	{
		buffs.push_back(m_Buffs[summon][n]);
	}
}