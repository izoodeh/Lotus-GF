#include "StdAfx.h"
#include "PostLoad.h"
#include "HookManager.h"
#include "UserPacketHandler.h"
#include "HtmlCmd.h"
#include "BuilderCmd.h"
#include "SchemeBuffer.h"
#include "Firewall.h"
#include "ObsceneFilter.h"
#include "ChatManager.h"
#include "DBPacketHandler.h"
#include "OlympiadSystem.h"
#include "Protector.h"
#include "AutoLoot.h"
#include "SpawnProtection.h"
#include "OfflineShop.h"
#include "SkillDB.h"
#include "SkillFx.h"
#include "AutoLearn.h"
#include "VoteSystem.h"
#include "DonateSystem.h"
#include "PvPSystem.h"
#include "ItemEnchant.h"
#include "UserCommandEx.h"
#include "ParserForSkill.h"
#include "AuthStat.h"
#include "PremiumService.h"
#include "EventName.h"
#include "NpcMod.h"
#include "World.h"
#include "FactionSystem.h"
#include "ItemDecay.h"
#include "ItemAutoConsume.h"
#include "Championship.h"
#include "TvT.h"
#include "AutoStart.h"
#include "Dungeons.h"
#include "ClassManager.h"
#include "TraitSystem.h"
#include "AutoAnnounce.h"
#include "CliExt.h"
#include "ItemLock.h"
#include "ItemReuseManager.h"
#include "TerritoryData.h"
#include "TeleportData.h"
#include "DominionManager.h"
#include "VoteReward.h"
#include "HtmlCache.h"
#include "AllInOne.h"
#include "SkillAcquireDB.h"

extern BOOL debuggerAttached;

void PostLoad::Init()
{
	g_ItemReuseManager.Init();
	WriteCall(0x6B18DB, PostLoad::InitBeforeGeodata);
	WriteCall(0x6B19A0, PostLoad::InitBeforeScriptDir);
	WriteCall(0x6B1F5A, PostLoad::InitAfterScriptDir);
	WriteCall(0x6B24D6, PostLoad::InitAfterCacheD);
	WriteCall(0x6B2791, PostLoad::InitWhenLoaded);
}

void PostLoad::InitBeforeGeodata()
{
	guard;
	/* Initialize stuff here - load builder cmd */
	DestroySectionHeader(true);
	g_CallStack.Init();
	g_BuilderCmd.Init();
	/* End */
	
	typedef void(*f)();
	f(0x4E2778L)();

	unguard;
}

void PostLoad::InitBeforeScriptDir(LPVOID pInstance, int type, const char* message)
{
	guard;
	/* Initializing stuff before script folder gets loaded */
	g_UserPacketHandler.Init();
	g_DBPacketHandler.Init();
	g_ChatManager.Init();
	g_UserCommandEx.Init();
	g_Firewall.Init();
	g_AuthStat.Init();
	g_HtmlCmd.Init();
	g_SkillFx.Init();
	g_ParserForSkill.Init();
	g_TraitSystem.Init();
	CCreature::Init();
	/* End */

	typedef void (*f)(LPVOID, int, const char*);
	f(0x6B9E14L)(pInstance, type, message);

	unguard;
}

void PostLoad::InitAfterScriptDir()
{
	guard;

	typedef void(*f)();
	f(0x69F9D4L)();

	/* Initialize stuff after script folder gets loaded */
	DeleteFile(L"Restart.exe");
	g_SkillAcquireDbEx.Init();
	g_SchemeBuffer.Init();
	g_EventName.Init();
	g_NpcMod.Init();
	g_Protector.Init();
	g_ObsceneFilter.Init();
	g_OlympiadSystem.Init();
	g_AutoLoot.Init();
	g_ItemDecay.Init();
	g_AutoLearn.Init();
	g_SpawnProtection.Init();
	g_OfflineShop.Init();
	g_VoteSystem.Init();
	g_DonateSystem.Init();
	g_ItemEnchant.Init();
	g_ItemAutoConsume.Init();
	g_FactionSystem.Init();
	g_DungeonSystem.Init();
	g_ClassManager.Init();
	CPremiumService::Init();
	g_TerritoryChecker.Init();
	g_TeleportData.Init();
	g_DominionManager.Init();
	g_VoteReward.Init();
	g_AllInOne.Init();
	g_HtmlCache.Init();


	/* End */
	unguard;
}

void PostLoad::InitAfterCacheD(LPVOID pInstance)
{
	guard;

	typedef void(*f)(LPVOID);
	f(0x76AD4CL)(pInstance);

	g_Championship.Init();
	g_TvT.Init();
	g_PvPSystem.Init();
	g_AutoStart.Init();
	g_AutoAnnounce.Init();
	g_CliExt.Init();
	g_ItemLock.Init();

	if(g_HtmlCache.IsEnabled())
	{
		if(g_HtmlCache.IsMultiLang())
		{
			for(UINT n=0;n<9;n++)
			{
				g_HtmlCache.LoadHtmls(n);
			}
		}else
		{
			g_HtmlCache.LoadHtmls();
		}
	}

	unguard;
}

void PostLoad::InitWhenLoaded(UINT param)
{
	guard;

	g_AutoStart.StartApps();

	typedef void (*f)(UINT);
	f(0x68FF7CL)(param);
	unguard;
}