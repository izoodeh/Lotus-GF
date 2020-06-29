#pragma once
#include "MemoryObject.h"
#include "SharedRWLock.h"

class CSSQSystem : MemoryObject
{
public:
	/* 18 */ CYieldLock lock;
	/* 24 */ UINT _unkn24;
	/* 28 */ HANDLE hEvent;
	/* 30 */ LPVOID _unkn30;
	/* 38 */ CSharedSSQInfo *pSD;

	static CSSQSystem* GetInstance();
};