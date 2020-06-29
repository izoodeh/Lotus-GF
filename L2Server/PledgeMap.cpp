#include "StdAfx.h"
#include "PledgeMap.h"

bool CPledgeMap::ExistKey(int key)
{
	typedef bool (*f)(CPledgeMap*, int);
	return f(0x7CF168L)(this, key);
}