#include "StdAfx.h"
#include "Log.h"

CLog g_Log;

CLog::CLog()
{
	lpInstance = (CLog*)0x1962620;
}

CLog::~CLog()
{
}

void CLog::Add(CLog::LogType type, const WCHAR *format, ...)
{
	va_list va;
	va_start(va, format);
	typedef void(*f)(CLog*, LogType, const WCHAR*, va_list);
	f(0x47DCF0L)(lpInstance, type, format, va);
	va_end(va);
}