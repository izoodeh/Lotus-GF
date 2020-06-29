#include "stdafx.h"
#include "NWindow.h"

UINT g_AutoTypeExit1 = 0;
UINT g_AutoTypeExit2 = 0;

UINT g_AutoTypeLowestId = 0;
UINT g_AutoTypeHighestId = 0;

CNWindow g_NWindow;

CNWindow::CNWindow() : m_Initialized(FALSE)
{
}

CNWindow::~CNWindow()
{
}

void __declspec(naked) IsAutoType1()
{
	// 19150 - 19160
	//20424 - 20426
	__asm
	{
		cmp eax, g_AutoTypeLowestId
		jl NormalExecute
		cmp eax, g_AutoTypeHighestId
		jg NormalExecute
		jmp IsGood

		NormalExecute:
			cmp eax, 3952
			jmp g_AutoTypeExit1

		IsGood:
			mov eax, 1
			retn
	};
}

void __declspec(naked) IsAutoType2()
{
	// 19150 - 19160
	//20424 - 20426
	__asm
	{
		cmp eax, g_AutoTypeLowestId
		jl NormalExecute
		cmp eax, g_AutoTypeHighestId
		jg NormalExecute
		jmp IsGood

		NormalExecute:
			cmp eax, 3952
			jmp g_AutoTypeExit2

		IsGood:
			mov eax, 1
			retn
	};
}

void CNWindow::Init()
{
	if(!m_Initialized)
	{
		m_Initialized = TRUE;
		const TCHAR* section = _T("AutoType");
		TCHAR configFile[256];
		GetCurrentDirectory(MAX_PATH, configFile);
		lstrcat(configFile, _T("\\Client.ini"));

		g_AutoTypeLowestId = GetPrivateProfileInt(section, _T("LowestId"), 0, configFile);
		g_AutoTypeHighestId = GetPrivateProfileInt(section, _T("HighestId"), 0, configFile);

		if(g_AutoTypeLowestId > 0 && g_AutoTypeHighestId > 0)
		{
			HMODULE hNWindow = GetModuleHandle(L"NWindow.dll");
			if(hNWindow)
			{
				UINT addr1 = (UINT)hNWindow;
				UINT addr2 = (UINT)hNWindow;
				//russian version:  addr += 0x204F80;
				//83 protocol
				//addr += 0x204EF0;
				//87 protocol
				addr1 += 0x205C50;
				addr2 += 0x20835F;
				g_AutoTypeExit1 = addr1 + 5;
				g_AutoTypeExit2 = addr2 + 5;
				Memory::WriteJump(addr1, IsAutoType1, 0);
				Memory::WriteJump(addr2, IsAutoType2, 0);
			}
		}	
	}
}