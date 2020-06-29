#pragma once
typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)
#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)

typedef HANDLE (WINAPI * _openProcess)(__in DWORD dwDesiredAccess, __in BOOL bInheritHandle, __in DWORD dwProcessId);
typedef BOOL (WINAPI * _enumWindows)(WNDENUMPROC, LPARAM);
typedef BOOL (WINAPI * _writeProcessMemory)(  __in   HANDLE hProcess,  __in   LPVOID lpBaseAddress,  __in   LPCVOID lpBuffer,  __in   SIZE_T nSize,  __out  SIZE_T *lpNumberOfBytesWritten);
typedef BOOL (WINAPI * _closeHandle)( __in  HANDLE hObject);
typedef int (WINAPI * _messageBoxA)(  __in_opt  HWND hWnd,  __in_opt  const CHAR* lpText,  __in_opt  const CHAR* lpCaption,  __in      UINT uType);
typedef BOOL (WINAPI * _exitWindowsEx)( __in  UINT uFlags,  __in  DWORD dwReason);
typedef VOID (WINAPI * _exitProcess)(UINT);
typedef HANDLE (WINAPI * _createFile)(LPCSTR, DWORD,DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef DWORD (WINAPI * _getFileSize)(HANDLE, LPDWORD);
typedef HANDLE (WINAPI *_createThread)(__in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes, __in      SIZE_T dwStackSize,  __in      LPTHREAD_START_ROUTINE lpStartAddress, __in_opt  LPVOID lpParameter, __in      DWORD dwCreationFlags,__out_opt LPDWORD lpThreadId);
typedef WINUSERAPI HHOOK (WINAPI *_setWindowsHookExW)(    __in int idHook,    __in HOOKPROC lpfn,    __in_opt HINSTANCE hmod,    __in DWORD dwThreadId);
typedef int (WINAPI* _getClassNameW)(HWND, LPWSTR, int);
typedef int (WINAPI* _getWindowTextW)(HWND, LPWSTR, int);
typedef DWORD (WINAPI* _getWindowThreadProcessId)( __in HWND hWnd, __out_opt LPDWORD lpdwProcessId);

extern _openProcess openProcess;
extern _writeProcessMemory writeProcessMemory;
extern _closeHandle closeHandle;
extern _messageBoxA messageBoxA;
extern _exitWindowsEx exitWindowsEx;
extern _exitProcess exitProcess;
extern _getFileSize getFileSize;
extern _createFile createFile;
extern _createThread createThread;
extern _setWindowsHookExW setWindowsHookExW;
extern _enumWindows enumWindows;
extern _getClassNameW getClassNameW;
extern _getWindowTextW getWindowTextW;
extern _getWindowThreadProcessId getWindowThreadProcessId;

extern BOOL g_RequestExit;

typedef struct _CLIENT_ID
{
     PVOID UniqueProcess;
     PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

namespace Nt
{
	NTSTATUS NTAPI ProtectVirtualMemory( IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN OUT PULONG NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection );
	NTSTATUS NTAPI 	DebugContinue (__in HANDLE DebugObjectHandle, __in PCLIENT_ID ClientId, __in NTSTATUS ContinueStatus);
	extern "C" NTSYSAPI PVOID NTAPI RtlImageDirectoryEntryToData( IN PVOID Base, IN BOOLEAN MappedAsImage, IN USHORT DirectoryEntry, OUT PULONG Size );
	extern "C" NTSYSAPI VOID NTAPI DbgUiRemoteBreakin( IN PVOID Context );
	extern "C" NTSYSAPI VOID NTAPI DbgUserBreakPoint( VOID );
	extern "C" NTSYSAPI VOID NTAPI DbgBreakPoint( VOID );
};

namespace Kernel
{
	BOOL WINAPI CreateProcessInternalW (HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken);
	HMODULE WINAPI GetModuleHandleA(PCHAR lpName);
	HMODULE WINAPI GetModuleHandleW(PWCHAR lpName);
	HANDLE WINAPI CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPCH lpFileName, DWORD nSize);
	BOOL WINAPI CloseHandle(HANDLE hObject);
};

namespace Utils
{
	BOOL Protect(PVOID BaseAddress, SIZE_T RegionSize, ULONG& ProtectType);
	PVOID GetProcAddress( PVOID ModuleBase, PCHAR pFunctionName );
	PVOID GetProcAddress( PVOID ModuleBase, LPBYTE pFunctionName, UINT funcNameSize );
	void DestroySectionHeader(bool main = false);
	bool CompareMemory();
	void WriteHook(BYTE OpCode, DWORD dwBaseAddr, DWORD dwTargetAddr, int nNops);
	DWORD WriteDWORD(DWORD dwBaseAddr, DWORD newValue);
	DWORD WriteBYTE(DWORD dwBaseAddr, BYTE newValue);
	bool WriteBYTES(DWORD dwBaseAddr, PBYTE pData, SIZE_T len);
	FARPROC HookProcInModule(PSTR sModule, PSTR sFunction, FARPROC pNewFunction, BOOL showOverridenBytes = FALSE);
	FARPROC HookProcInModule(LPBYTE pModule, UINT moduleSize, LPBYTE pFunction, UINT functionSize, LPVOID pNewFunction);
	FARPROC HookProcInModuleEx(LPBYTE sModule, UINT moduleSize, LPBYTE sFunction, UINT functionSize, FARPROC pNewFunction, LPDWORD lpWriteAddr);
	void ShowMessage(const char* caption, const char* format, ...);
	BYTE GetModuleByte(HANDLE hModule, UINT offset);
};

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY
{
    ULONG  Unknown1;
    ULONG  Unknown2;
#ifdef _WIN64
        ULONG Unknown3;
        ULONG Unknown4;
#endif
    PVOID  Base;
    ULONG  Size;
    ULONG  Flags;
    USHORT  Index;
    USHORT  NameLength;
    USHORT  LoadCount;
    USHORT  PathLength;
    CHAR  ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
    ULONG Count;
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

inline PCHAR UnpackString(PBYTE coded, PCHAR out, int size)
{
	int m=0;
	for(int n=0;n<size;n++)
	{
		if((n%2)==0)
		{
			out[m] = coded[n];
			m++;
		}
	}
	return out;
}

inline void Crypt(LPBYTE lpIn, LPBYTE lpOut, int len)
{
	BYTE crypt = 0;
	for(int n=0;n<len;n++)
	{
		crypt += n;
		BYTE xorKey = crypt % 223;
		lpOut[n] = lpIn[n] ^ xorKey;
	}
}

inline PWCHAR UnpackStringW(PWORD coded, PWCHAR out, int size)
{
	int m=0;
	for(int n=0;n<size;n++)
	{
		if((n%2)==0)
		{
			out[m] = coded[n];
			m++;
		}
	}
	return out;
}

#pragma pack( push, 1 )

union HookMem
{
	struct
	{
		BYTE p[5];
	}byte;
	struct
	{
		BYTE opCode;
		DWORD addr;
	}full;
};

#pragma pack( pop )