#include "StdAfx.h"
#include "KillReuseDb.h"

CKillReuseDb::CKillReuseDb()
{
	SetName(L"Kill Reuse Db");
}

CKillReuseDb::~CKillReuseDb()
{

}

UINT CKillReuseDb::SetGet(UINT killerDbId, UINT victimDbId, UINT killReuse, BOOL checkIPnMAC, IPnMAC killerIPnMAC, IPnMAC victimIPnMAC)
{
	UINT reuse = UINT_MAX;
	Lock();
	if(checkIPnMAC)
	{
		KillReuseMapEx::iterator Iter = mDataEx.find(killerDbId);
		if(Iter!=mDataEx.end())
		{
			vector<KillReuseDataEx>& vKillReuse = Iter->second;
			for(vector<KillReuseDataEx>::iterator vIter = vKillReuse.begin();vIter!=vKillReuse.end();vIter++)
			{
				if(vIter->Part.uniqueId == victimIPnMAC)
				{
					UINT currentTime = _time32(NULL);
					if(currentTime > vIter->Part.killTimestamp)
					{
						vIter->Part.killTimestamp = currentTime + killReuse;
						reuse =  0;
						break;
					}else
					{					
						reuse = vIter->Part.killTimestamp - currentTime;
						break;
					}
				}
			}
			if(reuse == UINT_MAX )
			{
				KillReuseDataEx krd;
				krd.Part.uniqueId = victimIPnMAC;
				krd.Part.killTimestamp = _time32(NULL) + killReuse;
				vKillReuse.push_back(krd);
				reuse = 0;
			}
		}else
		{
			KillReuseDataEx krd;
			krd.Part.uniqueId = victimIPnMAC;
			krd.Part.killTimestamp = _time32(NULL) + killReuse;

			vector<KillReuseDataEx> vKillReuse;
			vKillReuse.push_back(krd);

			mDataEx.insert(pair<UINT, vector<KillReuseDataEx>>(killerDbId, vKillReuse));

			reuse = 0;
		}
	}
	KillReuseMap::iterator Iter = mData.find(killerDbId);
	if(Iter!=mData.end())
	{
		vector<KillReuseData>& vKillReuse = Iter->second;
		for(vector<KillReuseData>::iterator vIter = vKillReuse.begin();vIter!=vKillReuse.end();vIter++)
		{
			if(vIter->Part.databaseId == victimDbId)
			{
				UINT currentTime = _time32(NULL);
				if(currentTime > vIter->Part.killTimestamp)
				{
					vIter->Part.killTimestamp = currentTime + killReuse;
					if(reuse == UINT_MAX)
					{
						reuse =  0;
					}
					break;
				}else
				{					
					UINT newReuse = vIter->Part.killTimestamp - currentTime;
					if(reuse == UINT_MAX || newReuse > reuse)
					{
						reuse = newReuse;
					}
					break;
				}
			}
		}
		if(reuse == UINT_MAX )
		{
			KillReuseData krd;
			krd.Part.databaseId = victimDbId;
			krd.Part.killTimestamp = _time32(NULL) + killReuse;
			vKillReuse.push_back(krd);
			reuse = 0;
		}
	}else
	{
		KillReuseData krd;
		krd.Part.databaseId = victimDbId;
		krd.Part.killTimestamp = _time32(NULL) + killReuse;

		vector<KillReuseData> vKillReuse;
		vKillReuse.push_back(krd);

		mData.insert(pair<UINT, vector<KillReuseData>>(killerDbId, vKillReuse));

		reuse = 0;
	}

	Unlock();

	return reuse;
}