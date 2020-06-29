#pragma once

//size 30h
class CSharedRWLock
{
public:
	/* 00 */ HANDLE hFileMapping; //L2SharedLock_MMF_%s_%d
	/* 08 */ HANDLE hEvent;	//L2SharedLock_Event_%s_%d
	/* 10 */ HANDLE hSemaphoreR; //L2SharedLock_SemReader_%s_%d
	/* 18 */ HANDLE hSemaphoreW; //L2SharedLock_SemWriter_%s_%d
	/* 20 */ LPVOID lpMemory;
	/* 28 */ INT32 _unkn28;
	/* 2C */ INT32 _align2C;

	void SetSpinCount(DWORD dwSpinCount);
	void ReadLock(const WCHAR* file = __FILEW__, int line = __LINE__, bool bConcession = false);
	void WriteLock(const WCHAR* file = __FILEW__, int line = __LINE__);
	void Done(const WCHAR* file = __FILEW__, int line = __LINE__);
};

class CYieldLock
{
	DWORD unkn1;	//0
	DWORD unkn2;	//-1
	DWORD unkn3;	//0
public:
	void Enter(const WCHAR* file = __FILEW__, int line = __LINE__);
	void Leave(const WCHAR* file = 0, int line = 0);
};

class CRWLock
{
	CRITICAL_SECTION cs;
public:
	void Acquire(const WCHAR* file, int line);
	void Release();
};