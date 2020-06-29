#pragma once

extern "C"
{
	void SafeScrollEtcTypeHook();
};

class CItemFactory
{
public:
	static void Init();
	static INT64 FindInTable(UINT field, WCHAR* str);
};