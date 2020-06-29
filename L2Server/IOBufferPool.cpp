#include "StdAfx.h"
#include "IOBufferPool.h"
#include "HookManager.h"

CIOBufferExPool** lpIOBufferExPool = (CIOBufferExPool**)0x2308DC0;
CIOBufferPool** lpIOBufferPool = (CIOBufferPool**)0x2308E50;
CRITICAL_SECTION g_IOBufferExCriticalSection[32];
CRITICAL_SECTION g_IOBufferCriticalSection[32];

void IOBufferExPoolInit()
{

	for(int n=0;n<32;n++)
	{
		InitializeCriticalSection(&g_IOBufferExCriticalSection[n]);
	}

	WriteNOP(0x41B2B2, 14); //CIOBufferEx::Release
	WriteNOP(0x418F86, 6);

	WriteCall(0x41D75A, IOBufferExAllocate);
	WriteCall(0x508D45, IOBufferExAllocate);
	WriteCall(0x5094A0, IOBufferExAllocate);
	WriteCall(0x50A2BD, IOBufferExAllocate);
	WriteCall(0x5D16ED, IOBufferExAllocate);
	WriteCall(0x6BB687, IOBufferExAllocate);
	WriteCall(0x7C0CFC, IOBufferExAllocate);
	WriteCall(0x859274, IOBufferExAllocate);
	WriteCall(0x8592B1, IOBufferExAllocate);
	WriteCall(0x859509, IOBufferExAllocate);
	WriteCall(0x859546, IOBufferExAllocate);
	WriteCall(0x8599C1, IOBufferExAllocate);
	WriteCall(0x8A90DE, IOBufferExAllocate);

	WriteCall(0x41D7AD, IOBufferExRelease);
	WriteCall(0x421DC1, IOBufferExRelease);
	WriteCall(0x422841, IOBufferExRelease);
	WriteCall(0x44EB82, IOBufferExRelease);
	WriteCall(0x4C7613, IOBufferExRelease);
	WriteCall(0x5091B4, IOBufferExRelease);
	WriteCall(0x509290, IOBufferExRelease);
	WriteCall(0x5092F2, IOBufferExRelease);
	WriteCall(0x5094FB, IOBufferExRelease);
	WriteCall(0x509777, IOBufferExRelease);
	WriteCall(0x5097F2, IOBufferExRelease);
	WriteCall(0x509981, IOBufferExRelease);
	WriteCall(0x509A18, IOBufferExRelease);
	WriteCall(0x509D4A, IOBufferExRelease);
	WriteCall(0x5414EC, IOBufferExRelease);
	WriteCall(0x541B54, IOBufferExRelease);
	WriteCall(0x5421CE, IOBufferExRelease);
	WriteCall(0x6115CB, IOBufferExRelease);
	WriteCall(0x72060A, IOBufferExRelease);
	WriteCall(0x7F2E20, IOBufferExRelease);
	WriteCall(0x858CEC, IOBufferExRelease);
	WriteCall(0x858D57, IOBufferExRelease);
	WriteCall(0x858F00, IOBufferExRelease);
	WriteCall(0x858F99, IOBufferExRelease);
	WriteCall(0x85922E, IOBufferExRelease);
	WriteCall(0x859449, IOBufferExRelease);
	WriteCall(0x8595CD, IOBufferExRelease);
	WriteCall(0x85960E, IOBufferExRelease);
	WriteCall(0x859A1B, IOBufferExRelease);
	WriteCall(0x859A52, IOBufferExRelease);
	WriteCall(0x8A9131, IOBufferExRelease);
	WriteCall(0x8AEB62, IOBufferExRelease);
	WriteCall(0x8AEE2D, IOBufferExRelease);
	WriteCall(0x8CAE5F, IOBufferExRelease);
}

void IOBufferPoolInit()
{
	for(int n=0;n<32;n++)
	{
		InitializeCriticalSection(&g_IOBufferCriticalSection[n]);
	}

	WriteNOP(0x4567B2, 14);
	WriteNOP(0x4564CA, 6);

	WriteCall(0x456BF5, IOBufferAllocate);
	WriteCall(0x45735C, IOBufferAllocate);
	WriteCall(0x458291, IOBufferAllocate);
	WriteCall(0x651405, IOBufferAllocate);
	WriteCall(0x7EB235, IOBufferAllocate);

	WriteCall(0x457070, IOBufferRelease);
	WriteCall(0x45714C, IOBufferRelease);
	WriteCall(0x4571AE, IOBufferRelease);
	WriteCall(0x4573B7, IOBufferRelease);
	WriteCall(0x457633, IOBufferRelease);
	WriteCall(0x4576AE, IOBufferRelease);
	WriteCall(0x45783D, IOBufferRelease);
	WriteCall(0x4578D4, IOBufferRelease);
	WriteCall(0x457C06, IOBufferRelease);
}

void IOBufferPoolAtExit()
{
	for(int n=0;n<32;n++)
	{
		DeleteCriticalSection(&g_IOBufferCriticalSection[n]);
	}
}

void IOBufferExPoolAtExit()
{
	for(int n=0;n<32;n++)
	{
		DeleteCriticalSection(&g_IOBufferExCriticalSection[n]);
	}
}

CIOBufferEx* IOBufferExAllocate()
{
	guard;

	CIOBufferEx* pBuffer = 0;
	UINT threadIdx = GetThreadIndex();
	if(threadIdx < 32)
	{

		CIOBufferExPool* lpPool = lpIOBufferExPool[threadIdx];
		EnterCriticalSection(&g_IOBufferExCriticalSection[threadIdx]);
		try
		{
			lpPool->inUse = 1;
			typedef CIOBufferEx* (*f)();
			pBuffer = f(0x41CDB8L)();
			lpPool->inUse = 0;
		}catch(...)
		{
			g_Log.Add(CLog::Error, "[%s] Exception - ThreadIndex[%d] Pool[%p]", __FUNCTION__, threadIdx, lpPool);
		}
		LeaveCriticalSection(&g_IOBufferExCriticalSection[threadIdx]);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Too high current thread index[%d]!", __FUNCTION__, threadIdx);
		LPDWORD p = 0;
		(*p) = 0x666;
	}

	unguard;
	return pBuffer;
}

CIOBuffer* IOBufferAllocate()
{
	guard;

	CIOBuffer* pBuffer = NULL;
	UINT threadIdx = GetThreadIndex();
	if(threadIdx < 32)
	{	
		CIOBufferPool* lpPool = lpIOBufferPool[threadIdx];
		EnterCriticalSection(&g_IOBufferCriticalSection[threadIdx]);
		lpPool->inUse = 1;
		typedef CIOBuffer* (*f)();
		pBuffer = f(0x45691CL)();
		lpPool->inUse = 0;
		LeaveCriticalSection(&g_IOBufferCriticalSection[threadIdx]);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Too high current thread index[%d]!", __FUNCTION__, threadIdx);
		LPDWORD p = 0;
		(*p) = 0x666;
	}

	unguard;
	return pBuffer;
}

void IOBufferExRelease(CIOBufferExPool *lpPool, CIOBufferEx *pBuffer)
{
	guard;

	UINT threadIdx = pBuffer->poolThreadIdx;
	if(threadIdx < 32)
	{
		EnterCriticalSection(&g_IOBufferExCriticalSection[threadIdx]);
		lpPool->inUse = 1;
		typedef void(*f)(CIOBufferExPool*, CIOBufferEx*);
		f(0x41B2A0L)(lpPool, pBuffer);
		lpPool->inUse = 0;
		LeaveCriticalSection(&g_IOBufferExCriticalSection[threadIdx]);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Too high current thread index[%d]!", __FUNCTION__, threadIdx);
		LPDWORD p = 0;
		(*p) = 0x666;
	}
	unguard;
}

void IOBufferRelease(CIOBufferPool *lpPool, CIOBuffer *pBuffer)
{
	guard;

	UINT threadIdx = pBuffer->poolThreadIdx;
	if(threadIdx < 32)
	{
		EnterCriticalSection(&g_IOBufferCriticalSection[threadIdx]);
		lpPool->inUse = 1;
		typedef void(*f)(CIOBufferPool*, CIOBuffer*);
		f(0x4567A0L)(lpPool, pBuffer);
		lpPool->inUse = 0;
		LeaveCriticalSection(&g_IOBufferCriticalSection[threadIdx]);
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Too high current thread index[%d]!", __FUNCTION__, threadIdx);
		LPDWORD p = 0;
		(*p) = 0x666;
	}
	unguard;
}
