#include "StdAfx.h"
#include "Utils.h"
#include "VirtualizerSDK.h"

LPBYTE g_lpDllStart = 0;
SIZE_T g_DllSize = 0;
LPBYTE g_lpDllBackup = 0;

BYTE arrNtProtectVirtualMemory[46] = { 78, 53, 116, 190, 80, 182, 114, 77, 111, 251, 116, 0, 101, 48, 99, 34, 116, 0, 86, 92, 105, 0, 114, 126, 116, 18, 117, 225, 97, 75, 108, 0, 77, 82, 101, 139, 109, 97, 111, 0, 114, 0, 121, 0, 0, 0 };
char sNtProtectVirtualMemory[23];
BYTE arrNtDebugContinue[32] = { 78, 0, 116, 0, 68, 4, 101, 0, 98, 127, 117, 226, 103, 149, 67, 0, 111, 53, 110, 230, 116, 156, 105, 0, 110, 89, 117, 230, 101, 0, 0, 0 };
char sNtDebugContinue[16];
BYTE arrGetModuleHandleA[34] = { 71, 6, 101, 0, 116, 129, 77, 139, 111, 0, 100, 179, 117, 247, 108, 9, 101, 0, 72, 0, 97, 119, 110, 59, 100, 10, 108, 0, 101, 182, 65, 202, 0, 0 };
char sGetModuleHandleA[17];
BYTE arrGetModuleHandleW[34] = { 71, 72, 101, 0, 116, 176, 77, 0, 111, 157, 100, 197, 117, 150, 108, 53, 101, 77, 72, 0, 97, 0, 110, 30, 100, 0, 108, 0, 101, 40, 87, 0, 0, 0 };
char sGetModuleHandleW[17];

NTSTATUS NTAPI Nt::ProtectVirtualMemory( IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN OUT PULONG NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection )
{
	typedef NTSTATUS (NTAPI *f)( IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN OUT PULONG NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection );
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_NtDll, UnpackString(arrNtProtectVirtualMemory, sNtProtectVirtualMemory, 46)));

	return _f(ProcessHandle, BaseAddress, NumberOfBytesToProtect, NewAccessProtection, OldAccessProtection);
}

NTSTATUS NTAPI Nt::DebugContinue(__in HANDLE DebugObjectHandle, __in PCLIENT_ID ClientId, __in NTSTATUS ContinueStatus)
{
	typedef NTSTATUS (NTAPI * f)(__in HANDLE, __in PCLIENT_ID, __in NTSTATUS);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_NtDll, UnpackString(arrNtDebugContinue, sNtDebugContinue, 32)));

	return _f(DebugObjectHandle, ClientId, ContinueStatus);
}

HMODULE WINAPI Kernel::GetModuleHandleA(PCHAR lpName)
{
	typedef HMODULE (WINAPI * f)(PCHAR);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, UnpackString(arrGetModuleHandleA, sGetModuleHandleA, 34)));
	return _f(lpName);
}

HMODULE WINAPI Kernel::GetModuleHandleW(PWCHAR lpName)
{
	typedef HMODULE (WINAPI * f)(PWCHAR);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, UnpackString(arrGetModuleHandleW, sGetModuleHandleW, 34)));
	return _f(lpName);
}

BYTE sCreateFileW[12] = { 0x43, 0x73, 0x66, 0x67, 0x7e, 0x6a, 0x53, 0x75, 0x48, 0x48, 0x60, 0x42};
HANDLE WINAPI Kernel::CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	typedef HANDLE (WINAPI *f)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, sCreateFileW, 12));

	return _f(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BYTE sCreateFileA[12] = { 0x43, 0x73, 0x66, 0x67, 0x7e, 0x6a, 0x53, 0x75, 0x48, 0x48, 0x76, 0x42};
HANDLE WINAPI Kernel::CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	typedef HANDLE (WINAPI *f)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, sCreateFileA, 12));

	return _f(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BYTE sGetModuleFileNameA[19] = { 0x47, 0x64, 0x77, 0x4b, 0x65, 0x6b, 0x60, 0x70, 0x41, 0x6b, 0x5e, 0x2e, 0x2b, 0x15, 0x8, 0x15, 0xed, 0xd8, 0xab};
DWORD WINAPI Kernel::GetModuleFileNameA(HMODULE hModule, LPCH lpFileName, DWORD nSize)
{
	typedef DWORD (WINAPI*f)(HMODULE, LPCH, DWORD);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, sGetModuleFileNameA, 19));
	
	return _f(hModule, lpFileName, nSize);
}

BYTE sCloseHandle[12] = { 0x43, 0x6d, 0x6c, 0x75, 0x6f, 0x47, 0x74, 0x72, 0x40, 0x41, 0x52, 0x42};
BOOL WINAPI Kernel::CloseHandle(HANDLE hObject)
{
	typedef BOOL (WINAPI*f)(HANDLE);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, sCloseHandle, 12));

	return _f(hObject);
}

BYTE arrCreateProcessInternalW[46] = { 67, 0, 114, 0, 101, 18, 97, 249, 116, 6, 101, 145, 80, 0, 114, 0, 111, 0, 99, 57, 101, 79, 115, 123, 115, 141, 73, 104, 110, 0, 116, 113, 101, 43, 114, 101, 110, 18, 97, 97, 108, 199, 87, 239, 0, 0 };
char sCreateProcessInternalW[23];

BOOL WINAPI Kernel::CreateProcessInternalW (HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken)
{
	typedef BOOL (WINAPI *f)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
	static f _f = reinterpret_cast<f>(Utils::GetProcAddress(g_Kernel32, UnpackString(arrCreateProcessInternalW, sCreateProcessInternalW, 46)));

	return _f(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
}

#pragma optimize("", off)
BOOL Utils::Protect(PVOID BaseAddress, SIZE_T RegionSize, ULONG& ProtectType)
{
	VIRTUALIZER_START;
	BOOL ret = FALSE;
	if ( BaseAddress && RegionSize && ProtectType )
	{
		if ( Nt::ProtectVirtualMemory( reinterpret_cast<HANDLE>(-1), &BaseAddress, &RegionSize, ProtectType, &ProtectType ) == 0 )
		{
			ret = TRUE;
		}
	}
	VIRTUALIZER_END;
	return ret;
}
#pragma optimize("", on)

PVOID Utils::GetProcAddress( PVOID ModuleBase, LPBYTE pFunctionName, UINT funcNameSize )
{
	Crypt(pFunctionName, pFunctionName, funcNameSize);

	PVOID pFunctionAddress = NULL;

	try
	{
		ULONG size = 0;
		PIMAGE_EXPORT_DIRECTORY exports =(PIMAGE_EXPORT_DIRECTORY)
			Nt::RtlImageDirectoryEntryToData(ModuleBase, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size);

		ULONG addr = (ULONG)exports-(ULONG)ModuleBase;

		PULONG functions = (PULONG)((ULONG) ModuleBase + exports->AddressOfFunctions);
		PSHORT ordinals  = (PSHORT)((ULONG) ModuleBase + exports->AddressOfNameOrdinals);
		PULONG names = (PULONG)((ULONG) ModuleBase + exports->AddressOfNames);
		ULONG  max_name = exports->NumberOfNames;
		ULONG  max_func = exports->NumberOfFunctions;

		ULONG i;

		for (i = 0; i < max_name; i++)
		{
			ULONG ord = ordinals[i];
			if(i >= max_name || ord >= max_func)
			{
				pFunctionAddress = 0;
				break;
			}
			if (functions[ord] < addr || functions[ord] >= addr + size)
			{
				if (strcmp((PCHAR) ModuleBase + names[i], (PCHAR)pFunctionName)  == 0)
				{
					pFunctionAddress =(PVOID)((PCHAR) ModuleBase + functions[ord]);
					break;
				}
			}
		}
	}
	catch(...)
	{
		pFunctionAddress = NULL;
	}
	Crypt(pFunctionName, pFunctionName, funcNameSize);

	return pFunctionAddress;
}

PVOID Utils::GetProcAddress( PVOID ModuleBase, PCHAR pFunctionName )
{
	PVOID pFunctionAddress = NULL;

	try
	{
		ULONG size = 0;
		PIMAGE_EXPORT_DIRECTORY exports =(PIMAGE_EXPORT_DIRECTORY)
			Nt::RtlImageDirectoryEntryToData(ModuleBase, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size);

		ULONG addr = (ULONG)exports-(ULONG)ModuleBase;

		PULONG functions = (PULONG)((ULONG) ModuleBase + exports->AddressOfFunctions);
		PSHORT ordinals  = (PSHORT)((ULONG) ModuleBase + exports->AddressOfNameOrdinals);
		PULONG names = (PULONG)((ULONG) ModuleBase + exports->AddressOfNames);
		ULONG  max_name = exports->NumberOfNames;
		ULONG  max_func = exports->NumberOfFunctions;

		ULONG i;

		for (i = 0; i < max_name; i++)
		{
			ULONG ord = ordinals[i];
			if(i >= max_name || ord >= max_func)
			{
				pFunctionAddress = 0;
				break;
			}
			if (functions[ord] < addr || functions[ord] >= addr + size)
			{
				if (strcmp((PCHAR) ModuleBase + names[i], pFunctionName)  == 0)
				{
					pFunctionAddress =(PVOID)((PCHAR) ModuleBase + functions[ord]);
					break;
				}
			}
		}
	}
	catch(...)
	{
		pFunctionAddress = NULL;
	}

	return pFunctionAddress;
}

BYTE g_EngineBackup[0x63c000];
BOOL g_EngineLoaded = FALSE;

bool Utils::CompareMemory()
{
	for(UINT n=0; n < g_DllSize; n++)
	{
		if(g_lpDllBackup[n] != g_lpDllStart[n])
		{
			g_RequestExit = 98852 + n;
		}
	}
	/*
	if(g_EngineLoaded)
	{
		//compare
		ofstream log("EngineDiff.txt");
		LPBYTE lpEngine = reinterpret_cast<LPBYTE>(g_Engine);
		for(UINT n=0;n<sizeof(g_EngineBackup); n++)
		{
			if(g_EngineBackup[n] != lpEngine[n])
			{
				log << "Offset: 0x" << hex << n << " Org: 0x" << hex << (UINT)g_EngineBackup[n]  << " New: 0x" << hex << (UINT)lpEngine[n] << endl;
			}
		}
		log.close();
	}else
	{
		g_EngineLoaded = TRUE;
		LPBYTE lpEngine = reinterpret_cast<LPBYTE>(g_Engine);
		for(UINT n=0;n<sizeof(g_EngineBackup); n++)
		{
			g_EngineBackup[n] = lpEngine[n];
		}
	}
	
	*/

	return true;
};

void Utils::DestroySectionHeader(bool main)
{
	VIRTUALIZER_START;

	ULONG_PTR Address;
	ULONG i;
	PIMAGE_DOS_HEADER ImageDosHeader;
	PIMAGE_NT_HEADERS ImageNtHeader;
	PIMAGE_SECTION_HEADER SectionHeader;
	LPVOID lpAddr = 0;
	DWORD size = 0, priv = PAGE_EXECUTE_READWRITE;


	if(main)
	{
		Address = (ULONG_PTR)Kernel::GetModuleHandle(0);
	}else
	{
		Address = (ULONG_PTR)g_Client;
	}

	for ( i = Address; i < Address + 0xF000; i += 0x1000 )
	{
		if ( *(PUSHORT) i == 0x5A4D )
		{
			ImageDosHeader = (PIMAGE_DOS_HEADER)i;
			ImageNtHeader = (PIMAGE_NT_HEADERS)( i + ImageDosHeader->e_lfanew );
			if ( ImageNtHeader->Signature == 0x00004550 )
			{
				SectionHeader = (PIMAGE_SECTION_HEADER) ( (PBYTE)&ImageNtHeader->OptionalHeader + ImageNtHeader->FileHeader.SizeOfOptionalHeader );

				lpAddr = &SectionHeader[0].Name[0];
				if(!main)
				{
					g_lpDllStart = reinterpret_cast<LPBYTE>(Address + SectionHeader[0].VirtualAddress);
					g_DllSize = SectionHeader[0].SizeOfRawData;
					g_lpDllBackup = new BYTE[g_DllSize];
					for(UINT n=0; n < g_DllSize; n++)
					{
						g_lpDllBackup[n] = g_lpDllStart[n];
					}

				};
				size = ImageNtHeader->FileHeader.NumberOfSections*IMAGE_SIZEOF_SECTION_HEADER;
				if(Utils::Protect(lpAddr, size, priv))
				{
					RtlZeroMemory(lpAddr, size);
					Utils::Protect(lpAddr, size, priv);
				}
				lpAddr = &ImageNtHeader->FileHeader;
				size = IMAGE_SIZEOF_FILE_HEADER;
				priv = PAGE_EXECUTE_READWRITE;
				if(Utils::Protect(lpAddr, size, priv))
				{
					ImageNtHeader->FileHeader.Characteristics |= 0x0001;	// RELOCATIONS_STRIPPED
					Utils::Protect(lpAddr, size, priv);
				}
				break;
			}
		}
	}
	VIRTUALIZER_END;
}

DWORD Utils::WriteDWORD(DWORD dwBaseAddr, DWORD newValue)
{
	DWORD ret = 0;

	DWORD privilege = PAGE_EXECUTE_READWRITE;
	if(Utils::Protect((PVOID)dwBaseAddr, 4, privilege))
	{
		ret = (*(LPDWORD)dwBaseAddr);
		(*(LPDWORD)dwBaseAddr) = newValue;
		Utils::Protect((PVOID)dwBaseAddr, 4, privilege);
	}

	return ret;
}

DWORD Utils::WriteBYTE(DWORD dwBaseAddr, BYTE newValue)
{
	DWORD ret = 0;

	DWORD privilege = PAGE_EXECUTE_READWRITE;
	if(Utils::Protect((PVOID)dwBaseAddr, 1, privilege))
	{
		ret = (*(LPDWORD)dwBaseAddr);
		(*(LPBYTE)dwBaseAddr) = newValue;
		Utils::Protect((PVOID)dwBaseAddr, 1, privilege);
	}

	return ret;
}

bool Utils::WriteBYTES(DWORD dwBaseAddr, PBYTE pData, SIZE_T len)
{
	DWORD privilege = PAGE_EXECUTE_READWRITE;
	if(Utils::Protect((PVOID)dwBaseAddr, 1, privilege))
	{
		memcpy((PBYTE)dwBaseAddr, pData, len);
		Utils::Protect((PVOID)dwBaseAddr, 1, privilege);
		return true;
	}

	return false;
}

void Utils::WriteHook(BYTE OpCode, DWORD dwBaseAddr, DWORD dwTargetAddr, int nNops)
{
	VIRTUALIZER_START;

	DWORD priv = PAGE_EXECUTE_READWRITE;
	SIZE_T BytesWritten = NULL;
	int nSize = 5 + nNops;
	UCHAR *buffer = new UCHAR[nSize];
	if(nNops)
	{
		memset(buffer, 0, nSize);
		buffer[0]=OpCode;
		dwTargetAddr-=dwBaseAddr+5;
		memcpy(&buffer[1],&dwTargetAddr,4);
		for(int n=5; n<nSize; n++)
		{
			buffer[n]=0x90;
		}

	}else
	{
		memset(buffer, 0, 5);
		buffer[0]=OpCode;
		dwTargetAddr-=dwBaseAddr+5;
		memcpy(buffer+1,&dwTargetAddr,4);
	}
	HANDLE 	hHandle = openProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());

	if(Utils::Protect((PVOID)dwBaseAddr, nSize, priv))
	{
		if (!writeProcessMemory(hHandle, (LPVOID)(dwBaseAddr), buffer, nSize, &BytesWritten))
		{
			ExitProcess(0);
		}
		Utils::Protect((PVOID)dwBaseAddr, nSize, priv);
	}
	closeHandle(hHandle);

	delete [] buffer;

	VIRTUALIZER_END;
}

FARPROC Utils::HookProcInModule(LPBYTE sModule, UINT moduleSize, LPBYTE sFunction, UINT functionSize, LPVOID pNewFunction)
{
	VIRTUALIZER_START;

	Crypt(sModule, sModule, moduleSize);
	Crypt(sFunction, sFunction, functionSize);

	HMODULE hModule = Kernel::GetModuleHandleA(reinterpret_cast<PCHAR>(sModule));
	FARPROC pNewProc = NULL;
	if(hModule)
	{
		LPBYTE pFunction = (LPBYTE)Utils::GetProcAddress(hModule, reinterpret_cast<PCHAR>(sFunction));
		if(pFunction)
		{			
			LPBYTE pMemory = (LPBYTE)VirtualAlloc(0, 32, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if(pMemory)
			{
				HookMem hm;
				for(int n=0;n<5;n++)
				{
					pMemory[n] = pFunction[n];
					hm.byte.p[n] = pFunction[n];
				}

				if(hm.full.opCode == 0xE8 || hm.full.opCode == 0xE9)
				{
					//reallocate addr
					DWORD addr = hm.full.addr;
					addr += (reinterpret_cast<DWORD>(pFunction) + 5);

					hm.full.addr = addr - (reinterpret_cast<DWORD>(pMemory) + 5);

					for(int n=0;n<5;n++)
					{
						pMemory[n] = hm.byte.p[n];
					}
				}

				WriteHook(0xE9, reinterpret_cast<DWORD>(&pMemory[5]), (reinterpret_cast<DWORD>(pFunction) + 5), 0);
				WriteHook(0xE9, reinterpret_cast<DWORD>(pFunction), reinterpret_cast<DWORD>(pNewFunction), 0);
				pNewProc = (FARPROC)pMemory;
			}
		}
	}

	
	Crypt(sModule, sModule, moduleSize);
	Crypt(sFunction, sFunction, functionSize);

	VIRTUALIZER_END;
	return pNewProc;
}

FARPROC Utils::HookProcInModule(PSTR sModule, PSTR sFunction, FARPROC pNewFunction, BOOL showOverridenBytes)
{
	VIRTUALIZER_START;

	HMODULE hModule = Kernel::GetModuleHandleA(sModule);
	FARPROC pNewProc = NULL;
	if(hModule)
	{
		LPBYTE pFunction = (LPBYTE)Utils::GetProcAddress(hModule, sFunction);
		if(pFunction)
		{			
			LPBYTE pMemory = (LPBYTE)VirtualAlloc(0, 32, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if(pMemory)
			{
				HookMem hm;
				for(int n=0;n<5;n++)
				{
					pMemory[n] = pFunction[n];
					hm.byte.p[n] = pFunction[n];
				}

				if(hm.full.opCode == 0xE8 || hm.full.opCode == 0xE9)
				{
					//reallocate addr
					DWORD addr = hm.full.addr;
					addr += ((DWORD)pFunction + 5);

					hm.full.addr = addr - ((DWORD)pMemory + 5);

					for(int n=0;n<5;n++)
					{
						pMemory[n] = hm.byte.p[n];
					}
				}

				WriteHook(0xE9, (DWORD)&pMemory[5], ((DWORD)pFunction + 5), 0);
				WriteHook(0xE9, (DWORD)pFunction, (DWORD)pNewFunction, 0);
				pNewProc = (FARPROC)pMemory;
			}
		}
	}

	VIRTUALIZER_END;
	return pNewProc;
}

void Utils::ShowMessage(const char* caption, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	char Msg[4096];
	vsprintf(Msg, format, va);
	messageBoxA(NULL, Msg, caption, MB_OK);
	va_end(va);
}

BYTE Utils::GetModuleByte(HANDLE hModule, UINT offset)
{
	try
	{
		LPBYTE lpModule = reinterpret_cast<LPBYTE>(hModule);
		return lpModule[offset];
	}catch(...)
	{
		return 0xFF;
	}
}

FARPROC Utils::HookProcInModuleEx(LPBYTE sModule, UINT moduleSize, LPBYTE sFunction, UINT functionSize, FARPROC pNewFunction, LPDWORD lpWriteAddr)
{
	VIRTUALIZER_START;

	Crypt(sModule, sModule, moduleSize);
	Crypt(sFunction, sFunction, functionSize);

	HMODULE hModule = GetModuleHandleA((PSTR)sModule);
	FARPROC pNewProc = NULL;

	if(hModule)
	{
		LPBYTE pFunction = (LPBYTE)Utils::GetProcAddress(hModule, (PSTR)sFunction);
		if(pFunction)
		{			
			LPBYTE pMemory = (LPBYTE)VirtualAlloc(0, 32, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			for(int n=0;n<32;n++)
			{
				pMemory[n]=0x90;
			}
			if(pMemory)
			{
				HookMem hm;
				for(int n=0;n<5;n++)
				{
					pMemory[n] = pFunction[n];
					hm.byte.p[n] = pFunction[n];
				}

				if(hm.full.opCode == 0xE8 || hm.full.opCode == 0xE9)
				{
					//reallocate addr
					DWORD addr = hm.full.addr;
					addr += ((DWORD)pFunction + 5);

					hm.full.addr = addr - ((DWORD)pMemory + 5);

					for(int n=0;n<5;n++)
					{
						pMemory[n] = hm.byte.p[n];
					}
				}

				WriteHook(0xE9, (DWORD)&pMemory[5], ((DWORD)(pFunction + 5)), 0);
				WriteHook(0xE9, (DWORD)pFunction, (DWORD)pNewFunction, 0);
				if(lpWriteAddr)
				{
					(*lpWriteAddr) = (DWORD)pFunction;
				}
				pNewProc = (FARPROC)pMemory;

			}
		}
	}

	Crypt(sModule, sModule, moduleSize);
	Crypt(sFunction, sFunction, functionSize);
	VIRTUALIZER_END;

	return pNewProc;
}