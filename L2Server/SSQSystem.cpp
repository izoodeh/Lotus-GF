#include "stdafx.h"
#include "SSQSystem.h"

CSSQSystem* CSSQSystem::GetInstance()
{
	typedef CSSQSystem* (*f)();
	return f(0x85A450L)();
}