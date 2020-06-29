#pragma once

class CQuery
{
	wstring m_query;
public:
	CQuery(wstring query);
	bool AddParam(INT32 param);
	bool AddParam(UINT param);
	bool AddParam(wstring param);
	const WCHAR* Get();
};