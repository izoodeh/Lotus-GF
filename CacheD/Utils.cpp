#include "StdAfx.h"
#include "Utils.h"

const unsigned char* Disassemble(const unsigned char* packet, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	typedef const unsigned char* (*f)(const unsigned char*, const char*, va_list);
	const unsigned char* ret = f(0x4737A4L)(packet, format, va);
	va_end(va);

	return ret;
}

int Assemble(char* buff, int len, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	typedef int (*f)(char*, int, const char*, va_list);
	int ret = f(0x4730C4L)(buff, len, format, va);
	va_end(va);

	return ret;
}

int VAssemble(char* buff, int len, const char* format, va_list va)
{
	typedef int (*f)(char*, int, const char*, va_list);
	return f(0x4730C4L)(buff, len, format, va);
}

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