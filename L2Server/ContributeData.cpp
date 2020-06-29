#include "stdafx.h"
#include "ContributeData.h"

void CContributeData::Clear()
{
	typedef void (*f)(CContributeData*);
	f(0x75AE74L)(this);
}