#pragma once

class CGuard
{
public:
	CGuard();
	virtual void NullSub() {};
	virtual void NullSub2() {};
	virtual ~CGuard();
	virtual void Init();
	virtual bool DetectIGBot();
	virtual bool NullSub3() { return true; };
	virtual bool ValidateRunningApps();
	virtual bool ValidateRunningAppsEx();
};

class CRequestExit
{
public:
	CRequestExit();
	virtual ~CRequestExit();
};

class CInitializeExt
{
	map<int, int> mData;
	CRITICAL_SECTION _lock;
public:
	CInitializeExt();
	virtual ~CInitializeExt();
	virtual bool Init();
	virtual map<int, int>& GetMap();
};