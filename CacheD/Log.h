#pragma once

class CLog
{
	CLog* lpInstance;
public:
	enum LogType
	{
		Black = 0,
		Blue = 1,
		Error = 2,
		In = 6,
		Audit = 8
	};

	CLog();
	~CLog();
	void Add(LogType type, const WCHAR* format, ...);
};

extern CLog g_Log;