#pragma once

class CIOBufferExPool
{
public:
	/* 00 */ LONG inUse;
	/* 04 */ UINT maxCount;
	/* 08 */ LPVOID _unkn08;
	/* 10 */ CIOBufferEx* lpPool;
	/* 18 */ INT64 freeCount;
	/* 20 */ INT64 usedCount;
};

class CIOBufferPool
{
public:
	/* 00 */ LONG inUse;
	/* 04 */ UINT maxCount;
	/* 08 */ LPVOID _unkn08;
	/* 10 */ CIOBuffer* lpPool;
	/* 18 */ INT64 freeCount;
	/* 20 */ INT64 usedCount;

};

void IOBufferExPoolInit();
CIOBufferEx* IOBufferExAllocate();
void IOBufferExRelease(CIOBufferExPool* lpPool, CIOBufferEx* pBuffer);

void IOBufferPoolInit();
CIOBuffer* IOBufferAllocate();
void IOBufferRelease(CIOBufferPool* lpPool, CIOBuffer* pBuffer);

void IOBufferPoolAtExit();
void IOBufferExPoolAtExit();