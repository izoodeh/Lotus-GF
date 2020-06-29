#include "StdAfx.h"
#include "PVPSystem.h"
#include "PCTrap.h"
#include "DB.h"
#include "HTMLCacheManager.h"

CPvPSystem g_PvPSystem;

CPvPSystem::CPvPSystem() : enabled(FALSE)
{
	SetName(L"PvP System");
};

CPvPSystem::~CPvPSystem()
{

}

BOOL CPvPSystem::IsEnabled()
{
	return enabled;
}

BOOL CPvPSystem::IsStatistics()
{
	return statistics;
}

void CPvPSystem::Init()
{
	enabled = FALSE;
	season.startTime = 0;
	season.state = 0;
	loadTopPlayersTick = 5;
	LoadINI();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Statistics[%d]", GetName(), IsStatistics());
		g_DB.RequestLoadPvPSeason();
	
		wstringstream html;
		html << L"<html><head></head><body><center><font color=\"LEVEL\">Top " << topMonthWinners << L" - Previous PvP Season</font><br><br><br>";
		html << L"<table width=\"145\" border=\"0\">";
		Lock();
		UINT n = 1;
		for(list<wstring>::iterator Iter = topPlayers.begin(); Iter!= topPlayers.end(); Iter++)
		{
			html << L"<tr><td width=\"20\">" << n << L".</td><td width=\"125\">" << Iter->c_str() << L"</td></tr>";
		}
		Unlock();

		html << L"</table></center></body></html>";
		topPlayersHtml = html.str();
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
	}
}

void CPvPSystem::LoadINI()
{
	const TCHAR* sectionName = _T("PvPSystem");
	enabled = GetPrivateProfileInt(sectionName, _T("Enabled"), 0, g_ConfigFile);
	statistics = GetPrivateProfileInt(sectionName, _T("Statistics"), 0, g_ConfigFile);
	requiredLevel = GetPrivateProfileInt(sectionName, _T("RequiredLevel"), 60, g_ConfigFile);
	levelGap = GetPrivateProfileInt(sectionName, _T("LevelGap"), 15, g_ConfigFile);
	checkIPnMAC	= GetPrivateProfileInt(sectionName, _T("CheckIPnMAC"), 1, g_ConfigFile);
	killReuse = GetPrivateProfileInt(sectionName, _T("KillReuse"), 30, g_ConfigFile);
	pvpCommand = GetPrivateProfileInt(sectionName, _T("PvPCommand"), 1, g_ConfigFile);
	topMonthWinners = GetPrivateProfileInt(sectionName, _T("TopMonthWinners"), 1, g_ConfigFile);
	topMonthRewardId = GetPrivateProfileInt(sectionName, _T("TopMonthRewardId"), 1, g_ConfigFile);
	topMonthRewardCount = GetPrivateProfileInt(sectionName, _T("TopMonthRewardCount"), 1, g_ConfigFile);
	pvpTokenId = GetPrivateProfileInt(sectionName, _T("PvPTokenId"), 1, g_ConfigFile);
	pointIncreasedMsgId = GetPrivateProfileInt(sectionName, _T("PointIncreasedMsgId"), 0, g_ConfigFile);
}

void CPvPSystem::AddTopPlayer(wstring wName, bool first)
{
	Lock();
	if(first)
		topPlayers.clear();

	topPlayers.push_back(wName);

	wstringstream html;
	html << L"<html><head></head><body><center><font color=\"LEVEL\">Top " << topMonthWinners << L" - Previous PvP Season</font><br><br><br>";
	html << L"<table width=\"145\" border=\"0\">";
	Lock();
	UINT n = 1;
	for(list<wstring>::iterator Iter = topPlayers.begin(); Iter!= topPlayers.end(); Iter++)
	{
		html << L"<tr><td width=\"20\">" << n << L".</td><td width=\"125\">" << Iter->c_str() << L"</td></tr>";
		n++;
	}
	Unlock();

	html << L"</table></center></body></html>";
	topPlayersHtml = html.str();
	Unlock();
}

void CPvPSystem::OnDie(User *pUser, CCreature *pEnemy)
{
	if(enabled)
	{
		if(pUser->ValidUser() && pEnemy->ValidCreature())
		{
			User *pKiller = 0;
			if(pEnemy->IsUser())
			{
				pKiller = pEnemy->CastUser();
			}else if(pEnemy->IsPCTrap())
			{
				pKiller = pEnemy->CastPCTrap()->GetUserOrMaster();
			}else if(pEnemy->IsSummon())
			{
				pKiller = pEnemy->CastSummon()->GetMaster();
			}else if(pEnemy->IsPet())
			{
				pKiller = pEnemy->CastPet()->GetMaster();
			}
			if(pKiller->ValidUser())
			{
				if(statistics)
				{
					ReportDieStat(pUser, pKiller);
				}

				if(pUser->pSD->in_battlefield)
					return;


				if(pUser->pSD->pledge_id > 0 && pUser->pSD->pledge_id == pKiller->pSD->pledge_id)
					return;

				if(checkIPnMAC && pUser->pUserSocket->hardwareId.IsEqual(pKiller->pUserSocket->hardwareId))
				{
					return;
				}

				if(CAlliance* pAlliance = pUser->GetAlliance())
				{
					if(pAlliance == pKiller->GetAlliance())
						return;
				}

				if(pKiller->pSD->level < requiredLevel || pUser->pSD->level < requiredLevel)
					return;

				if(abs(pKiller->pSD->level - pUser->pSD->level) > levelGap)
					return;

				if(!pKiller->pSD->is_guilty || !pUser->pSD->is_guilty)
					return;

				if(killReuse > 0)
				{
					IPnMAC killerIPnMAC;
					killerIPnMAC.Part.ip = pKiller->pUserSocket->address.S_un.S_addr;
					killerIPnMAC.Part.mac = pKiller->pUserSocket->macAddress;
					IPnMAC victimIPnMAC;
					victimIPnMAC.Part.ip = pUser->pUserSocket->address.S_un.S_addr;
					victimIPnMAC.Part.mac = pUser->pUserSocket->macAddress;

					if(killReuseDb.SetGet(pKiller->databaseId, pUser->databaseId, killReuse, checkIPnMAC, killerIPnMAC, victimIPnMAC) > 0)
					{
						return;
					}
				}

				pKiller->pEUD->pvpData.points++;
				pKiller->pEUD->pvpData.saveFlag = TRUE;

				if(pointIncreasedMsgId > 0)
				{
					pKiller->pUserSocket->SendSystemMessage(pointIncreasedMsgId);
				}

			}
		}
	}
}

void CPvPSystem::ReportDieStat(User *pUser, User *pEnemy)
{
	if(pUser && pEnemy)
	{
		int victimId = pUser->databaseId;
		int victimLevel = pUser->pSD->level;
		int victimClassId = pUser->pSD->occupation;

		int killerId = pEnemy->databaseId;
		int killerClassId = pEnemy->pSD->occupation;
		int killerLevel = pEnemy->pSD->level;
		int killerHp = (int)pEnemy->pSD->hp;
		int killerMaxHp = (int)pEnemy->pSD->max_hp;
		int killerCp = (int)pEnemy->pSD->cp;
		int killerMaxCp = (int)pEnemy->pSD->max_cp;

		int skillId = pEnemy->pEUD->currentActivatedSkillId;
		int skillLevel = pEnemy->pEUD->currentActivatedSkillLevel;

		int pvpType = PT_PK;

		IVector pos(pUser->pSD->pos);

		if(pUser->pSD->in_battlefield)
			pvpType = PT_PVP;
		else if(pUser->pSD->is_guilty)
			pvpType = PT_PVP;
		else if(pUser->pSD->karma > 0)
			pvpType = PT_KARMA;
		else if(pUser->pSD->karma > 0 && pEnemy->pSD->karma > 0)
			pvpType = PT_BOTH_KARMA;

		if(pUser->CheckMyPledgeDeclareWar(pEnemy) && pEnemy->CheckMyPledgeDeclareWar(pUser))
		{
			pvpType = PT_CLAN_WAR;
		}

		g_DB.RequestSavePvPStat(victimId, victimLevel, victimClassId, killerId,  killerClassId, killerLevel, killerHp, killerMaxHp, killerCp, killerMaxCp, pvpType, pos.x, pos.y, pos.z, skillId, skillLevel, _time32(NULL));
	}
}

void CPvPSystem::ConvertTransferredPoints(User *pUser)
{
	if(enabled && pUser->ValidUser())
	{
		if(pUser->pEUD->pvpData.transferredPoints > 0)
		{
			if(pvpTokenId)
			{
				if(pUser->inventory.CheckAddable(pvpTokenId, pUser->pEUD->pvpData.transferredPoints))
				{
					pUser->AddItemToInventory(pvpTokenId, pUser->pEUD->pvpData.transferredPoints);
					g_Log.Add(CLog::Audit, "Converting transferred pvp points[%d] into tokens[%d] for user[%S]", pUser->pEUD->pvpData.transferredPoints, pvpTokenId, pUser->pSD->wName);
					pUser->pEUD->pvpData.transferredPoints = 0;
					g_DB.RequestSaveTransferredPvPPoints(pUser->databaseId, 0);
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"Cannot add items to inventory!");
				}
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"You've got not enough points to perform this action.");
		}
	}
}

bool CPvPSystem::HandleCmd(User *pUser, const WCHAR *wParam)
{
	if(pUser->ValidUser() && pvpCommand)
	{
		size_t len = wcslen(wParam);
		if(len > 0)
		{
			if(!wcscmp(wParam, L"convert"))
			{
				ConvertTransferredPoints(pUser);
				return true;
			}else if(!wcscmp(wParam, L"points"))
			{
				RequestShowPoints(pUser);
				return true;
			}else if(!wcscmp(wParam, L"reward"))
			{
				if(pUser->pEUD->pvpData.topPlayer && pUser->pEUD->pvpData.receivedReward == 0)
				{
					if(topMonthRewardId > 0 && topMonthRewardCount > 0)
					{
						if(pUser->inventory.CheckAddable(topMonthRewardId, topMonthRewardCount))
						{
							pUser->pUserSocket->SendSystemMessage(L"Congratulation you won the reward in PvP System!");
							pUser->AddItemToInventory(topMonthRewardId, topMonthRewardCount);
							g_DB.RequestSetPvPRewardFlag(pUser->databaseId);
							g_Log.Add(CLog::Audit, "User[%S] received pvp reward[%d][%d]", pUser->pSD->wName, topMonthRewardId, topMonthRewardCount);
							pUser->pEUD->pvpData.receivedReward = TRUE;
						}else
						{
							pUser->pUserSocket->SendSystemMessage(L"In order to receive pvp reward, please clean up your inventory.");
						}
					}
				}
				return true;
			}
		}

		const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"pvp.htm", pUser->pSD->language);
		if(wHtml)
		{
			pUser->ShowHTML(L"pvp.htm", wHtml);
			return true;
		}else
		{
			g_Log.Add(CLog::Error, "[%S] Cannot load pvp.htm! - check HTML directory!", GetName());
		}
	}
	return false;
}

void CPvPSystem::OnLoadSeason(BOOL initialized, UINT id, UINT state, UINT startTime)
{
	if(enabled)
	{
		if(initialized)
		{
			season.id = id;
			season.startTime = startTime;
			season.state = state;
		}else
		{
			__time32_t current = _time32(0);
			season.id = 1;
			season.startTime = current;
			season.state = 0;

			//start new season
			g_DB.RequestProcessPvPSeason(current, topMonthWinners);
			
			wstringstream announceStream;
			announceStream << L"Period " << season.id << " of the PvP system has started!";
			BroadcastToAllUser("cddSS", 0x4A, 0, 18, L"", announceStream.str().c_str());
		}
	}
}

void CPvPSystem::OnLoadData(User *pUser, INT32 points, INT32 lastSeasonPoints, INT32 transferredPoints, BOOL topPlayer, BOOL receivedReward)
{
	if(enabled && pUser)
	{
		pUser->pEUD->pvpData.points = points;
		pUser->pEUD->pvpData.lastSeasonPoints = lastSeasonPoints;
		pUser->pEUD->pvpData.transferredPoints = transferredPoints;
		pUser->pEUD->pvpData.topPlayer = topPlayer;
		pUser->pEUD->pvpData.receivedReward = receivedReward;
		if(topPlayer && receivedReward == FALSE)
		{
			if(topMonthRewardId > 0 && topMonthRewardCount > 0)
			{
				if(pUser->inventory.CheckAddable(topMonthRewardId, topMonthRewardCount))
				{
					pUser->pUserSocket->SendSystemMessage(L"Congratulation you won the reward in PvP System!");
					pUser->AddItemToInventory(topMonthRewardId,topMonthRewardCount);
					g_DB.RequestSetPvPRewardFlag(pUser->databaseId);
					g_Log.Add(CLog::Audit, "User[%S] received pvp reward[%d][%d]", pUser->pSD->wName, topMonthRewardId, topMonthRewardCount);
					pUser->pEUD->pvpData.receivedReward = TRUE;
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"In order to receive pvp reward, please clean up your inventory.");
				}
			}
		}
	}
}

void CPvPSystem::TimerExpired()
{
	guard;
	if(enabled)
	{
		if(season.startTime > 0 && season.state == 0)
		{
			tm ti; 
			time_t rawtime;
			time ( &rawtime );
			localtime_s(&ti, &rawtime );

			__time32_t nextSeasonTime = static_cast<__time32_t>(Utils::ConvertTime(ti.tm_year + 1900, ti.tm_mon, 1, 0, 0, 0));
			
			loadTopPlayersTick--;
			if(loadTopPlayersTick == 0)
			{
				loadTopPlayersTick = 300;
				g_DB.RequestLoadPvPTopPlayers();
			}

			if(season.startTime < nextSeasonTime)
			{
				g_DB.RequestProcessPvPSeason(nextSeasonTime, topMonthWinners);
				loadTopPlayersTick = 5;
				
				wstringstream announceStream;
				announceStream << L"Period " << season.id << " of the PvP system has ended!";
				BroadcastToAllUser("cddSS", 0x4A, 0, 18, L"", announceStream.str().c_str());
				
				g_Log.Add(CLog::Blue, "[%s] Starting new pvp season [%d][%d]", __FUNCTION__, season.startTime, nextSeasonTime);
				//start new season
				season.id++;
				season.startTime = nextSeasonTime;
				season.state = 0;

				wstringstream announceStream2;
				announceStream2 << L"Period " << season.id << " of the PvP system has started!";
				BroadcastToAllUser("cddSS", 0x4A, 0, 18, L"", announceStream2.str().c_str());
			}
		}
	}
	unguard;
}

void CPvPSystem::RequestShowTopPlayers(User *pUser)
{
	guard;

	if(enabled && pUser)
	{
		wstring html;
		Lock();
		html = topPlayersHtml;
		Unlock();

		pUser->ShowHTML(L"pvp_top_players.htm", html.c_str());
	}
	unguard;
}

void CPvPSystem::RequestShowPoints(User *pUser)
{
	guard;

	if(enabled && pUser)
	{
		wstringstream str;
		str << L"You have accumulated " << pUser->pEUD->pvpData.points << L" pvp point(s) in current PvP period.";
		pUser->pUserSocket->SendSystemMessage(GetName(), str.str().c_str());

		wstringstream str2;
		str2 << L"You can convert " << pUser->pEUD->pvpData.transferredPoints << L" pvp point(s) accumulated in previous PvP period(s).";
		pUser->pUserSocket->SendSystemMessage(GetName(), str2.str().c_str());
	}

	unguard;
}