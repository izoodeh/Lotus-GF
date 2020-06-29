#pragma once

class CFeature
{
	WCHAR m_featureName[64];
	CRITICAL_SECTION m_lock;
public:
	inline void Lock() { EnterCriticalSection(&m_lock); }
	inline void Unlock() { LeaveCriticalSection(&m_lock); }
	CFeature();
	virtual ~CFeature();
	const WCHAR* GetName() const;
	void SetName(const WCHAR* name);
};