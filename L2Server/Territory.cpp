#include "stdafx.h"
#include "Territory.h"

bool CTerritory::IsInside(FVector &pos, bool param)
{
	typedef bool (*f)(CTerritory*, FVector&, bool);
	return f(0x86C224L)(this, pos, param);
}