#include "stdafx.h"
#include "Dungeons.h"
#include "UserAction.h"
#include "Party.h"

CDungeons g_DungeonSystem;

CDungeons::CDungeons() : m_enabled(FALSE)
{
	SetName(L"Dungeon System");
}

CDungeons::~CDungeons()
{
}

void CDungeons::Init()
{
	m_enabled = FALSE;
	LoadData();
	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Loaded [%d] dungeons.", GetName(), m_dungeons.size());
	}
}

void CDungeons::LoadData()
{
	guard;
	LPBYTE lpData = 0;
	DWORD len = Utils::ReadFileBinary("..//Script//DungeonData.txt", lpData);
	if(lpData)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		wstring line;
		while(getline(file, line))
		{
			if(line.size() > 4)
			{
				if( line[0] == L'/' || line[0] == L';' )
					continue;

				if( line.find(L"setting_begin") == 0 )
				{
					m_enabled = ParseOptionInt(line, L"enabled", 0);
					continue;
				}
				if( line.find(L"dungeon_begin") == 0 )
				{
					//dungeon_begin	enabled=1	min_loc=0/0/0	max_loc=0/0/0	kickout_loc=0/0/0	entry_loc=0/0/0	required_item_id=57	required_item_count=50000	party_required=1	min_members_count=3	max_members_count=9	dungeon_end
					DungeonData dd;
					dd.id = ParseOptionInt(line, L"id", 0);
					dd.enabled = ParseOptionInt(line, L"enabled", 0);
					wstring pos = ParseOptionString(line, L"min_loc", L"0/0/0");
					if(pos.size() > 3)
					{
						pos = Utils::ReplaceString(pos, L"/", L" ", true);
						wstringstream sstr;
						sstr << pos;
						sstr >> dd.range[0][0] >> dd.range[1][0] >> dd.range[2][0];
					}
					pos = ParseOptionString(line, L"max_loc", L"0/0/0");
					if(pos.size() > 3)
					{
						pos = Utils::ReplaceString(pos, L"/", L" ", true);
						wstringstream sstr;
						sstr << pos;
						sstr >> dd.range[0][1] >> dd.range[1][1] >> dd.range[2][1];
					}
					pos = ParseOptionString(line, L"entry_loc", L"0/0/0");
					if(pos.size() > 3)
					{
						pos = Utils::ReplaceString(pos, L"/", L" ", true);
						wstringstream sstr;
						sstr << pos;
						sstr >> dd.entryPos.x >> dd.entryPos.y >> dd.entryPos.z;
					}

					pos = ParseOptionString(line, L"kickout_loc", L"0/0/0");
					if(pos.size() > 3)
					{
						pos = Utils::ReplaceString(pos, L"/", L" ", true);
						wstringstream sstr;
						sstr << pos;
						sstr >> dd.kickOutPos.x >> dd.kickOutPos.y >> dd.kickOutPos.z;
					}
					dd.requiredItemId = ParseOptionInt(line, L"required_item_id", 0);
					dd.requiredItemCount = ParseOptionInt(line, L"required_item_count", 0);
					dd.partyRequired = ParseOptionInt(line, L"party_required", 0);
					dd.minPartyMembers = ParseOptionInt(line, L"min_members_count", 0);
					dd.maxPartyMembers = ParseOptionInt(line, L"max_members_count", 0);
					dd.timeLimit = ParseOptionInt(line, L"time_limit", 0);
					dd.canJoinInviteParty = ParseOptionInt(line, L"can_join_invite_party", 0);

					m_dungeons.insert(pair<UINT, DungeonData>(dd.id, dd));
				}
			}
		}
		delete [] lpData;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//DungeonData.txt !", GetName());
	}
	unguard;
}

void CDungeons::KickOutUser(User *pUser, DungeonData& data)
{
	guard;
	//kick out
	if(data.kickOutPos.x != 0 || data.kickOutPos.y != 0)
	{
		pUser->pEUD->dungeonUser.id = 0;
		pUser->pEUD->dungeonUser.enterTime = 0;
		pUser->pEUD->dungeonUser.resetTimer = 0;
		UserAction::OnTeleportToLocation(pUser, data.kickOutPos.x, data.kickOutPos.y, data.kickOutPos.z, pUser->instantZoneId, 0x10000);
		pUser->pEUD->dungeonUser.kickoutTimer = 0;
		pUser->pEUD->dungeonUser.canJoinInviteParty = FALSE;
	}
	unguard;
}

void CDungeons::TimerExpired(User *pUser)
{
	guard;
	if(m_enabled && pUser->ValidUser())
	{
		bool inside = false;
		//check if is inside dungeon
		for(map<UINT, DungeonData>::iterator Iter = m_dungeons.begin(); Iter!= m_dungeons.end();Iter++)
		{
			if(Iter->second.enabled)
			{
				if(Iter->second.range[0][0] <= pUser->pSD->pos.x && Iter->second.range[0][1] >= pUser->pSD->pos.x)
				{
					if(Iter->second.range[1][0] <= pUser->pSD->pos.y && Iter->second.range[1][1] >= pUser->pSD->pos.y)
					{
						if(Iter->second.range[2][0] <= pUser->pSD->pos.z && Iter->second.range[2][1] >= pUser->pSD->pos.z)
						{
							inside = true;
							if(pUser->pEUD->dungeonUser.id == Iter->first)
							{
								if(Iter->second.timeLimit > 0)
								{
									if((pUser->pEUD->dungeonUser.enterTime + Iter->second.timeLimit) < time(NULL))
									{
										pUser->pUserSocket->SendSystemMessage(L"You've reached dungeons time limit, you'll be moved outside dungeons.");
										KickOutUser(pUser, Iter->second);
										break;
									}
								}

								if(Iter->second.partyRequired)
								{
									if(CParty *pParty = pUser->GetParty())
									{
										//check for member count
										if(pParty->memberCount < Iter->second.minPartyMembers)
										{
											if(pUser->pEUD->dungeonUser.kickoutTimer == 0)
											{
												pUser->pEUD->dungeonUser.kickoutTimer = 15;
												pUser->pUserSocket->SendSystemMessage(L"Your party has not enough members, you'll be moved outside dungeons.");
											
											}
											pUser->pEUD->dungeonUser.kickoutTimer--;
											if(pUser->pEUD->dungeonUser.kickoutTimer == 0)
											{
												KickOutUser(pUser, Iter->second);
											}
										}else if(pParty->memberCount >= Iter->second.minPartyMembers && pUser->pEUD->dungeonUser.kickoutTimer != 0)
										{
											pUser->pEUD->dungeonUser.kickoutTimer = 0;
										}
									}else
									{
										KickOutUser(pUser, Iter->second);
									}
								}
							}else
							{
								//enter by foot
								KickOutUser(pUser, Iter->second);
							}
						}
					}
				}
			}
		}

		if(!inside && pUser->pEUD->dungeonUser.id)
		{
			if(pUser->pEUD->dungeonUser.resetTimer == 0)
			{
				pUser->pEUD->dungeonUser.resetTimer = 22;
				pUser->pUserSocket->SendSystemMessage(L"You have left the dungeons area. If you'd like to come back you can do that in 15 seconds.");
			}
			if(pUser->pEUD->dungeonUser.resetTimer > 0)
			{
				pUser->pEUD->dungeonUser.resetTimer--;
				if(pUser->pEUD->dungeonUser.resetTimer == 0)
				{
					pUser->pEUD->dungeonUser.id = 0;
					pUser->pEUD->dungeonUser.enterTime = 0;
					pUser->pEUD->dungeonUser.kickoutTimer = 0;
					pUser->pEUD->dungeonUser.resetTimer = 0;
					pUser->pEUD->dungeonUser.canJoinInviteParty = FALSE;
					pUser->pUserSocket->SendSystemMessage(L"You have left the dungeons permanently.");
				}
			}
		}
	}
	unguard;
}

void CDungeons::RequestEnter(User *pUser, UINT dungeonId)
{
	guard;
	if(m_enabled && pUser->ValidUser())
	{
		map<UINT, DungeonData>::iterator Iter = m_dungeons.find(dungeonId);
		if(Iter != m_dungeons.end())
		{
			if(Iter->second.partyRequired)
			{
				if(CParty *pParty = pUser->GetParty())
				{
					//check if it's party leader
					if(pUser->IsPartyMaster())
					{
						if(pParty->memberCount >= Iter->second.minPartyMembers)
						{
							if(pParty->memberCount <= Iter->second.maxPartyMembers)
							{
								//check if members are alive and not in olympiad and stuff
								vector<User*> vMembers = pParty->GetAllMembers();
								for(vector<User*>::iterator partyIter = vMembers.begin();partyIter!=vMembers.end();partyIter++)
								{
									if(User *pMember = (*partyIter)->CastUser())
									{
										if(!pMember->pSD->alive || pMember->olympiadUser.status)
										{
											pUser->pUserSocket->SendSystemMessage(L"At least one of your party members doesn't meet requirements to enter the dungeons.");
											unguard;
											return;
										}
									}
								}

								//check for required items
								if(Iter->second.requiredItemId > 0 && Iter->second.requiredItemCount > 0)
								{
									if(!pUser->inventory.HaveItemByClassId(Iter->second.requiredItemId, Iter->second.requiredItemCount))
									{
										pUser->pUserSocket->SendSystemMessage(L"You've got not enough required item(s) to enter the dungeons!"); 
										unguard;
										return;
									}

									pUser->DeleteItemInInventory(Iter->second.requiredItemId, Iter->second.requiredItemCount);
								}

								//teleport players inside
								for(vector<User*>::iterator partyIter = vMembers.begin();partyIter!=vMembers.end();partyIter++)
								{
									if(User *pMember = (*partyIter)->CastUser())
									{
										pMember->pEUD->dungeonUser.id = dungeonId;
										pMember->pEUD->dungeonUser.enterTime = time(NULL);
										pMember->pEUD->dungeonUser.canJoinInviteParty = Iter->second.canJoinInviteParty;
										pMember->pUserSocket->SendSystemMessage(L"You are now being moved to the dungeons.");
										UserAction::OnTeleportToLocation(pMember, Iter->second.entryPos.x, Iter->second.entryPos.y, Iter->second.entryPos.z, pMember->instantZoneId, 0x10000);
									}
								}
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"Your party has exceeded dungeons member limit.");
							}
						}else
						{
							pUser->pUserSocket->SendSystemMessage(L"Your party has not enough members to be able to enter the dungeons.");
						}
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"Only party leader can make it happen.");
					}
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You have to be in party to be able to enter this dungeons.");
				}
			}else
			{
				//check for required items
				if(Iter->second.requiredItemId > 0 && Iter->second.requiredItemCount > 0)
				{
					if(!pUser->inventory.HaveItemByClassId(Iter->second.requiredItemId, Iter->second.requiredItemCount))
					{
						pUser->pUserSocket->SendSystemMessage(L"You've got not enough required item(s) to enter the dungeons!"); 
						unguard;
						return;
					}

					pUser->DeleteItemInInventory(Iter->second.requiredItemId, Iter->second.requiredItemCount);
				}

				pUser->pEUD->dungeonUser.id = dungeonId;
				pUser->pEUD->dungeonUser.enterTime = time(NULL);
				pUser->pEUD->dungeonUser.canJoinInviteParty = Iter->second.canJoinInviteParty;
				pUser->pUserSocket->SendSystemMessage(L"You are now being moved to the dungeons.");
				UserAction::OnTeleportToLocation(pUser, Iter->second.entryPos.x, Iter->second.entryPos.y, Iter->second.entryPos.z, pUser->instantZoneId, 0x10000);
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%S] User[%S] tried to enter invalid dungeons[%d]!", GetName(), pUser->pSD->wName, dungeonId);
		}
	}
	unguard;
}