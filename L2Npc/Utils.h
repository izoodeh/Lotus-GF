#pragma once

inline void WriteMemory(UINT64 baseAddress, LPCVOID lpMemory, SIZE_T size)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	LPBYTE lpData = (LPBYTE)lpMemory;
	DWORD priv = 0;
	if(VirtualProtect(lpBase, size, PAGE_EXECUTE_READWRITE, &priv))
	{
		for(int n=0;n<size;n++)
		{
			lpBase[n] = lpData[n];
		}
		DWORD old = 0;
		VirtualProtect(lpBase, size, priv, &old);
	}
}

inline void WriteNOP(UINT64 baseAddress, SIZE_T count)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	DWORD priv = 0;
	if(VirtualProtect(lpBase, count, PAGE_EXECUTE_READWRITE, &priv))
	{
		for(SIZE_T n = 0; n < count ; n++)
		{
			lpBase[n] = 0x90;
		}
		DWORD old = 0;
		VirtualProtect(lpBase, count, priv, &old);
	}
}

inline void WriteBYTE(UINT64 baseAddress, BYTE value)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	DWORD priv = 0;
	if(VirtualProtect(lpBase, 1, PAGE_EXECUTE_READWRITE, &priv))
	{
		(*lpBase) = value;
		DWORD old = 0;
		VirtualProtect(lpBase, 1, priv, &old);
	}
}

inline void WriteDWORD(UINT64 baseAddress, DWORD value)
{
	LPDWORD lpBase = (LPDWORD)baseAddress;
	DWORD priv = 0;
	if(VirtualProtect(lpBase, 4, PAGE_EXECUTE_READWRITE, &priv))
	{
		(*lpBase) = value;
		DWORD old = 0;
		VirtualProtect(lpBase, 4, priv, &old);
	}
}

inline void WriteQWORD(UINT64 baseAddress, INT64 value)
{
	PINT64 lpBase = (PINT64)baseAddress;
	DWORD priv = 0;
	if(VirtualProtect(lpBase, 8, PAGE_EXECUTE_READWRITE, &priv))
	{
		(*lpBase) = value;
		DWORD old = 0;
		VirtualProtect(lpBase, 8, priv, &old);
	}
}

inline void WriteDOUBLE(UINT64 baseAddress, DOUBLE value)
{
	DOUBLE* lpBase = (DOUBLE*)baseAddress;
	DWORD priv = 0;
	if(VirtualProtect(lpBase, 8, PAGE_EXECUTE_READWRITE, &priv))
	{
		(*lpBase) = value;
		DWORD old = 0;
		VirtualProtect(lpBase, 8, priv, &old);
	}
}

inline void WriteCall(UINT64 baseAddress, LPVOID function, SIZE_T nop = 0)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	HookMem mem;
	mem.full.opCode = 0xE8;	//CALL
	UINT64 fundAddr = (UINT64)function;
	fundAddr -= (baseAddress + 5);
	mem.full.addr = (DWORD)fundAddr;

	DWORD priv = 0;
	if(VirtualProtect(lpBase, (5+nop), PAGE_EXECUTE_READWRITE, &priv))
	{
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
		DWORD old = 0;
		VirtualProtect(lpBase, (5+nop), priv, &old);
	}
}

inline void WriteToVMT(INT64 vmtAddress, INT32 offset, LPVOID function)
{
	PINT64 lpVMT = (PINT64)vmtAddress;
	offset /= 8;
	DWORD priv = 0;
	if(VirtualProtect(&lpVMT[offset], 8, PAGE_EXECUTE_READWRITE, &priv))
	{
		lpVMT[offset] = (INT64)function;
		DWORD old = 0;
		VirtualProtect(&lpVMT[offset], 8, priv, &old);
	}
}

inline void WriteJump(INT64 baseAddress, INT64 func, SIZE_T nop = 0)
{
	LPBYTE lpBase = (LPBYTE)baseAddress;
	HookMem mem;
	mem.full.opCode = 0xE9;	//Jump
	func -= (baseAddress + 5);
	mem.full.addr = (DWORD)func;

	DWORD priv = 0;
	if(VirtualProtect(lpBase, (5+nop), PAGE_EXECUTE_READWRITE, &priv))
	{
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
		DWORD old = 0;
		VirtualProtect(lpBase, (5+nop), priv, &old);
	}
}

inline void WriteRelative(INT64 baseAddr, LPVOID lpObject)
{
	DWORD relative = (DWORD)((INT64)lpObject - (baseAddr + 4));
	DWORD priv = 0;
	if(VirtualProtect((LPVOID)baseAddr, 4, PAGE_EXECUTE_READWRITE, &priv))
	{
		(*(LPDWORD)baseAddr) = relative;
		DWORD old = 0;
		VirtualProtect((LPVOID)baseAddr, 4, priv, &old);
	}
}

void MovLogFiles(LPCTSTR sLogDir);
BOOL WriteFileBinary(LPCSTR lpFileName, LPBYTE lpBuffer, UINT length);