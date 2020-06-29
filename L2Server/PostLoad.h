#pragma once

class PostLoad
{
public:
	static void Init();
	static void InitBeforeGeodata();
	static void InitBeforeScriptDir(LPVOID pInstance, int type, const char* message);
	static void InitAfterScriptDir();
	static void InitAfterCacheD(LPVOID pInstance);
	static void InitWhenLoaded(UINT param);
};