#include "StdAfx.h"
#include "Feature.h"

CFeature::CFeature()
{
	InitializeCriticalSection(&m_lock);
	wcscpy_s(m_featureName, 64, L"Feature Base");
};

CFeature::~CFeature()
{
	DeleteCriticalSection(&m_lock);
}

void CFeature::SetName(const WCHAR *name)
{
	Lock();
	wcscpy_s(m_featureName, 64, name);
	Unlock();
}

const WCHAR* CFeature::GetName() const
{
	return m_featureName;
}