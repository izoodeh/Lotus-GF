#include "StdAfx.h"
#include "IOCriticalSection.h"

CIOCriticalSection::CIOCriticalSection()
{
	typedef CIOCriticalSection* (*f)(CIOCriticalSection*);
	f(0x476040L)(this);
}

CIOCriticalSection::~CIOCriticalSection()
{
	typedef void (*f)(CIOCriticalSection*);
	f(0x476140L)(this);
}

void CIOCriticalSection::Lock(const WCHAR* file, int line)
{
	typedef void (*f)(CIOCriticalSection*, const WCHAR*, int);
	f(0x4762FCL)(this, file, line);
}

void CIOCriticalSection::Unlock(const WCHAR* file, int line)
{
	typedef void (*f)(CIOCriticalSection*, const WCHAR*, int);
	f(0x476360L)(this, file, line);
}