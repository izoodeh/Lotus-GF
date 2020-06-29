#include "StdAfx.h"
#include "User.h"
#include "SkillDB.h"
#include "SkillFx.h"
#include "Party.h"
#include "SkillAcquireDB.h"
#include "DB.h"

bool User::IsNowTrade()
{
	if(this)
	{
		typedef bool (*f)(User*);
		return f(0x8A4E8CL)(this);
	}
	return false;
}

void User::TradeCancel()
{
	if(this)
	{
		typedef void (*f)(User*);
		f(0x8A4C34L)(this);
	}
}

SlotType User::UnequipItem(CItem *pItem)
{
	typedef SlotType (*f)(User*, CItem*);
	return f(0x8E85C4L)(this, pItem);
}

bool User::ValidUser()
{
	return Valid(VT_USER);
}

void User::ActionFailed()
{
	typedef void(*f)(User*);
	return f(0x8F2ABCL)(this);
}

bool User::BasicAction(int id, bool ctrl, bool shift, UINT stopType)
{
	typedef bool (*f)(User*, int, bool, bool, UINT);
	return f(0x8F29A4L)(this, id, ctrl, shift, stopType);
}

bool User::CanUseItem(CItem *pItem)
{
	typedef bool (*f)(User*, CItem*);
	return f(0x8A8718L)(this, pItem);
}

bool User::DeleteItemInInventory(int classId, INT64 count, AtomicJob *pJob)
{
	typedef bool(*f)(User*, int, INT64, AtomicJob*);
	return f(0x8D9A20L)(this, classId, count, pJob);
}

void User::DeleteItemInInventory(int classId)
{
	typedef void(*f)(User*, int);
	f(0x8E05BCL)(this, classId);
}

void User::DespawnPet(bool param, bool param2, UINT despawnJob)
{
	typedef void (*f)(User*, bool, bool, UINT);
	f(0x8FCF50L)(this, param, param2, despawnJob);
}

void User::EnchantCancel()
{
	if(this)
	{
		SetPendingUseETCItem(0, PUI_SCROLL);
		SetPendingUseETCItem(0, PUI_ENCHANT_BONUS);
		isEnchantingItem = false;
		enchantRateBonus = 0.0;
		pUserSocket->Send("cddQ", 0x87, 2, 0, 0);
	}
}

void User::EndBuyProduct()
{
	typedef void(*f)(User*);
	f(0x477514L)(this);
}

void User::SendAbnormalStatusInfo()
{
	typedef void(*f)(User*);
	f(0x8AF7E4L)(this);
}

void User::SendDeleteSkillToDB(int skillId)
{
	typedef void(*f)(User*, int);
	f(0x8F274CL)(this, skillId);
}

void User::SendCharInfo(CUserSocket *pSocket, bool param)
{
	typedef void(*f)(User*, CUserSocket*, bool);
	f(0x90C3E0L)(this, pSocket, param);
}

void User::SetKarma(int value)
{
	typedef void (*f)(User*, int);
	f(0x88DAF0L)(this, value);
}

void User::SendUserInfo(CUserSocket* pGMSocket)
{
	typedef void(*f)(User*, CUserSocket*);
	f(0x909D84L)(this, pGMSocket);
}

void User::SendQuestList(CUserSocket *pSocket)
{
	typedef void(*f)(User*, CUserSocket*);
	f(0x8A7B80L)(this, pSocket);
}

void User::SetPendingUseETCItem(CItem *pItem, PendingUseItemType type)
{
	typedef void (*f)(User*, CItem*, PendingUseItemType);
	f(0x8D8A2CL)(this, pItem, type);
}

void User::SetSpelledInfoChagned()
{
	typedef void(*f)(CCreature*);
	f(0x8D3DB0L)(this);
}

CParty* User::GetParty()
{
	if(this)
	{
		typedef CParty* (*f)(User*);
		return f(0x8A1C40L)(this);
	}
	return 0;
}

CItem* User::GetPendingUseETCItem(PendingUseItemType type)
{
	typedef CItem* (*f)(User*, PendingUseItemType);
	return f(0x8D8B34L)(this, type);
}

CAlliance* User::GetAlliance()
{
	typedef CAlliance*(*f)(User*);
	return f(0x8EE2A4L)(this);
}

CPledge* User::GetPledge()
{
	typedef CPledge*(*f)(User*);
	return f(0x8EE1DCL)(this);
}

CSummon* User::GetSummonOrPet()
{
	typedef CSummon*(*f)(User*);
	return f(0x8FC6CCL)(this);
}

void User::PlaySound(const WCHAR* wName)
{
	typedef void (*f)(User*, const WCHAR*, int, int, int, int, int);
	f(0x88E538L)(this, wName, 1, serverId, (int)pSD->pos.x, (int)pSD->pos.y, (int)pSD->pos.z);
}

void User::ProcessMyParty(bool param)
{
	typedef void(*f)(User*, bool);
	f(0x8A3C5CL)(this, param);
}

bool User::RideForEvent(INT32 npcClassId, INT32 timeInSec, INT32 param)
{
	typedef bool (*f)(User*, INT32, INT32, INT32);
	return f(0x8FDF58L)(this, npcClassId, timeInSec, param);
}

bool User::RideWyvern(INT32 npcClassId)
{
	typedef bool (*f)(User*, INT32);
	return f(0x8FF4DCL)(this, npcClassId);
}

CTrade* User::GetTrade()
{
	typedef CTrade*(*f)(User*);
	return f(0x8A0DE4L)(this);
}

User* User::GetUser(LPUINT lpTargetId)
{
	typedef User* (*f)(LPUINT);
	return f(0x86E518L)(lpTargetId);
}

bool User::IsPartyMaster()
{
	typedef bool(*f)(User*);
	return f(0x8A1D08L)(this);
}

void User::SetStopSay(UINT timeInMs, bool saveInDB)
{
	typedef void(*f)(User*, UINT, bool);
	f(0x89A5D0L)(this, timeInMs, saveInDB);
}

bool User::TeleportToLocation(int x, int y, int z, UINT paramEx)
{
	typedef bool (*f)(User*, int, int, int, UINT, INT);
	return f(0x8A3DF8L)(this, x, y, z, instantZoneId, paramEx);
}

void User::ShowHTML(const WCHAR* fileName, const WCHAR* wContent, int objectType)
{
	typedef void (*f)(User*, const WCHAR*, const WCHAR*, int);
	f(0x8D6594L)(this, fileName, wContent, objectType);
}

void User::ShowQuestMark(int id)
{
	if(this)
	{
		pUserSocket->Send("chd", 0xFE, 33, id);
	}
}

void User::Kick()
{
	if(this)
	{
		if(pUserSocket)
		{
			StopMove();
			pUserSocket->ForceClose();
		}
	}
}

void User::StoreAbnormal(AbnormalData &ad, time_t expireTime)
{
	guard;
	StoredAbnormalData sad(ad, expireTime);
	pEUD->lStoredAbnormal.push_back(sad);
	unguard;
}

bool User::RestoreAbnormal(StoredAbnormalData &sad)
{
	guard;

	if(sad.pSI)
	{
		//check buff count and if buff already exists
		UINT count = 0;
	
		abnormalDataLock.Enter(__FILEW__, __LINE__);

		for(AbnormalDataVector::iterator Iter = vAbnormalData.begin(); Iter!= vAbnormalData.end();Iter++)
		{
			if(Iter->pSI)
			{
				if(Iter->pSI->slotType == sad.pSI->slotType)
				{

					abnormalDataLock.Leave();
					unguard;
					return false;
				}
				if(sad.pSI->isMagic == 3 && Iter->pSI->isMagic == 3)	//dance&song
				{
						count++;
						continue;
				}
				if(sad.pSI->isMagic != 3 && Iter->pSI->isMagic != 3)
				{
					if(Iter->pSI->debuffType == sad.pSI->debuffType)
						count++;
				}
			}
		}

		abnormalDataLock.Leave();

		if(sad.pSI->debuffType == DT_DEBUFF)
		{
			if(count < 10)
			{
				sad.pSI->ActivateSkill(this, this);
				UINT newTime = (sad.castTime - _time32(NULL) + sad.pSI->abnormalTime);
				SetNewAbnormalTime(sad.pSI->skillId, sad.pSI->level, newTime, true);
				unguard;
				return true;
			}
		}else
		{
			if(sad.pSI->isMagic == 3 && count < 12)
			{
				sad.pSI->ActivateSkill(this, this);
				UINT newTime = (sad.castTime - _time32(NULL) + sad.pSI->abnormalTime);
				SetNewAbnormalTime(sad.pSI->skillId, sad.pSI->level, newTime, true);
				unguard;
				return true;
			}else if(sad.pSI->isMagic != 3 && count < (20+enlargedBuffSlot))
			{
				sad.pSI->ActivateSkill(this, this);
				UINT newTime = (sad.castTime - _time32(NULL) + sad.pSI->abnormalTime);
				SetNewAbnormalTime(sad.pSI->skillId, sad.pSI->level, newTime, true);
				unguard;
				return true;
			}
		}
	}
	unguard;
	return false;
}

void User::StoredAbnormalTimerExpired()
{
	guard;

	if(pSD->alive && ( (pEUD->deathTick + 2000) < GetTickCount() ) )
	{
		bool restored = false;

		vector<StoredAbnormalData> vToRestore;

		abnormalDataLock.Enter(__FILEW__, __LINE__);
		if(pEUD->lStoredAbnormal.size() > 0)
		{
			time_t currentTime = time(NULL);
			for(StoredAbnormalDataList::iterator Iter = pEUD->lStoredAbnormal.begin();Iter!=pEUD->lStoredAbnormal.end();)
			{
				if(Iter->expireTime < currentTime)
				{
					vToRestore.push_back((*Iter));
					pEUD->lStoredAbnormal.erase(Iter++);
					continue;
				}
				++Iter;
			}
		}

		abnormalDataLock.Leave();


		if(vToRestore.size() > 0)
		{
			for(vector<StoredAbnormalData>::iterator Iter = vToRestore.begin(); Iter!=vToRestore.end();Iter++)
			{
				RestoreAbnormal((*Iter));
			}

			abnormalDataLock.Enter(__FILEW__, __LINE__);
			CheckBuffLock();
			abnormalDataLock.Leave();
			SendAbnormalStatusInfo();
			SetSpelledInfoChagned();
			CheckAbnormalVisualEffect();
			ValidateParameters();
		}
	}
	unguard;
}

void User::ResetStoredAbnormalTimes()
{
	abnormalDataLock.Enter(__FILEW__, __LINE__);
	if(pEUD->lStoredAbnormal.size() > 0)
	{
		for(StoredAbnormalDataList::iterator Iter = pEUD->lStoredAbnormal.begin();Iter!=pEUD->lStoredAbnormal.end();Iter++)
		{
			Iter->expireTime = 0;
		}
	}
	abnormalDataLock.Leave();
}

void User::DeleteStoredAbnormals()
{
	abnormalDataLock.Enter(__FILEW__, __LINE__);
	if(pEUD->lStoredAbnormal.size() > 0)
	{
		pEUD->lStoredAbnormal.clear();
	}
	abnormalDataLock.Leave();
}

bool User::StoreRandomBuffs(CCreature *pOther, CSkillInfo *pSI, UINT chance, UINT count)
{
	if(ValidUser())
	{
		if(!blockBuff && count > 0)
		{
			if(chance > 100)
				chance = 100;
			if(chance < 25)
				chance = 25;

			bool requestUpdate = false;
			UINT deletedBuffs = 0;
			vector<AbnormalStatusSlotType> vToDelete;
			abnormalDataLock.Enter(__FILEW__, __LINE__);
			
			for(AbnormalDataVector::iterator Iter = vAbnormalData.begin();Iter!=vAbnormalData.end();Iter++)
			{

				if(Iter->pSI)
				{
					if(Iter->pSI->magicLevel < 0 || Iter->pSI->debuffType != DT_BUFF)
					{
						continue;
					}

					if(Iter->pSI->skillId == 1323 )
					{
						continue;
					}
					if(Iter->pSI->skillId == 1410 )
					{
						continue;
					}
					if(Iter->pSI->skillId == 438 )
					{
						continue;
					}

					double dispelChance = 100.0;
					if(chance < 100)
					{
						dispelChance = CSkillFx::GetDispelChance((double)chance, Iter->pSI, pSI, &_unknB78);
						if(dispelChance > 75.0)
							dispelChance = 75.0;
						if(dispelChance < 25.0)
							dispelChance = 25.0;
					}
					double rnd = RandDoubleEx(100.0);

					if(rnd < dispelChance)
					{
						vToDelete.push_back(Iter->pSI->slotType);
						deletedBuffs++;
						if(deletedBuffs >= count)
							break;
					}

				}
			}

			for(vector<AbnormalStatusSlotType>::iterator Iter = vToDelete.begin(); Iter!= vToDelete.end();Iter++)
			{
				UINT index = 0;
				for(AbnormalDataVector::iterator aIter = vAbnormalData.begin();aIter!=vAbnormalData.end();aIter++)
				{
					if(aIter->pSI)
					{
						if(aIter->pSI->slotType == (*Iter))
						{
							time_t expireTime = time(NULL);
							if(pSI->abnormalTime > 0)
								expireTime += pSI->abnormalTime;
							else
								expireTime += 15;

							StoreAbnormal((*aIter), expireTime);
							DeleteAbnormalStatus(index, pOther, pSI);
							break;
						}
					}
					index++;
				}
			}

			abnormalDataLock.Leave();
			if(vToDelete.size() > 0)
			{
				SetAbnormalChanged(pSI->skillId, pSI->level);
				return true;
			}
		}
	}
	return false;
}

bool User::CheckMyPledgeDeclareWar(User *pOther)
{
	typedef bool (*f)(User*, User*);
	return f(0x892B54L)(this, pOther);
}

UINT User::GetPartyMemberCount(FVector& pos, double range, bool diffIp, bool diffMac)
{
	UINT count = 0;
	if(ValidUser())
	{
		if(CParty *pParty = GetParty())
		{
			UINT uniqueIp[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			INT64 uniqueMac[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //max 9 party members
			UserVector vMembers = pParty->GetAllMembers();
			for(UserVector::iterator Iter = vMembers.begin();Iter!=vMembers.end();Iter++)
			{
				UINT unique = 0, maxUnique = 0;
				
				User *pMember = (*Iter);
				if(pMember->ValidUser())
				{
					if(diffIp)
					{
						maxUnique++;
						for(UINT n=0;n<count;n++)
						{
							if(uniqueIp[n] == pMember->pUserSocket->address.S_un.S_addr)
							{
								unique++;
								break;
							}
						}
					}

					if(diffMac)
					{
						maxUnique++;
						if(pMember->pUserSocket->macAddress > 0)
						{
							for(UINT n=0;n<count;n++)
							{
								if(uniqueMac[n] == pMember->pUserSocket->macAddress)
								{
									unique++;
									break;
								}
							}
						}
					}

					if(( maxUnique == 0 || (unique < maxUnique) ) && pMember->IsInRange(pos, range))
					{
						uniqueIp[count] = pMember->pUserSocket->address.S_un.S_addr;
						uniqueMac[count] = pMember->pUserSocket->macAddress;
						count++;
					}
				}
			}
		}else
		{
			//no party
			if(IsInRange(pos, range))
				count++;
		}
	}
	return count;
}

bool User::UseItem(CItem *pItem, UINT param1, bool param2)
{
	typedef bool (*f)(User*, CItem*, UINT, bool);
	return f(0x8DD00CL)(this, pItem, param1, param2);
}

void User::QuitPrivateStore()
{
	typedef void(*f)(User*);
	f(0x89A830L)(this);
}

void User::SocialAction(int actionType, int param2, bool param3)
{
	typedef void (*f)(User*, int, int, bool);
	f(0x89358CL)(this, actionType, param2, param3);
}

void User::SetLevel(int level)
{
	guard;
	if(level > 0 && level < 86)
	{
		UINT64 addr = (*(PUINT64)this);
		addr+= 0x568;
		addr = (*(PUINT64)addr);
		typedef void(*f)(User*, UINT64);
		PUINT64 lpExpTable = (PUINT64)(0xA7C388);
		UINT64 exp = lpExpTable[level];
		lpYieldLock->Enter(__FILEW__, __LINE__);
		f((PVOID)addr)(this, exp);
		lpYieldLock->Leave();
		ValidateParameters();
	}
	unguard;
}

void User::SetGuilty(bool value)
{
	typedef void(*f)(User*, bool);
	f(0x88C0A8L)(this, value);
}

void User::SendHome(bool withKarma)
{
	FVector newPos;
	FVector currentPos = pSD->pos;
	typedef void (*f)(UINT64, FVector*, UINT /*raceType*/, FVector* /*currentPos*/, bool /*hasKarma*/, int /*pkCount*/);
	f(0x80D578L)(0x10BA3300L, &newPos, pSD->race, &currentPos, withKarma ? false : (bool)pSD->karma, withKarma ? 0 : pSD->pk_count);

	TeleportToLocation((int)newPos.x, (int)newPos.y, (int)newPos.z, 0x10000);
}

void User::SetPvPFlag()
{
	typedef bool(*f)(LPUINT);
	f(0x68E64CL)(&pSD->pvpFlagTimestamp);	//CIOObject::SetTimeStamp
	SetGuilty(true);
}

void User::ValidateAcquiredSkills()
{
	CTL;
	vector<pair<INT32, INT32>> skillToDelevel;
	vector<INT32> skillToDelete;

	m_AcquiredSkillLock.Enter(__FILEW__, __LINE__);
	for(map<INT32, UINT64>::iterator it = m_AcquiredSkills.begin(); it != m_AcquiredSkills.end(); it++)
	{
		AcquiredSkillParam as;
		memcpy(&as, &it->second, 8);
		SkillPchId skill(it->first, as.level);
	//	g_Log.Add(CLog::Blue, "[%s] skill[%d] level[%d] temp[%d]", __FUNCTION__, it->first, as.level, as.temporary);
		if(g_SkillAcquireDbEx.IsAcquirableSkill(skill.skillId, skill.level))
		{
			INT32 maxAvailable = g_SkillAcquireDbEx.GetMaxAvailableLevel(pSD->occupation, skill.skillId, pSD->level + 9);
			if(skill.level > 100)
			{
				INT32 maxAcquirable = g_SkillAcquireDbEx.GetMaxAcquirableSkillLevel(skill.skillId);
				if(!g_SkillAcquireDbEx.CanAcquire(pSD->occupation, skill.skillId, maxAcquirable, 85))
				{
					if(maxAcquirable == 0)
					{
						skillToDelete.push_back(skill.skillId);
					}else
					{
						//can't enchant this skill - delevel it
						skillToDelevel.push_back(pair<INT32, INT32>(skill.skillId, maxAvailable));
					}
				}
			}else
			{
				if(maxAvailable > 0)
				{
					if(maxAvailable < skill.level)
					{
						skillToDelevel.push_back(pair<INT32, INT32>(skill.skillId, maxAvailable));
					}
				}else
				{
					skillToDelete.push_back(skill.skillId);
				}
			}
		}else
		{
			skillToDelete.push_back(skill.skillId);
		}
	}
	m_AcquiredSkillLock.Leave();

	//loop skills to delevel
	for(UINT n=0;n<skillToDelevel.size();n++)
	{
		AcquireSkill(skillToDelevel[n].first, skillToDelevel[n].second, TRUE, TRUE);
		g_DB.SendAcquireSkillToDB(databaseId, skillToDelevel[n].first, skillToDelevel[n].second, true);
	}


	//loop skills to delete
	for(UINT n=0;n<skillToDelete.size();n++)
	{
		DeleteAcquireSkill(skillToDelete[n]);
		g_DB.SendDeleteSkill(databaseId, skillToDelete[n]);
	}

	ValidateParameters();
}

void User::SetUserInfoChanged()
{
	InterlockedExchange(&pEUD->userInfoChanged, TRUE);
}

void User::SetCharInfoChanged()
{
	InterlockedExchange(&pEUD->charInfoChanged, TRUE);
}