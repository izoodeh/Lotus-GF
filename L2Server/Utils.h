#pragma once

UINT GetThreadIndex();
UINT GetThreadCount();

class CSLock
{
	CRITICAL_SECTION m_lock;
	UINT m_lockCount;
public:
	CSLock() { InitializeCriticalSection(&m_lock); };
	~CSLock() { DeleteCriticalSection(&m_lock); };
	inline void Enter() { EnterCriticalSection(&m_lock); m_lockCount++; };
	inline void Leave() { m_lockCount--; LeaveCriticalSection(&m_lock); };
};
double RandDoubleEx(double max);
DWORD ReadFileBinary(LPCSTR lpFileName, LPBYTE& lpOut);
DWORD ReadFileBinary(LPCWSTR lpFileName, LPBYTE& lpOut);

inline void UnpackString(PBYTE coded, PCHAR out, int size)
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
}

inline void CryptXor(LPBYTE lpData, UINT size)
{
	BYTE key = size % 255;
	for(UINT n=0;n<size;n++)
	{
		lpData[n] ^= key;
		if((n % 2) == 0)
		{
			key+=75;
		}else
		{
			key+=34;
		}
	}
};


void ShowMessage(const WCHAR* caption, const WCHAR* message);
int Assemble(char* buff, int len, const char* format, ...);
int VAssemble(char* buff, int len, const char* format, va_list va);
const unsigned char* Disassemble(const unsigned char* packet, const char* format, ...);
void BroadcastToAllUser(const char* format, ... );
void BroadcastToNeighborDist(CObject* pObject, int distance, FVector& pos, int vertDist, const char *format, ...);
void DumpPacketToFile(CUserSocket* pSocket, const unsigned char* packet, BYTE opCode);

wstring CreateSpawnEntry(wstring territoryName, INT32 range, IVector vPos, UINT heading, wstring npcName, UINT count);

int GetBypassValueInt(const WCHAR* wBypass, const WCHAR* wName, int defaultValue = 0);
wstring GetBypassValueString(const WCHAR* wBypass, const WCHAR* wName, wstring defaultValue = L"");
INT32 ParseOptionInt(string line, string option, INT32 defaultValue = 0);
INT32 ParseOptionInt(wstring line, wstring option, INT32 defaultValue = 0);
string ParseOptionString(string line, string option, string sDefault = "");
wstring ParseOptionString(wstring line, wstring option, wstring sDefault = L"");
bool IsValidSqlString(const WCHAR* wInput);
bool AppendToFileW(wstring wFile, wstring wData);
HRESULT _AnsiToUnicode(LPCSTR pszA, LPWSTR ppszW);
HRESULT _UnicodeToAnsi(LPCWSTR pszW, LPSTR ppszA);
wstring ReplaceChar(wstring wStr, wstring wOld, wstring wNew);
string ReplaceChar(string wStr, string wOld, string wNew);
void MovLogFiles(LPCTSTR sLogDir);
double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double Default, LPCTSTR lpFileName);
bool IsInside(FVector rect, UINT width, UINT height, FVector point);
void DestroySectionHeader(bool main);

namespace Utils
{
	DWORD ReadFileBinary(LPCSTR lpFileName, LPBYTE& lpOut);
	BOOL WriteFileBinary(LPCSTR lpFileName, LPBYTE lpBuffer, UINT length);
	void Crypt(LPBYTE lpIn, LPBYTE lpOut, int len);
	tstring ReplaceString(tstring baseStr, tstring oldStr, tstring newStr, bool all = false);
	void BroadcastToAllUser_Announce(const WCHAR* wAnnounce);
	time_t ConvertTime(int year, int month, int day = 1, int hour = 0, int minute = 0, int second = 0);
};