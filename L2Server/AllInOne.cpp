#include "stdafx.h"
#include "AllInOne.h"
#include "DB.h"
#include "SkillAcquireDB.h"

CAllInOne g_AllInOne;

CAllInOne::CAllInOne(): m_Enabled(0), m_UseOutsidePeacezone(false), m_CanUseSubclass(false), m_LeavePeacezone(false), m_TotalSkills(0)
{

}

void CAllInOne::Init()
{
	m_Enabled = 0;
	m_TotalSkills = 0;
	LoadData();
	if(m_Enabled)
	{
		g_Log.Add(CLog::Blue, "[%s] Feature is enabled, loaded [%d] aio and [%d] skills in total.", __FUNCTION__, m_AIO.size(), m_TotalSkills);
	}
}

void CAllInOne::LoadData()
{
	LPBYTE lpFile = 0;
	UINT size = ReadFileBinary(L"..\\Script\\AiOData.txt", lpFile);
	if(lpFile)
	{
		if(size > 2)
		{
			wstringstream file((PWCHAR)&lpFile[2]);
			wstring line;
			UINT currentId = 0;
			AIOData* pData = 0;
			while(getline(file, line))
			{
				if(line.find(L"\\") == 0 || line.find(L"//") == 0)
				{
					continue;
				}
				if(line.find(L"global_setting_begin") == 0)
				{
//setting_begin	enabled=1	can_use_outside_peacezone=1	can_move_outside_peacezone=1	setting_end
					m_Enabled = Parser::ParseOptionInt(line, L"enabled") ? true : false;
					m_UseOutsidePeacezone = Parser::ParseOptionInt(line, L"can_use_outside_peacezone") ? true : false;
					m_LeavePeacezone = Parser::ParseOptionInt(line, L"can_move_outside_peacezone") ? true : false;	
					m_CanUseSubclass = Parser::ParseOptionInt(line, L"can_use_subclass", 1) ? true : false;
				}else if(line.find(L"setting_begin") == 0)
				{
//setting_begin	expire_time=3600 minimum_level=80	price_id=57	price_amount=100000	gift_id=7575	gift_amount=1	setting_end
					if(pData)
					{
						pData->expireTime = Parser::ParseOptionInt(line, L"expire_time", 0);
						if(pData->expireTime == 0)
						{
							pData->type = AIOInfinity;
						}else
						{
							pData->type = AIOTimeLimited;
						}
						pData->requiredLevel = Parser::ParseOptionInt(line, L"minimum_level", 0);
						pData->requiredItem.itemId = Parser::ParseOptionInt(line, L"price_id", 0);
						pData->requiredItem.amount = Parser::ParseOptionInt(line, L"price_amount", 0);
						pData->giftItem.itemId = Parser::ParseOptionInt(line, L"gift_id", 0);
						pData->giftItem.amount = Parser::ParseOptionInt(line, L"gift_amount", 0);
					}
				}else if(line.find(L"skill_begin") == 0)
				{
					if(pData)
					{
//skill_begin	skill_id=1204	skill_level=2	abnormal_time=3600	party_buff=0	skill_end
						AIOSkill skill;
						skill.skillId = Parser::ParseOptionInt(line, L"skill_id");
						skill.level = Parser::ParseOptionInt(line, L"skill_level");
						skill.customTime = Parser::ParseOptionInt(line, L"abnormal_time");
						skill.partyBuff = Parser::ParseOptionInt(line, L"party_buff") ? true : false;
						pData->skills.push_back(skill);
						m_TotalSkills++;
					}
				}else if(line.find(L"aio_") == 0)
				{
					//aio_X_begin / aio_X_begin
					line = Parser::Replace(line, L'_', L' ');
					wstringstream paramStream(line);
					wstring temp;
					if(paramStream >> temp >> currentId >> temp)
					{
						if(currentId > 0)
						{
							if(temp == L"begin")
							{
								pData = new AIOData();
								pData->id = currentId;
							}else if(temp == L"end")
							{
								currentId = 0;
								if(pData)
								{
									m_AIO.push_back(pData);
								}
							}
						}
					}
				}
			}
		}
		delete [] lpFile;
	}
}

bool CAllInOne::CanUse(User *pUser, int skillId)
{
	CTL;
	AIOUser& aioUser = pUser->pEUD->aioUser;
	if(aioUser.aioId > 0)
	{
		if(!pUser->inPeaceZone && !m_UseOutsidePeacezone)
		{
			//check if it's aio skill
			for(UINT n=0;n<aioUser.pData->skills.size();n++)
			{
				if(skillId == aioUser.pData->skills[n].skillId)
				{
					return false;
				}
			}
		}
	}
	return true;
}

INT32 CAllInOne::GetAbnormalTime(User *pUser, INT32 skillId)
{
	CTL;
	INT32 abnormalTime = 0;
	AIOUser& aioUser = pUser->pEUD->aioUser;
	if(aioUser.aioId > 0)
	{
		if(aioUser.pData)
		{
			for(UINT n=0;n<aioUser.pData->skills.size();n++)
			{
				if(skillId == aioUser.pData->skills[n].skillId)
				{
					abnormalTime = aioUser.pData->skills[n].customTime;
					break;
				}
			}
		}
	}
//	g_Log.Add(CLog::Blue, "[%s] abnormalTime[%d] skillId[%d]", __FUNCTION__, abnormalTime, skillId);
	return abnormalTime;
}

AIOData* CAllInOne::Get(UINT aioId)
{
	for(UINT n=0;n<m_AIO.size();n++)
	{
		if(m_AIO[n]->id == aioId)
		{
			return m_AIO[n];
		}
	}
	return 0;
}

void CAllInOne::Acquire(User *pUser, UINT aioId)
{
	CTL;
	if(m_Enabled)
	{
		if(pUser->pSD->subjob_id == 0)
		{
			if(AIOData *pData = Get(aioId))
			{
				if(pData->requiredLevel <= pUser->pSD->level)
				{
					if(pData->requiredItem.itemId > 0 && pData->requiredItem.amount > 0)
					{
						if(!pUser->DeleteItemInInventory(pData->requiredItem.itemId, pData->requiredItem.amount))
						{
							pUser->pUserSocket->SendSystemMessage(L"You don't have enough item(s) to acquire AllInOne status!");
							return;
						}

						//give gift
						if(pData->giftItem.itemId > 0 && pData->giftItem.amount > 0)
						{
							pUser->AddItemToInventory(pData->giftItem.itemId, pData->giftItem.amount);
						}

						AIOUser& aioUser = pUser->pEUD->aioUser;
						aioUser.aioId = aioId;
						aioUser.aioAcquireTime = time(0);
						aioUser.pData = pData;

						g_DB.RequestSaveAIO(pUser->databaseId, aioUser.aioId, static_cast<UINT>(aioUser.aioAcquireTime));

						//acquire skills
						AcquireSkills(pUser, aioId);
						pUser->SocialAction(SAT_CURSED_WEAPON_LEVEL_UP);
						pUser->pUserSocket->SendSystemMessage(L"Congratulations! You have acquired AllInOne status!");
						g_Logger.Add(L"User[%s][%d] acquired aio[%d] status.", pUser->pSD->wName, pUser->databaseId, aioId);
					}
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"Your level is too low to acquire AllInOne status.");
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid aioId[%d] - User[%S]!", __FUNCTION__, aioId, pUser->pSD->wName);
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"Please switch to your Main class in order to continue.");
		}
	}
}

void CAllInOne::AcquireSkills(User *pUser, UINT aioId)
{
	CTL;
	if(pUser->pSD->subjob_id == 0)
	{
		if(AIOData *pData = Get(aioId))
		{
			for(UINT n=0;n<pData->skills.size();n++)
			{
				AIOSkill& skill = pData->skills[n];
				if(!pUser->HaveSkill(skill.skillId, skill.level))
				{
					pUser->AcquireSkill(skill.skillId, skill.level);
					g_DB.SendAcquireSkillToDB(pUser->databaseId, skill.skillId, skill.level, false);
				}
			}
		}
		pUser->ValidateParameters();
	}
}

void CAllInOne::DeleteSkills(User *pUser)
{
	CTL;
	
	pUser->ValidateAcquiredSkills();
}

void CAllInOne::OnDBLoad(User *pUser, UINT aioId, time_t startTime)
{
	CTL;
	if(m_Enabled)
	{
		if(aioId > 0)
		{
			if(AIOData *pData = Get(aioId))
			{
				AIOUser& aioUser = pUser->pEUD->aioUser;
				aioUser.aioId = aioId;
				aioUser.aioAcquireTime = startTime;
				aioUser.pData = pData;
				if(pUser->pSD->subjob_id == 0)
				{
					DeleteSkills(pUser);
					AcquireSkills(pUser, aioId);
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Undefined aio id[%d]- User[%S]!", __FUNCTION__, aioId, pUser->pSD->wName);
			}
		}
	}
}

void CAllInOne::OnTimerExpired(User *pUser)
{
	CTL;

	if(m_Enabled)
	{
		if(pUser->pSD->subjob_id == 0)
		{
			AIOUser& aioUser = pUser->pEUD->aioUser;
			if(aioUser.aioId > 0)
			{
				if(AIOData *pData = Get(aioUser.aioId))
				{
					if(pData->expireTime > 0)
					{
						//check if aio expired
						time_t aioTimeout = aioUser.aioAcquireTime + pData->expireTime;
						if(aioTimeout < time(0))
						{
							//delete aio status
							pUser->DoNothing();
							DeleteSkills(pUser);
							aioUser.aioAcquireTime = 0;
							aioUser.aioId = 0;
							aioUser.pData = 0;
							pUser->DeleteItemInInventory(pData->giftItem.itemId, pData->giftItem.amount);
							g_DB.RequestSaveAIO(pUser->databaseId, 0, 0);
							g_Logger.Add(L"User[%s][%d] - AllInOne status expired.", pUser->pSD->wName, pUser->databaseId);
							pUser->SetUserInfoChanged();
							pUser->SetCharInfoChanged();
							pUser->pUserSocket->SendSystemMessage(L"Your AllInOne status expired.");
						}
					}else
					{
						//infinity aio - do nothing	
					}

					if(!LeavePeacezone())
					{
						//check if user is in peacezone if not port him to his last peacezone pos
						if(!pUser->inPeaceZone)
						{
							if(pUser->pEUD->lastPeaceZonePos.x != 0 && pUser->pEUD->lastPeaceZonePos.y != 0)
							{
								pUser->StopMove();
								pUser->DoNothing();
								pUser->pUserSocket->SendSystemMessage(L"You cannot leave PeaceZone with AllInOne status!");
								pUser->TeleportToLocation(pUser->pEUD->lastPeaceZonePos.x, pUser->pEUD->lastPeaceZonePos.y, pUser->pEUD->lastPeaceZonePos.z);
							}else
							{
								//send home
								pUser->SendHome(true);
							}
						}
					}
				}
			}
		}
	}
}