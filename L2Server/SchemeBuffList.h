#pragma once

class CSchemeBuffList
{
	vector<UINT> m_Buffs[2];
	UINT m_Limit;
	UINT m_Dbid;
public:
	void Init( int max, UINT databaseId);
	void Add(UINT skillClassId, bool summon = false);
	void Clear( bool summon = false );
	void GetData(vector<UINT>& buffs, bool summon = false);
};