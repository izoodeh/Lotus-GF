#include "StdAfx.h"
#include "Summon.h"

User* CSummon::GetMaster()
{
	if(this)
	{
		typedef User* (*f)(CSummon*);
		return f(0x7B1000L)(this);
	}
	return 0;
}