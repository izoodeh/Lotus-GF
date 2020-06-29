#pragma once

class CLog
{
	CLog* pInstance;
public:
	enum LogType
	{
		Black = 0,	//SYSTEM
		Blue = 1,	//SYSTEM
		Error = 2,	//ERR
		In = 6,
		Db = 7,
		Audit = 8,
		Chat = 9
	};
	CLog();
	void Add(LogType Type, const CHAR *format, ...);
	void Add(LogType Type, const WCHAR *format, ...);
	void CallStack(LogType type);
};

extern CLog g_Log;

#define EXCEPTION_LOG g_Log.Add(CLog::Error, "[%s] exception occurred!", __FUNCTION__)
#define EXCEPTION_LOG_PARAM(paramString) g_Log.Add(CLog::Error, "[%s] exception occurred! - (%s)", __FUNCTION__, paramString)