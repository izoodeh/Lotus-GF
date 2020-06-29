#include "stdafx.h"
#include "TeleportData.h"
#include "UserAction.h"

CTeleportData g_TeleportData;

CTeleportData::CTeleportData() : m_Enabled(FALSE)
{
}

CTeleportData::~CTeleportData()
{
}

void CTeleportData::Init()
{
	LoadData();
	if(m_Enabled)
	{
		g_Log.Add(CLog::Blue, "[%s] Loaded [%d] teleport(s).", __FUNCTION__, m_Teleports.size());
	}
}

void CTeleportData::LoadData()
{
	guard;
	
	LPBYTE lpFile = 0;
	UINT len = Utils::ReadFileBinary("..\\Script\\TeleportData.txt", lpFile);

	m_Teleports.clear();
	if(lpFile && len > 2)
	{
		wstring line;
		wstringstream file((PWCHAR)&lpFile[2]);
		while(getline(file, line))
		{
			if( line[0] == L'/' || line[0] == L';' )
					continue;

			if( line.find(L"setting_begin") == 0 )
			{
				//setting_begin	enabled=1 show_animation=1	to_low_level_msg=2344	to_high_level_msg=2345	not_enough_adena_msg=2346	setting_end
				m_Enabled = ParseOptionInt(line, L"enabled", 0);
				m_AllowPK = ParseOptionInt(line, L"allow_pk", 0);
				m_AllowGuilty = ParseOptionInt(line, L"allow_guilty", 0);
				m_AllowCombat = ParseOptionInt(line, L"allow_combat", 0);
				m_AllowRemote = ParseOptionInt(line, L"allow_remote", 0);
				continue;
			}

			if( line.find(L"teleport_begin") == 0 )
			{
				//teleport_begin	teleport_id=1	location=83400/147943/-3404	min_level=1	max_level=80	price_id=1458	price_amount=5	castle_id=3	teleport_end
				TeleportInfo ti;
				ti.id = ParseOptionInt(line, L"teleport_id", 0);
				ti.castleId = ParseOptionInt(line, L"castle_id", 0);
				wstring loc = ParseOptionString(line, L"location", L"0/0/0");
				loc = ReplaceChar(loc, L"/", L" ");
				wstringstream locationStream(loc);
				locationStream >> ti.loc.x >> ti.loc.y >> ti.loc.z;
				ti.minLevel = ParseOptionInt(line, L"min_level");
				ti.maxLevel = ParseOptionInt(line, L"max_level", 90);
				ti.price.itemId = ParseOptionInt(line, L"price_id");
				ti.price.amount = ParseOptionInt(line, L"price_amount");
				if(ti.id > 0)
				{
					m_Teleports[ti.id] = ti;
				}
			}
		}
	}

	unguard;
}

bool CTeleportData::HandleReply(User *pUser, UINT messageId, UINT reply)
{
	guard;
	if(m_Enabled && messageId == 811)
	{
		if(pUser->pEUD->teleportUser.ti.id > 0)
		{
			if(reply)
			{
				Handle(pUser, pUser->pEUD->teleportUser.ti.id, true);
			}else
			{
				pUser->pEUD->teleportUser.ti.clear();
				pUser->pEUD->teleportUser.timeoutTick = 0;
			}
			unguard;
			return true;
		}
	}
	unguard;
	return false;
}

void CTeleportData::Handle(User *pUser, UINT teleportId, bool execute)
{
	guard;
	if(m_Enabled)
	{
		if(pUser->pSD->alive && pUser->pEUD->offlineShop == FALSE && pUser->instantZoneId == 0)
		{
			if(pUser->olympiadUser.status == 0)
			{
				if(pUser->pSD->is_in_combat_mode == false || m_AllowCombat)
				{
					if(pUser->pSD->is_guilty == false || m_AllowGuilty)
					{
						if(pUser->pSD->karma == 0 || m_AllowPK)
						{
							map<UINT, TeleportInfo>::iterator it = m_Teleports.find(teleportId);
							if(it!=m_Teleports.end())
							{
								TeleportInfo& ti = it->second;
								if(ti.minLevel <= pUser->pSD->level && ti.maxLevel >= pUser->pSD->level)
								{
									if( (ti.price.itemId > 0 && ti.price.amount > 0 && pUser->inventory.HaveItemByClassId(ti.price.itemId, ti.price.amount)) || (ti.price.itemId == 0 || ti.price.amount == 0) )
									{
										if(ti.price.itemId > 0 && ti.price.amount > 0 && execute)
										{
											pUser->DeleteItemInInventory(ti.price.itemId, ti.price.amount);
										}

										if(!execute)
										{
											pUser->pEUD->teleportUser.ti = ti;
											pUser->pEUD->teleportUser.timeoutTick = _time32(0) + 15;
											//811	1	a,You will be moved to ($s1). Do you wish to continue?\0	4	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,popup\0
											CHAR buff[8190];
											int len = Assemble(buff, sizeof(buff), "cdddddddd", 0xF3, 811, 1, 7, ti.loc.x, ti.loc.y, ti.loc.z, 9000, 0);
											if(len > 0)
											{
												pUser->pUserSocket->Send("b", len, buff);
											}
										}else
										{
											TeleportUser& tu = pUser->pEUD->teleportUser;
											if(tu.ti.id > 0 && tu.timeoutTick > time(0))
											{
												UserAction::OnTeleportToLocation(pUser, ti.loc.x, ti.loc.y, ti.loc.z, pUser->instantZoneId, 0x10000);
												tu.ti.clear();
												tu.timeoutTick = 0;
											}else
											{
												tu.ti.clear();
												tu.timeoutTick = 0;
											}
										}
									}else
									{
										pUser->pUserSocket->SendSystemMessage(L"You don't have enough item(s) to pay for the teleport service!");
									}
								}else
								{
									pUser->pUserSocket->SendSystemMessage(L"You cannot use this teleport with your current level!");
								}
							}else
							{
								g_Log.Add(CLog::Error, "[%s] Undefined teleport[%d] - User[%S]", __FUNCTION__, teleportId, pUser->pSD->wName);
							}
						}else
						{
							pUser->pUserSocket->SendSystemMessage(L"You cannot use teleport in PK mode!");
						}
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot use teleport in PvP mode!");
					}
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot use teleport in combat mode!");
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot use teleport while participing in olympiad!");
			}
		}
	}
	unguard;
}