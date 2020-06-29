#pragma once

class CObjectDB
{
	CObjectDB* lpInstance;
public:
	CObjectDB();
	~CObjectDB();
	int GetClassIdFromName(const WCHAR* wName);
	const WCHAR* GetName(UINT classId);
};

extern CObjectDB g_ObjectDB;