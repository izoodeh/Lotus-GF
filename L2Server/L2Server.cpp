#include "stdafx.h"
#include "HookManager.h"
#include "ClassExt.h"
#include "Appearance.h"
#include "PostLoad.h"
#include "L2Time.h"
#include "BaseAction.h"
#include "VitalitySystem.h"
#include "Protector.h"
#include "PledgeSystem.h"
#include "IOBufferPool.h"
#include "ItemFactory.h"
#include "Menu.h"

INT32 g_ProtocolVersion = 83;
BOOL g_EnableStatisticsPacket = FALSE;
INT64 g_MaxMultiSellQuanity = 10000;
BOOL g_EnableExpStopCommand = FALSE;
BOOL g_SpiritShotExtension = FALSE;
BOOL g_DisableFameSystem = FALSE;

WCHAR g_ConfigFile[260];
double g_AttributeAttackMultipler = 1.0;
double g_AttributeDefenceMultipler = 1.0;
HMODULE g_DllModule = 0;
time_t g_ServerStartTime = 0;
UINT g_ServerVersion = 100037;
WCHAR g_ExtenderRelease[64];
UINT g_DropGetReuse = 1000;
BOOL g_ShowEnterWorldHtml = FALSE;
BOOL g_OldSkillReuseFormula = FALSE;
INT32 g_CustomPremiumShopItemId = 0;
BOOL g_ExtInitialized = FALSE;
UINT g_ShutdownTime = 30;

vector<pair<WORD, WORD>> g_NamePatterns;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

int GetCurrentRevision()
{
	return g_ProtocolVersion;
}

HMODULE g_DllFileHandle = 0;
HANDLE g_RunningMutex = 0;

void InitializeExtender(HMODULE hModule)
{
	if(g_ExtInitialized == FALSE)
	{
		g_ExtInitialized = TRUE;
		g_RunningMutex = CreateMutex(0, false, _T("Global\\GraciaFinalExtender"));
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			MessageBox(0, _T("Server is already running!"), _T("Gracia Final Extension"), MB_OK);
			ExitProcess(-1);
			return;
		}
		//	wstringstream str;
		//	str << "Offset: " << hex << offsetof(CCreature, CCreature::lpPerfectDefenceTrait);
		//	ShowMessage(str.str().c_str(), L"test");
		
		g_ExtenderRelease[0] = 0;
		srand((UINT)time(NULL));
		GetCurrentDirectory(MAX_PATH,g_ConfigFile);
		lstrcat(g_ConfigFile, _T("\\ServerExt.ini"));
		g_DllModule = hModule;
		const WCHAR* sectionName = L"Setting";
		g_ServerStartTime = time(NULL);
		g_ProtocolVersion = GetPrivateProfileInt(sectionName, _T("ProtocolVersion"), 83, g_ConfigFile);
		g_EnableStatisticsPacket = GetPrivateProfileInt(sectionName, _T("EnableStatisticsPacket"), 1, g_ConfigFile);
		g_MaxMultiSellQuanity = GetPrivateProfileInt(sectionName, _T("MaxMultiSellQuanity"), 10000, g_ConfigFile);
		g_EnableExpStopCommand = GetPrivateProfileInt(sectionName, _T("EnableExpStopCommand"), 0, g_ConfigFile);
		DWORD deadlockTimeout = GetPrivateProfileInt(sectionName, _T("DeadlockTimeout"), 60000, g_ConfigFile);
		BOOL allowDropAugmentatedItems = GetPrivateProfileInt(sectionName, _T("AllowDropAugmentatedItems"), 0, g_ConfigFile);
		g_DropGetReuse = GetPrivateProfileInt(sectionName, _T("DropGetReuse"), 1000, g_ConfigFile);
		g_SpiritShotExtension = GetPrivateProfileInt(sectionName, _T("SpiritshotExtension"), FALSE, g_ConfigFile);
		g_OldSkillReuseFormula = GetPrivateProfileInt(sectionName, _T("OldSkillReuseFormula"), FALSE, g_ConfigFile);
		g_ShowEnterWorldHtml = GetPrivateProfileInt(sectionName, _T("ShowEnterWorldHtml"), FALSE, g_ConfigFile);
		g_DisableFameSystem = GetPrivateProfileInt(sectionName, _T("DisableFameSystem"), FALSE, g_ConfigFile);
		g_CustomPremiumShopItemId = GetPrivateProfileInt(sectionName, _T("CustomPremiumShopItemId"), 0, g_ConfigFile);
		g_ShutdownTime = GetPrivateProfileInt(sectionName, _T("ShutdownTime"), 30, g_ConfigFile);

		TCHAR temp[8190];
		if(GetPrivateProfileString(sectionName, _T("NamePattern"), 0, temp, 8190, g_ConfigFile))
		{
			wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
			wstring low, high;
			while(patternStream >> low >> high)
			{
				g_NamePatterns.push_back(pair<WORD, WORD>(low[0], high[0]));
			}
		}
		if(g_DisableFameSystem)
		{
			WriteNOP(0x89229B, 16);	//User::CanGetPVPPoint return always false

		}
		if(allowDropAugmentatedItems)
		{
			WriteJump(0x8B6D60, 0x8B6D8B, 3);
		}

		WriteDWORD(0x690053, deadlockTimeout);
		g_AttributeAttackMultipler = 1.0f;
		g_AttributeDefenceMultipler = 1.0f;
		g_AttributeDefenceMultipler *= g_AttributeDefenceMultipler;
		int bonus = GetPrivateProfileInt(L"SkillFx", _T("AttributeAttackMultipler"), 100, g_ConfigFile);
		g_AttributeAttackMultipler = (double)bonus;
		g_AttributeAttackMultipler /= 100.0;
		bonus = GetPrivateProfileInt(L"SkillFx", _T("AttributeDefenceMultipler"), 100, g_ConfigFile);
		g_AttributeDefenceMultipler = (double)bonus;

		g_AttributeDefenceMultipler /= 100.0;
		MovLogFiles(TEXT("LogBackup"));

		WriteCall(0x4889E1, GetCurrentRevision);
		WriteCall(0x57ECE9, GetCurrentRevision);
		WriteCall(0x6B3263, GetCurrentRevision);
		WriteCall(0x6B3980, GetCurrentRevision);
		WriteCall(0x933654, GetCurrentRevision);
		WriteCall(0x933B6C, GetCurrentRevision);

		//inserting deleted object
		WriteNOP(0x6AEA04, 5);
		WriteNOP(0x6AEC6C, 5);
		WriteNOP(0x6AEED4, 5);
		WriteNOP(0x6AF13C, 5);
		WriteNOP(0x6AF3A4, 5);

		//NOP Malloc (>32k), ClassName[%s], AllocSize[%d]
		WriteJump(0x6D321F, 0x6D323F, 27);
		//NOP [NO ERROR] L2Server Server Start
		WriteNOP(0x6B3423, 5);
		//NOP BuilredCmd [%s] duplicated
		WriteNOP(0x4E2954, 5);
		//\"eventdata.INI\" format is not valid  - it is valid
		WriteNOP(0x7A75CC, 5);
		//Move from err to In [NO_ERROR] Airship(%d) MOVE (%d [%d,%d,%d] / [%d,%d,%d]) (diff:%d) (cycle:%d)
		//.text:000000000042E43E                 mov     edx, 2
		WriteDWORD(0x42E43F, CLog::In);
		//callstack for CDominionManager::ChangeState - not needed
		WriteNOP(0x5EA1E2, 5);
		//TutorialEvent for 18 lvl to 28 lvl
		WriteBYTE(0x55C4DF, 28);

		g_ClassExt.Init();
		g_L2Time.Init();
		Appearance::Init();
		PostLoad::Init();
		BaseAction::Init();
		VitalitySystem::Init();
		CPledgeSystem::Init();
		CPledge::ExtInit();
		IOBufferPoolInit();
		IOBufferExPoolInit();
		DestroySectionHeader(false);
		CItemFactory::Init();
		g_Menu.Init();
	}
}

DLLEXPORT BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			InitializeExtender(hModule);

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			IOBufferPoolAtExit();
			IOBufferExPoolAtExit();
			if(g_DllFileHandle)
			{
				CloseHandle(g_DllFileHandle);
				g_DllFileHandle = 0;
			}
			break;
		}
	default:
		{
			break;
		}
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

