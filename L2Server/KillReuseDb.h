#pragma once

typedef map<UINT, vector<KillReuseData>> KillReuseMap;
typedef map<UINT, vector<KillReuseDataEx>> KillReuseMapEx;

class CKillReuseDb : public CFeature
{
	KillReuseMap mData;
	KillReuseMapEx mDataEx;
public:
	CKillReuseDb();
	~CKillReuseDb();
	UINT SetGet(UINT killerDbId, UINT victimDbId, UINT killReuse, BOOL checkMACnIP, IPnMAC killerIPnMAC, IPnMAC victimIPnMAC);
};