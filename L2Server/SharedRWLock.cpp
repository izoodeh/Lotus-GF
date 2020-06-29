#include "StdAfx.h"
#include "SharedRWLock.h"

void CSharedRWLock::SetSpinCount(DWORD dwSpinCount)
{
	typedef void (*f)(CSharedRWLock*, DWORD);
	f(0x813F84L)(this, dwSpinCount);
}

void CSharedRWLock::ReadLock(const WCHAR* file, int line, bool bConcession)
{
	typedef void (*f)(CSharedRWLock*, const WCHAR*, int, bool);
	f(0x814284L)(this, file, line, bConcession);
}

void CSharedRWLock::WriteLock(const WCHAR* file, int line)
{
	typedef void (*f)(CSharedRWLock*, const WCHAR*, int);
	f(0x8143DCL)(this, file, line);
}

void CSharedRWLock::Done(const WCHAR *file, int line)
{
	typedef void(*f)(CSharedRWLock*, const WCHAR*, int);
	f(0x814514L)(this, file, line);
}

void CYieldLock::Enter(const WCHAR *file, int line)
{
	typedef void (*f)(CYieldLock*, const WCHAR*, int);
	f(0x97E9D4L)(this, file, line);
}

void CYieldLock::Leave(const WCHAR* file, int line)
{
	typedef void (*f)(CYieldLock*, const WCHAR*, int);
	f(0x97EA74L)(this, file, line);
}

void CRWLock::Acquire(const WCHAR *file, int line)
{
	typedef void(*f)(CRWLock*, const WCHAR*, int);
	f(0x806844L)(this, file, line);
}

void CRWLock::Release()
{
	typedef void(*f)(CRWLock*);
	f(0x8069D4L)(this);
}