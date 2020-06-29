#pragma once
#include "EventMatch.h"

class CEventMatchMgr
{
public:
	/* 00 */ LPVOID _unkn00[2];	//lock
	/* 10 */ map<UINT, CEventMatch*> mData;
	CEventMatchMgr();
	~CEventMatchMgr();
	static CEventMatchMgr* GetInstance();
	void Lock(const WCHAR* file, int line);
	void Unlock(const WCHAR* file, int line);
};