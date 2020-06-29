#include "stdafx.h"
#include "Utils.h"

void MovLogFiles(LPCTSTR sLogDir)
{
	list<wstring> lLogs;
	CreateDirectory(sLogDir, NULL);
	WIN32_FIND_DATA info;
	HANDLE hFile = 0;
	TCHAR sDir[265];
	GetCurrentDirectory(260, sDir);
	_tcscat_s(sDir, 260, TEXT("\\*"));
	hFile = FindFirstFile(sDir, &info);
	if(hFile)
	{
		do
		{
			if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			 
			}
			else
			{
				if(_tcscmp(info.cFileName, TEXT("LinError.txt")))
				{
					if(_tcsstr(info.cFileName, TEXT("LinError")) || _tcsstr(info.cFileName, TEXT(".dmp")))
					{
						lLogs.push_back(info.cFileName);
						continue;
					}
				}
			}
		}
		while (FindNextFile(hFile, &info) != 0);
		FindClose(hFile);
	}
	if(lLogs.size() > 0)
	{
		for(list<wstring>::iterator Iter = lLogs.begin();Iter!=lLogs.end();Iter++)
		{
			TCHAR sNewFile[512];
			_tcscpy_s(sNewFile, 512, sLogDir);
			_tcscat_s(sNewFile, TEXT("\\"));
			_tcscat_s(sNewFile, Iter->c_str());
			MoveFile(Iter->c_str(), sNewFile);
		}
	}
};

BOOL WriteFileBinary(LPCSTR lpFileName, LPBYTE lpBuffer, UINT length)
{
	BOOL ret = FALSE;
	HANDLE hFile = CreateFileA(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile)
	{
		
		DWORD written = 0;
		WriteFile(hFile, lpBuffer, length, &written, NULL);

		CloseHandle(hFile);
		ret = TRUE;
	}
	return ret;
}