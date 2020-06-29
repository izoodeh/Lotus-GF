#include "StdAfx.h"
#include "OlympiadUser.h"

bool COlympiadUser::IsInside()
{
	typedef bool(*f)(COlympiadUser*);
	return f(0x7695A0L)(this);
}

void COlympiadUser::OnLeaveWorld()
{
	typedef void(*f)(COlympiadUser*);
	f(0x774D88L)(this);
}