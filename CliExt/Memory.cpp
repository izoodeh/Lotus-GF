#include "stdafx.h"
#include "Memory.h"

BOOL g_PerformingWriteOperation = FALSE;

using namespace Memory;
void Memory::Write(UINT address, LPVOID lpMemory, SIZE_T size)
{
	if(address && lpMemory)
	{
		LPVOID lpAddress = reinterpret_cast<LPVOID>(address);
		DWORD protect = PAGE_EXECUTE_READWRITE;
		g_PerformingWriteOperation = TRUE;
		if( VirtualProtect(lpAddress, size, protect, &protect) )
		{
			CopyMemory(lpAddress, lpMemory, size);
			VirtualProtect(lpAddress, size, protect, &protect);
		}
		g_PerformingWriteOperation = FALSE;
	}
}

void Memory::WriteBYTE(UINT address, BYTE value)
{
	Memory::Write(address, &value, 1);
};

void Memory::WriteDWORD(UINT address, DWORD value)
{
	Memory::Write(address, &value, sizeof(DWORD));
};

UINT Memory::WriteRelative(UINT nOrgAddress, UINT nWriteFrom, VOID *pArray, UINT nOffsetIndex) 
{ 
	UINT nAddressToPatch  = nOrgAddress+nWriteFrom;
	UINT nRIPAddress      = nOrgAddress+nWriteFrom+4;
	UINT nPointerAddress  = (UINT)pArray;
	UINT nRelativeAddress = ((nPointerAddress + nOffsetIndex) - nRIPAddress);

	WriteDWORD(nAddressToPatch, nRelativeAddress);

	return nRelativeAddress;
}

void Memory::Nop(UINT address, SIZE_T size)
{
	if(address)
	{
		LPBYTE lpAddress = reinterpret_cast<LPBYTE>(address);
		DWORD protect = PAGE_EXECUTE_READWRITE;
		g_PerformingWriteOperation = TRUE;
		if( VirtualProtect(lpAddress, size, protect, &protect) )
		{
			for(SIZE_T n = 0; n < size; n++)
			{
				lpAddress[n] = HookNOP;
			}
			VirtualProtect(lpAddress, size, protect, &protect);
		}
		g_PerformingWriteOperation = FALSE;
	}
}

void Memory::WriteCall(UINT address, PVOID function, SIZE_T nops)
{
	if(address)
	{
		LPBYTE lpAddress = reinterpret_cast<LPBYTE>(address);
		HookMemory mem;
		mem.data.type = HookCall;
		UINT fundAddr = reinterpret_cast<UINT>(function);
		fundAddr -= (address + 5);
		mem.data.address = static_cast<UINT>(fundAddr);

		DWORD priv = 0;
		g_PerformingWriteOperation = TRUE;
		if(VirtualProtect(lpAddress, (5+nops), PAGE_EXECUTE_READWRITE, &priv))
		{
			for(SIZE_T n = 0; n < (5 + nops) ; n++)
			{
				if(n < 5)
				{
					lpAddress[n] = mem.bytes.byte[n];
				}else
				{
					lpAddress[n] = HookNOP;
				}
			}
			VirtualProtect(lpAddress, (5+nops), priv, &priv);
		}
		g_PerformingWriteOperation = FALSE;
	}
}

void Memory::WriteJump(UINT address, PVOID function, SIZE_T nops)
{
	if(address)
	{
		LPBYTE lpAddress = reinterpret_cast<LPBYTE>(address);
		HookMemory mem;
		mem.data.type = HookJump;	//JMP
		UINT fundAddr = reinterpret_cast<UINT>(function);
		fundAddr -= (address + 5);
		mem.data.address = static_cast<UINT>(fundAddr);

		DWORD priv = 0;
		g_PerformingWriteOperation = TRUE;
		if(VirtualProtect(lpAddress, (5+nops), PAGE_EXECUTE_READWRITE, &priv))
		{
			for(SIZE_T n = 0; n < (5 + nops) ; n++)
			{
				if(n < 5)
				{
					lpAddress[n] = mem.bytes.byte[n];
				}else
				{
					lpAddress[n] = HookNOP;
				}
			}
			VirtualProtect(lpAddress, (5+nops), priv, &priv);
		}
		g_PerformingWriteOperation = FALSE;
	}
}

void Memory::WriteJump(UINT address, UINT jumpAddress, SIZE_T nops)
{
	if(address)
	{
		LPBYTE lpAddress = reinterpret_cast<LPBYTE>(address);
		HookMemory mem;
		mem.data.type = HookJump;	//JMP
		jumpAddress -= (address + 5);
		mem.data.address = static_cast<UINT>(jumpAddress);

		DWORD priv = 0;
		g_PerformingWriteOperation = TRUE;
		if(VirtualProtect(lpAddress, (5+nops), PAGE_EXECUTE_READWRITE, &priv))
		{
			for(SIZE_T n = 0; n < (5 + nops) ; n++)
			{
				if(n < 5)
				{
					lpAddress[n] = mem.bytes.byte[n];
				}else
				{
					lpAddress[n] = HookNOP;
				}
			}
			VirtualProtect(lpAddress, (5+nops), priv, &priv);
		}
		g_PerformingWriteOperation = FALSE;
	}
}

LPBYTE Memory::OpenSharedMemory(HANDLE& hMapFile, const WCHAR* wName, UINT size)
{
	if(hMapFile == 0)
	{
		hMapFile = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, wName);
	}
	LPBYTE pBuf = 0;
	if(hMapFile == 0 || hMapFile == INVALID_HANDLE_VALUE)
	{
		hMapFile = hMapFile = CreateFileMapping( INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, wName);
		pBuf = (LPBYTE) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if(pBuf)
		{
			memset(pBuf, 0, size);
		}
	}else
	{
		pBuf = (LPBYTE) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
	}
	return pBuf;
}