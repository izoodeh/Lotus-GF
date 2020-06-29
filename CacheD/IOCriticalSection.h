#pragma once

class CIOCriticalSection
{
	CRITICAL_SECTION _lock;
	BOOL inUse;
public:
	CIOCriticalSection();
	~CIOCriticalSection();
	void Lock(const WCHAR* file = __FILEW__, int line = __LINE__);
	void Unlock(const WCHAR* file = 0, int line = 0);
};