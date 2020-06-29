#pragma once
#include "MemoryObject.h"
#include "Territory.h"

//1F8 - areaType
class CArea : public CBaseObject
{
public:
	/* 18 */ LPVOID _unkn18[9];
	/* 60 */ CTerritory range;

	bool IsOn(UINT instantZoneId);
	bool IsBannedPoint(FVector& pos, bool param = true);
	bool IsInside(CCreature *pCreature);
};

class CAreaDB
{
	LPVOID m_orgInstance;
public:
	CAreaDB();
	~CAreaDB();
	CArea* FindArea(const WCHAR* wName);
};

extern CAreaDB g_AreaDB;