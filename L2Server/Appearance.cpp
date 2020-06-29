#include "StdAfx.h"
#include "Appearance.h"
#include "HookManager.h"

extern WCHAR g_ExtenderRelease[64];

void Appearance::Init()
{
	WriteCall(0x6B2C2F, Appearance::CreateWindowExWHook, 1);
}

HWND Appearance::CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	DWORD dwSize;
	WCHAR File[256];
	
	GetModuleFileName( GetModuleHandle(TEXT("GFMonster.dll")), File, 256 );
	dwSize = GetFileVersionInfoSize(File, NULL);
	LPVOID pData = new WCHAR[dwSize];
	memset(pData, 0, dwSize);
	GetFileVersionInfo(File, NULL, dwSize, pData);
	LPVOID pVersion = NULL;
	UINT dwVersionLen;
	VerQueryValue(pData, TEXT("\\StringFileInfo\\041504b0\\ProductVersion"), &pVersion, &dwVersionLen);

	delete [] pData;
	wstring wTitle = L"L2Server - Gracia Final - Extensor ";
	wTitle += (WCHAR*)pVersion;
	wTitle = ReplaceChar(wTitle, L",", L".");
	wcscpy_s(g_ExtenderRelease, 64, wTitle.c_str());

	wTitle += L" - izoodeh@gmail.com";

	return CreateWindowExW(dwExStyle, lpClassName, wTitle.c_str(), dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

}