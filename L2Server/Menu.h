#pragma once

enum MenuType
{
	MenuReloadRates = 5001
};

class CMenu
{
public:
	void Init();
	static BOOL ShowWindowHook(HWND hWnd, int nCmdShow);
	static LRESULT CALLBACK HandleCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ReloadServerRates();
};

extern CMenu g_Menu;