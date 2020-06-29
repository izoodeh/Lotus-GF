#include "stdafx.h"
#include "Log.h"

CLog g_Log;

CLog::CLog()
{
	pInstance = (CLog*)0x913EDD0;
}

void CLog::Add(LogType Type, const CHAR *format, ...)
{
	typedef void (*f)(CLog*, LogType, const CHAR*, va_list);
	va_list va;
	va_start(va, format);
	f(0x6B8890L)(pInstance, Type, format, va);
	va_end(va);
}

void CLog::Add(LogType Type, const WCHAR *format, ...)
{
	typedef void (*f)(CLog*, LogType, const WCHAR*, va_list);
	va_list va;
	va_start(va, format);
	f(0x6B92ACL)(pInstance, Type, format, va);
	va_end(va);
}

void CLog::CallStack(LogType type)
{
	typedef void (*f)(int);
	f(0x61B824L)(type);
}