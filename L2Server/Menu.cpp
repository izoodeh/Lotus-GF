#include "stdafx.h"
#include "Menu.h"
#include "HookManager.h"

CMenu g_Menu;

void CMenu::Init()
{
	WriteCall(0x6B3068, CMenu::ShowWindowHook, 1);
	WriteCall(0x6B0DF5, CMenu::HandleCommand, 1);
}

BOOL CMenu::ShowWindowHook(HWND hWnd, int nCmdShow)
{
	HMENU hOriginalMenu = GetMenu(hWnd);	
	HMENU hExtMenu = CreateMenu();
	AppendMenu(hExtMenu, 0, MenuReloadRates, _T("Reload Rates"));
	
	InsertMenu(hOriginalMenu, -1, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hExtMenu, _T("Extender"));	
	return ShowWindow(hWnd, nCmdShow);
}

LRESULT CALLBACK CMenu::HandleCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_COMMAND)	//it's always 0x111 in this hook
	{
		switch (LOWORD(wParam)) 
		{ 
		case MenuReloadRates:
			{
				g_Log.Add(CLog::Blue, "Reloading server rates...");
				g_Menu.ReloadServerRates();
				return TRUE;
			}
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CMenu::ReloadServerRates()
{
	TCHAR file[260];
	GetCurrentDirectory(MAX_PATH,file);
	lstrcat(file, _T("\\l2server.ini"));
	const TCHAR* section = _T("event");

	//adenaamount
	double adenaamount = static_cast<double>(GetPrivateProfileInt(section, _T("adenaamount"), 100, file));
	adenaamount /= 100;
	double* pAdenaAmount = (double*)0xE52380;
	(*pAdenaAmount) = adenaamount;
	//itemrate
	double itemrate = static_cast<double>(GetPrivateProfileInt(section, _T("itemrate"), 100, file));
	itemrate /= 100;
	double* pItemRate = (double*)0xE52388;
	(*pItemRate) = itemrate;
	//spoilrate
	double spoilrate = static_cast<double>(GetPrivateProfileInt(section, _T("spoilrate"), 100, file));
	spoilrate /= 100;
	double* pSpoilRate = (double*)0xE52390;
	(*pSpoilRate) = spoilrate;
	//exprate
	double exprate = static_cast<double>(GetPrivateProfileInt(section, _T("exprate"), 100, file));
	exprate /= 100;
	double* pExpRate = (double*)0xE52398;
	(*pExpRate) = exprate;
	//partyexprate
	double partyexprate = static_cast<double>(GetPrivateProfileInt(section, _T("partyexprate"), 100, file));
	partyexprate /= 100;
	double* pPartyExpRate = (double*)0xE523A0;
	(*pPartyExpRate) = partyexprate;
	//aiexprate
	double aiexprate = static_cast<double>(GetPrivateProfileInt(section, _T("aiexprate"), 100, file));
	aiexprate /= 100;
	double* pAiExpRate = (double*)0xE523A8;
	(*pAiExpRate) = aiexprate;
	//aiadenarate
	double aiadenarate = static_cast<double>(GetPrivateProfileInt(section, _T("aiadenarate"), 100, file));
	aiadenarate /= 100;
	double* pAiAdenaRate = (double*)0xF0CA60;
	(*pAiAdenaRate) = aiadenarate;
	g_Log.Add(CLog::Blue, "New server rates - adenaamount[%f], itemrate[%f], spoilrate[%f], exprate[%f], partyexprate[%f], aiexprate[%f], aiadenarate[%f]", adenaamount, itemrate, spoilrate, exprate, partyexprate, aiexprate, aiadenarate);
}