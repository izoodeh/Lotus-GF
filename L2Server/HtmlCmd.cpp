#include "StdAfx.h"
#include "HtmlCmd.h"
#include "HTMLCacheManager.h"
#include "SpawnProtection.h"
#include "SchemeBuffer.h"
#include "HookManager.h"
#include "TvT.h"
#include "PvPSystem.h"
#include "Dungeons.h"
#include "ClassManager.h"
#include "OfflineShop.h"
#include "AutoLoot.h"
#include "ItemLock.h"
#include "TeleportData.h"
#include "AllInOne.h"

CHtmlCmd g_HtmlCmd;

LPBYTE g_HtmlLinkCheck = (LPBYTE)0xF0D123;
LPBYTE g_LogOnInvalidLink = (LPBYTE)0xF0D124;
LPBYTE g_KickOnInvalidLink = (LPBYTE)0xF0D125;
extern BOOL g_EnableExpStopCommand;

HtmlCmdInfo htmlCmdInfoArray[HTML_CMD_LIMIT];

CHtmlCmd::CHtmlCmd() : handlerCount(0)
{
	SetName(L"HTML Command");
}

CHtmlCmd::~CHtmlCmd()
{
}

void CHtmlCmd::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	memset(htmlCmdInfoArray, 0, sizeof(htmlCmdInfoArray));

	//fix kick on invalid option
	WriteBYTE(0x948E4D, 0xB0);
	WriteBYTE(0x948E4E, 0x01);

	//new handlers
	AddHandler(L"sb_bp", HtmlCmdBuffPlayer);
	AddHandler(L"sb_bs", HtmlCmdBuffSummon);
	AddHandler(L"sb_ab", HtmlCmdAddBuff);
	AddHandler(L"sb_asb", HtmlCmdAddSummonBuff);
	AddHandler(L"sb_ubl", HtmlCmdUseBuffList);
	AddHandler(L"sb_usbl", HtmlCmdUseSummonBuffList);
	AddHandler(L"sb_cbl", HtmlCmdClearBuffList);
	AddHandler(L"sb_csbl", HtmlCmdClearSummonBuffList);
	AddHandler(L"sb_dispel", HtmlCmdBufferDispelAll);
	AddHandler(L"sb_s_dispel", HtmlCmdBufferSummonDispelAll);
	AddHandler(L"sb_healthy", HtmlCmdBufferHealthy);
	AddHandler(L"sb_s_healthy", HtmlCmdBufferSummonHealthy);

	AddHandler(L"tvt_register", HtmlCmdTvTRegister);
	AddHandler(L"tvt_unregister", HtmlCmdTvTUnregister);

	AddHandler(L"exp_mode", HtmlCmdExpMode);
	AddHandler(L"offline_shop", HtmlCmdOfflineShop);

	AddHandler(L"delevel_player", HtmlCmdDelevelPlayer);

	AddHandler(L"pvp_points", HtmlCmdPvPPoints);
	AddHandler(L"pvp_convert", HtmlCmdPvPConvertPoints);
	AddHandler(L"pvp_top_players", HtmlCmdPvPTopPlayers);

	AddHandler(L"dungeons_enter", HtmlCmdDungeonsEnter);

	AddHandler(L"cm_show", HtmlCmdClassManagerShow);
	AddHandler(L"cm_change", HtmlCmdClassManagerChange);
	AddHandler(L"cm_nobless", HtmlCmdClassManagerNobless);
	AddHandler(L"cm_subclass", HtmlCmdClassManagerSubClass);
	AddHandler(L"auto_loot", HtmlCmdAutoLoot);
	AddHandler(L"il_", HtmlCmdItemLock);
	AddHandler(L"set_level", HtmlCmdSetLevel);
	AddHandler(L"teleport_player", HtmlCmdTeleportPlayer);
	AddHandler(L"acquire_aio", HtmlCmdAcquireAiO);

	g_Log.Add(CLog::Blue, "[%S] Loaded [%d] new handlers", GetName(), handlerCount);
}

bool CHtmlCmd::IsSchemeBufferBypass(WCHAR* wBypass)
{
	if(!wcsncmp(L"sb_", wBypass, 3) )
	{
		return true;
	}
	return false;
}

void CHtmlCmd::AddHandler(const WCHAR* wName, HtmlCmdCallback func)
{
	if(wName && func)
	{
		if(handlerCount < HTML_CMD_LIMIT)
		{
			for(int n = 0; n < handlerCount;n++)
			{
				if(!wcscmp(htmlCmdInfoArray[handlerCount].wName, wName))
				{
					g_Log.Add(CLog::Error, "[%S] Handler[%S] already exist - cannot add!", GetName(), wName);
					return;
				}
			}

			wcscpy_s(htmlCmdInfoArray[handlerCount].wName, 64, wName);
			htmlCmdInfoArray[handlerCount].len = wcslen(wName);
			htmlCmdInfoArray[handlerCount].func = func;
			handlerCount++;
		}else
		{
			g_Log.Add(CLog::Error, "[%S] handlerCount reached the limit[%d]!", GetName(), HTML_CMD_LIMIT);
		}
	}
}

bool CHtmlCmd::Handle(CUserSocket *pSocket, User *pUser, WCHAR* wBypass, const unsigned char* packet)
{
	try
	{
		if(pSocket->ValidUserSocket() && pUser->ValidUser() && wBypass)
		{
			if(pUser->IsNowTrade())
				pUser->TradeCancel();

			if(pUser->pEUD->spawnProtection)
			{
				g_SpawnProtection.Deactivate(pUser);
			}

			//check for benom teleport and territory war flag
			if(!wcscmp(wBypass, L"menu_select?reply=79"))
			{
				if(CCreature *pTarget = CCreature::GetCreatureByObjectId(&pUser->talkTargetId))
				{
					if(pTarget->pSD->npc_class_id == 1035506)
					{
						if(CSharedItemData *pWD = pUser->GetEquipedWeapon())
						{
							if(CItemInfo *pWI = CItemInfo::Get(pWD))
							{
								if(pWI->weaponType == WeaponFlag)
								{
									pUser->pUserSocket->SendSystemMessage(L"You cannot visit the Benom room while equipping a flag!");
									return false;
								}
							}
						}
					}
				}
			}

			//Handle
			for(int n = 0; n < handlerCount; n++)
			{
				size_t len = htmlCmdInfoArray[n].len;
				if( !wcsncmp(htmlCmdInfoArray[n].wName, wBypass, len) )
				{
					//Check link
					if((*g_HtmlLinkCheck))
					{
						bool htmlCheck = true;

						if(IsSchemeBufferBypass(wBypass) && g_SchemeBuffer.IsHtmlLinkCheck() == FALSE)
						{
							htmlCheck = false;
						}

						if(htmlCheck)
						{
							if( pUser->expectingHtmlReply )
							{
								if( !g_HTMLCacheManager.IsValidLink(pUser->vExpectedHtmlReply, wBypass) )
								{
									if( (*g_LogOnInvalidLink) )
									{
										g_Log.Add(CLog::Error, "[%s] Invalid link on HTML - user[%S] link[%S]", __FUNCTION__, pUser->pSD->wName, wBypass);
									}
									if( (*g_KickOnInvalidLink) )
									{
										return true;
									}
									return false;	//drop packet
								}
							}
						}
					}
					return htmlCmdInfoArray[n].func(pSocket, pUser, &wBypass[len], wBypass );
				}
			}
			//not found then use org handler
			return UserCallback(0x948BD4L)(pSocket, packet);
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
	return false;
}

bool CHtmlCmd::UseValidTalkTarget(User *pUser)
{
	if(pUser->ValidUser())
	{
		try
		{
			bool valid = false;
			pUser->lpYieldLock->Enter(__FILEW__, __LINE__);
			CCreature *pTarget = CCreature::GetCreatureByObjectId(&pUser->talkTargetId);
			if(pTarget)
			{
				if( pTarget->Is(0x150) )
				{
					if( pTarget->Is64(0x1E8) )	//CastNpc
					{
						//compare x/y
						FVector pos1(pUser->pSD->pos);
						FVector pos2(pTarget->pSD->pos);
						double x = pos1.x - pos2.x;
						x *= x;
						double y = pos1.y - pos2.y;
						y *= y;

						if( (x + y) < 62500.0 )
						{
							valid = true;
						}
					}
				}
			}
			pUser->lpYieldLock->Leave();
			
			return valid;
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
}
bool HtmlCmdAcquireAiO(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR* wBypass)
{
	guard;
	if(g_AllInOne.Enabled())
	{
		UINT aioId = GetBypassValueInt(wBypass, L"id");
		if(aioId > 0)
		{
			g_AllInOne.Acquire(pUser, aioId);
		}
	}
	unguard;
	return false;
}

bool HtmlCmdTeleportPlayer(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR* wBypass)
{
	guard;
	if(g_TeleportData.IsEnabled())
	{
		if(g_TeleportData.IsAllowRemote() || CHtmlCmd::UseValidTalkTarget(pUser))
		{
			UINT teleportId = GetBypassValueInt(wBypass, L"id");
			if(teleportId > 0)
			{
				g_TeleportData.Handle(pUser, teleportId);
			}
		}
	}
	unguard;
	return false;
}

bool HtmlCmdSetLevel(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	guard;
	if(pUser->ValidUser())
	{
		if(CHtmlCmd::UseValidTalkTarget(pUser))
		{
			INT32 level = GetBypassValueInt(wBypass, L"level", -1);
			INT32 priceId = GetBypassValueInt(wBypass, L"priceId", 0);
			INT32 priceAmount = GetBypassValueInt(wBypass, L"priceAmount", 0);
			if(priceId > 0 && priceAmount > 0)
			{
				if(pUser->inventory.HaveItemByClassId(priceId, priceAmount))
				{
					pUser->DeleteItemInInventory(priceId, priceAmount);
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You don't have enough required item(s).");
					unguard;
					return false;
				}
			}
			if(level > 0 && level < 86)
			{
				pUser->SetLevel(level);
				g_Log.Add(CLog::Audit, "User[%s] changed level to [%d] through set_level - price[%d][%d].", pUser->pSD->wName, level, priceId, priceAmount);
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
		}
	}
	unguard;
	return false;
}

bool HtmlCmdItemLock(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	guard;
	bool ret = false;
	if(pUser->ValidUser())
	{
		ret = g_ItemLock.HandleBypass(pUser, wBypass);
	}
	unguard;
	return ret;
}

bool HtmlCmdOfflineShop(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		g_OfflineShop.RequestActivate(pUser);
	}
	return false;
}

bool HtmlCmdAutoLoot(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(g_AutoLoot.IsEnabled())
		{
			BOOL enable = GetBypassValueInt(wBypass, L"enable", 0);
			if(enable)
			{
				if(!pUser->pEUD->autoLoot)
				{
					pUser->pEUD->autoLoot = TRUE;
					pUser->pUserSocket->SendSystemMessage(L"The Auto Loot has been turned on.");
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"The Auto Loot is already turned on.");
				}
			}else
			{
				if(pUser->pEUD->autoLoot)
				{
					pUser->pEUD->autoLoot = FALSE;
					pUser->pUserSocket->SendSystemMessage(L"The Auto Loot has been turned off.");
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"The Auto Loot is already turned off.");
				}
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"The Auto Loot feature is disabled.");
		}
	}
	return false;
}

bool HtmlCmdClassManagerSubClass(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		g_ClassManager.RequestSubClassQuest(pUser);
	}
	return false;
}

bool HtmlCmdClassManagerNobless(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		g_ClassManager.RequestNobless(pUser);

	}
	return false;
}

bool HtmlCmdClassManagerShow(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		g_ClassManager.RequestShowHtml(pUser);	
	}
	return false;
}

bool HtmlCmdClassManagerChange(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		INT32 classId = GetBypassValueInt(wBypass, L"class", -1);
		if(classId > -1)
		{
			g_ClassManager.RequestChange(pUser, classId);
		}
	}
	return false;
}

bool HtmlCmdDungeonsEnter(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->builder_level > 0 || CHtmlCmd::UseValidTalkTarget(pUser))
		{
			UINT dungeonId = GetBypassValueInt(wBypass, L"id", 0);
			if(dungeonId > 0)
			{
				g_DungeonSystem.RequestEnter(pUser, dungeonId);
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
		}
	}
	return false;
}

bool HtmlCmdPvPPoints(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->builder_level > 0 || CHtmlCmd::UseValidTalkTarget(pUser))
		{
			g_PvPSystem.RequestShowPoints(pUser);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
		}
	}
	return false;
}

bool HtmlCmdPvPConvertPoints(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->builder_level > 0 || CHtmlCmd::UseValidTalkTarget(pUser))
		{
			g_PvPSystem.ConvertTransferredPoints(pUser);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
		}
	}
	return false;
}

bool HtmlCmdPvPTopPlayers(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->builder_level > 0 || CHtmlCmd::UseValidTalkTarget(pUser))
		{
			g_PvPSystem.RequestShowTopPlayers(pUser);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
		}
	}
	return false;
}

bool HtmlCmdTvTRegister(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		UINT team = GetBypassValueInt(wBypass, L"team", 0);
		if(team < 3)
		{
			g_TvT.Register(pUser, team);
		}
	}
	return false;
}

bool HtmlCmdTvTUnregister(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		UINT team = GetBypassValueInt(wBypass, L"team", 0);
		if(team < 3)
		{
			g_TvT.Unregister(pUser);
		}
	}
	return false;
}

bool HtmlCmdExpMode(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		BOOL state = GetBypassValueInt(wBypass, L"state", 0);
		if( g_EnableExpStopCommand )
		{
			if(pUser->pEUD->blockExp == FALSE && state)
			{
				pUser->pEUD->blockExp = TRUE;
				pSocket->SendSystemMessage(L"System", L"Stop Exp mode has been enabled.");
			}else if(pUser->pEUD->blockExp == TRUE && !state)
			{
				pUser->pEUD->blockExp = FALSE;
				pSocket->SendSystemMessage(L"System", L"Stop Exp mode has been disabled.");
			}
		}else
		{
			pSocket->SendSystemMessage(L"System", L"Stop Exp command has been disabled.");
		}
	}
	return false;
}

bool HtmlCmdBufferDispelAll(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->karma > 0 && !g_SchemeBuffer.IsAllowPk())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return false;
		}
		if(pUser->pSD->is_in_combat_mode && !g_SchemeBuffer.IsAllowCombat())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return false;
		}
		if(pUser->pSD->is_guilty && !g_SchemeBuffer.IsAllowGuilty())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return false;
		}

		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				pUser->DispelAllByGM();
				pUser->pUserSocket->SendSystemMessage(L"Your buffs have been cancelled.");
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdBufferSummonDispelAll(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->karma > 0 && !g_SchemeBuffer.IsAllowPk())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return false;
		}
		if(pUser->pSD->is_in_combat_mode && !g_SchemeBuffer.IsAllowCombat())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return false;
		}
		if(pUser->pSD->is_guilty && !g_SchemeBuffer.IsAllowGuilty())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return false;
		}

		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				if(CSummon *pSummon = pUser->GetSummonOrPet())
				{
					if(pSummon->IsInRange(pUser->pSD->pos, 500.0))
					{
						pSummon->DispelAllByGM();
						pUser->pUserSocket->SendSystemMessage(L"Your summon's buffs have been cancelled.");
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"Your summon is too far.");
					}
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdBufferHealthy(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->karma > 0 && !g_SchemeBuffer.IsAllowPk())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return false;
		}
		if(pUser->pSD->is_in_combat_mode && !g_SchemeBuffer.IsAllowCombat())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return false;
		}
		if(pUser->pSD->is_guilty && !g_SchemeBuffer.IsAllowGuilty())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return false;
		}

		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				pUser->lpYieldLock->Enter(__FILEW__, __LINE__);
				pUser->pSD->hp = pUser->pSD->max_hp;
				pUser->pSD->cp = pUser->pSD->max_cp;
				pUser->pSD->mp = pUser->pSD->max_mp;
				pUser->lpYieldLock->Leave();
				pUser->UpdateStat(STAT_HP);
				pUser->UpdateStat(STAT_MP);
				pUser->UpdateStat(STAT_HPnCP);
				pUser->pUserSocket->SendSystemMessage(L"Your combat, health and mana points have been restored.");
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdBufferSummonHealthy(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		if(pUser->pSD->karma > 0 && !g_SchemeBuffer.IsAllowPk())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when having karma!");
			return false;
		}
		if(pUser->pSD->is_in_combat_mode && !g_SchemeBuffer.IsAllowCombat())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that in combat mode!");
			return false;
		}
		if(pUser->pSD->is_guilty && !g_SchemeBuffer.IsAllowGuilty())
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot do that when being flagged!");
			return false;
		}

		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				if(CSummon *pSummon = pUser->GetSummonOrPet())
				{
					if(pSummon->IsInRange(pUser->pSD->pos, 500.0))
					{
						pSummon->lpYieldLock->Enter(__FILEW__, __LINE__);
						pSummon->pSD->hp = pSummon->pSD->max_hp;
						pSummon->pSD->mp = pSummon->pSD->max_mp;
						pSummon->lpYieldLock->Leave();
						pSummon->UpdateStat(STAT_HP);
						pSummon->UpdateStat(STAT_MP);
						pUser->pUserSocket->SendSystemMessage(L"Your summon's hp and mp have been restored.");
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"Your summon is too far.");
					}
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdDelevelPlayer(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				INT32 level = GetBypassValueInt(wBypass, L"level", 0);
				if(level > 0 && level < 85)
				{
					if(pUser->pSD->level >= level)
					{
						PINT64 lpExpTable = (PINT64)0xA7C388;
						INT32 oldLevel = pUser->pSD->level;
						pUser->SetExp(lpExpTable[level]);
						pUser->ValidateParameters();
						g_Log.Add(CLog::Black, "User[%S] used delevel bypass [%d] to [%d] level", pUser->pSD->wName, oldLevel, level);
					}
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdClearSummonBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				pUser->pEUD->schemeBuffList.Clear(true);
				pSocket->SendSystemMessage(3008);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdClearBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				pUser->pEUD->schemeBuffList.Clear();
				pSocket->SendSystemMessage(3007);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdAddBuff(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				int skillClassId = GetBypassValueInt(wBypass, L"pch_id", 0);
				if(skillClassId != 0)
				{
					g_SchemeBuffer.AddBuff(pUser, skillClassId, false);
				}else
				{
					g_Log.Add(CLog::Error, "[%s] Invalid pch_id in bypass[%S] - user[%S]", __FUNCTION__, wBypass, pUser->pSD->wName);
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdAddSummonBuff(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				int skillClassId = GetBypassValueInt(wBypass, L"pch_id", 0);
				if(skillClassId != 0)
				{
					g_SchemeBuffer.AddBuff(pUser, skillClassId, true);
				}else
				{
					g_Log.Add(CLog::Error, "[%s] Invalid pch_id in bypass[%S] - user[%S]", __FUNCTION__, wBypass, pUser->pSD->wName);
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdUseBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				g_SchemeBuffer.UseList(pUser, false);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};

bool HtmlCmdUseSummonBuffList(CUserSocket *pSocket, User *pUser, const WCHAR* wParams, const WCHAR *wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				g_SchemeBuffer.UseList(pUser, true);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
};


bool HtmlCmdBuffPlayer(CUserSocket *pSocket, User *pUser, const WCHAR *wParams, const WCHAR* wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				int skillClassId = GetBypassValueInt(wBypass, L"pch_id", 0);
				if(skillClassId != 0)
				{
					g_SchemeBuffer.BuffPlayer(pUser, skillClassId);
				}else
				{
					g_Log.Add(CLog::Error, "[%s] Invalid pch_id in bypass[%S] - user[%S]", __FUNCTION__, wBypass, pUser->pSD->wName);
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
}

bool HtmlCmdBuffSummon(CUserSocket *pSocket, User *pUser, const WCHAR *wParams, const WCHAR* wBypass)
{
	if(pUser->ValidUser())
	{
		try
		{
			if(pUser->pSD->builder_level > 0 || g_SchemeBuffer.IsValidateTalkTarget() == FALSE || CHtmlCmd::UseValidTalkTarget(pUser))
			{
				int skillClassId = GetBypassValueInt(wBypass, L"pch_id", 0);
				if(skillClassId != 0)
				{
					g_SchemeBuffer.BuffSummon(pUser, skillClassId);
				}else
				{
					g_Log.Add(CLog::Error, "[%s] Invalid pch_id in bypass[%S] - user[%S]", __FUNCTION__, wBypass, pUser->pSD->wName);
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid talk target - user[%S]", __FUNCTION__, pUser->pSD->wName);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
	return false;
}