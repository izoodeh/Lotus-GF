#pragma once

class CLogger
{
	CSLock m_Lock;
	HANDLE m_File;
public:
	CLogger(const WCHAR* wFileBase = 0);
	~CLogger();
	void Init();
	void Create(PWCHAR sFile, PWCHAR sDirectory);
	void GenerateFileName(PWCHAR sFile, PWCHAR sDirectory, const WCHAR* wFileBase = 0);
	void Add(const WCHAR* format, ...);
};

extern CLogger g_Logger;