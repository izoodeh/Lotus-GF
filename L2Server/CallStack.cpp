#include "stdafx.h"
#include "CallStack.h"
#include "HookManager.h"

CCallStack g_CallStack;

UINT g_CallStackIndex[32];
PCWSTR g_CallStackName[32][1000];

LPUINT g_CallTrackIndex = (LPUINT)0x228EC90;
PZPCWSTR g_CallTrackName = (PZPCWSTR)0x226F890;

CCallStack::CCallStack() : m_enabled(FALSE)
{
	memset(g_CallStackIndex, 0, sizeof(g_CallStackIndex));
	memset(g_CallStackName, 0, sizeof(g_CallStackName));
}

CCallStack::~CCallStack()
{
}

void CCallStack::Init()
{
	m_enabled = TRUE;
	if( GetFileAttributes(_T("ExtError.txt")) != INVALID_FILE_ATTRIBUTES)
	{
			tm ti;
			time_t rawtime;
			time ( &rawtime );
			localtime_s(&ti, &rawtime );

			tstringstream newName;
			newName << "ExtError.txt." << (ti.tm_year+1900) << "-" << (ti.tm_mon+1) << "-" << ti.tm_mday << "-" << ti.tm_hour << "-" << ti.tm_min << ".bak";
			MoveFile(TEXT("ExtError.txt"), newName.str().c_str());
	}
	WriteCall(0x61CAAE, CCallStack::GenerateLinError);
	WriteCall(0x61CC2C, CCallStack::GenerateLinError);
}

void CCallStack::GenerateLinError(LPVOID lpInstance)
{
	try
	{
		typedef void(*f)(LPVOID);
		f(0x61BCB8L)(lpInstance);
		g_CallStack.GenerateLog();
	}catch(...)
	{
		g_CallStack.GenerateLog(L"Exception while generating LinError!");
	}
}

void CCallStack::GenerateLog(const WCHAR* wInfo)
{
	if(m_enabled)
	{
		HANDLE hFile = CreateFile(_T("ExtError.txt"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile)
		{
			tm timeinfo;
			time_t rawtime;
			time ( &rawtime );
			localtime_s(&timeinfo, &rawtime );

			wstringstream dataStream;

			dataStream << L"Date: " << (timeinfo.tm_year+1900) << L"/" << (timeinfo.tm_mon+1) << L"/" << timeinfo.tm_mday << L" " << timeinfo.tm_hour << L":" << timeinfo.tm_min << L":" << timeinfo.tm_sec << L" " << endl;
			dataStream << L"Crash thread: " << GetThreadIndex() << L" - IO: " << GetThreadCount() << endl;

			UINT threadCount = GetThreadCount();

			for(UINT n=0;n<16;n++)
			{
				dataStream << L"Thread[" << n << L"] : ";
				UINT funcCount = g_CallStackIndex[n];
				for(UINT m=0;m<funcCount;m++)
				{
					dataStream << L" -> " << g_CallStackName[n][m];
				}
				dataStream << endl;
			}

			dataStream << endl << L"Extra info: " << endl;
			if(wInfo)
			{
				dataStream << wInfo << endl;
			}
			
			DWORD written;

			wstring output(dataStream.str());

			WriteFile(hFile, output.c_str(), (DWORD)(output.size() * 2), &written, NULL);

			CloseHandle(hFile);
		}else
		{
			MessageBox(0, _T("Cannot create ExtError.txt!"), _T("CallStack"), MB_OK);
		}
	}
}

bool CCallTrack::IsFunctionCalled(PCWSTR nameOffset)
{
	UINT threadIndex = GetThreadIndex();
	UINT funcCount = g_CallTrackIndex[threadIndex];
	UINT nameIndex = (threadIndex * 1000);

	for(UINT n=0;n<funcCount;n++)
	{
		if( g_CallTrackName[nameIndex + n] == nameOffset)
		{
			return true;
		}
	}

	return false;
}

void CCallTrack::CallStackThread(UINT threadId)
{
	if(threadId < 32)
	{
		UINT funcCount = g_CallTrackIndex[threadId];
		UINT nameIndex = (threadId * 1000);

		for(UINT n=0;n<funcCount;n++)
		{
			g_Log.Add(CLog::Blue, "CallStack[%d]: %S", n, g_CallTrackName[nameIndex + n]);
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Invalid threadId[%d]", __FUNCTION__, threadId);
	}
}