#include "StdAfx.h"
#include "PCTrap.h"

User* CPCTrap::GetUserOrMaster()
{
	typedef User* (*f)(CPCTrap*);
	return f(0x88065CL)(this);
}