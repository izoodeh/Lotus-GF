#pragma once

class CSLock
{
	CRITICAL_SECTION m_lock;
	UINT m_lockCount;
public:
	CSLock() { InitializeCriticalSection(&m_lock); };
	~CSLock() { DeleteCriticalSection(&m_lock); };
	inline void Enter() { EnterCriticalSection(&m_lock); m_lockCount++; };
	inline void Leave() { m_lockCount--; LeaveCriticalSection(&m_lock); };
	inline UINT GetLockCount() { return m_lockCount; };
};

inline void GetTimeInfo(tm& timeinfo)
{
	time_t rawtime;
	time ( &rawtime );
	localtime_s(&timeinfo, &rawtime );
}

const unsigned char* Disassemble(const unsigned char* packet, const char* format, ...);
int Assemble(char* buff, int len, const char* format, ...);
int VAssemble(char* buff, int len, const char* format, va_list va);
void MovLogFiles(LPCTSTR sLogDir);

inline void WriteMemory(UINT64 baseAddress, LPCVOID lpMemory, SIZE_T size)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	LPBYTE lpData = (LPBYTE)lpMemory;
	for(int n=0;n<size;n++)
	{
		lpBase[n] = lpData[n];
	}
}

inline void WriteNOP(UINT64 baseAddress, SIZE_T count)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	for(SIZE_T n = 0; n < count ; n++)
	{
		lpBase[n] = 0x90;
	}
}

inline void WriteBYTE(UINT64 baseAddress, BYTE value)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	(*lpBase) = value;
}

inline void WriteDWORD(UINT64 baseAddress, DWORD value)
{
	LPDWORD lpBase = (LPDWORD)baseAddress;
	(*lpBase) = value;
}

inline void WriteQWORD(UINT64 baseAddress, INT64 value)
{
	PUINT64 lpBase = (PUINT64)baseAddress;
	(*lpBase) = value;
}

inline void WriteCall(UINT64 baseAddress, LPVOID func, SIZE_T nop = 0)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	HookMem mem;
	mem.full.opCode = 0xE8;	//CALL
	UINT64 funcAddr = (UINT64)func;
	funcAddr -= (baseAddress + 5);
	mem.full.addr = (DWORD)funcAddr;

	for(SIZE_T n = 0; n < (5 + nop) ; n++)
	{
		if(n < 5)
		{
			lpBase[n] = mem.byte.p[n];
		}else
		{
			lpBase[n] = 0x90;
		}
	}
}

inline void WriteJump(UINT64 baseAddress, UINT64 destination, SIZE_T nop = 0)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	HookMem mem;
	mem.full.opCode = 0xE9;	//Jump
	destination -= (baseAddress + 5);
	mem.full.addr = (DWORD)destination;

	for(SIZE_T n = 0; n < (5 + nop) ; n++)
	{
		if(n < 5)
		{
			lpBase[n] = mem.byte.p[n];
		}else
		{
			lpBase[n] = 0x90;
		}
	}
}

inline void WriteJump(UINT64 baseAddress, PVOID func, SIZE_T nop = 0)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	HookMem mem;
	mem.full.opCode = 0xE9;	//Jump
	UINT64 destination = (UINT64)func;
	destination -= (baseAddress + 5);
	mem.full.addr = (DWORD)destination;

	for(SIZE_T n = 0; n < (5 + nop) ; n++)
	{
		if(n < 5)
		{
			lpBase[n] = mem.byte.p[n];
		}else
		{
			lpBase[n] = 0x90;
		}
	}
}