#include "stdafx.h"
#include "Area.h"

CAreaDB g_AreaDB;

bool CArea::IsOn(UINT instantZoneId)
{
	typedef bool (*f)(CArea*, UINT);
	return f(0x436C5CL)(this, instantZoneId);
}

bool CArea::IsBannedPoint(FVector& pos, bool param)
{
	typedef bool (*f)(CArea*, FVector&, bool);
	return f(0x43B14CL)(this, pos, param);
}

bool CArea::IsInside(CCreature *pCreature)
{
	if(IsOn(pCreature->instantZoneId))
	{
		if(range.IsInside(pCreature->pSD->pos))
		{
			if(!IsBannedPoint(pCreature->pSD->pos))
			{
				return true;
			}
		}
	}
	return false;
}

CAreaDB::CAreaDB()
{
	m_orgInstance = (LPVOID)0xED0C20L;
}

CAreaDB::~CAreaDB()
{

}

CArea* CAreaDB::FindArea(const WCHAR *wName)
{
	typedef CArea*(*f)(LPVOID, const WCHAR*);
	return f(0x4394ECL)(m_orgInstance, wName);
}