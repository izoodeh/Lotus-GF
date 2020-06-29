#include "StdAfx.h"
#include "Creature.h"
#include "CreatureAction.h"
#include "SkillDB.h"
#include "HookManager.h"

extern BOOL g_CustomTraitSystem;

void CCreature::Init()
{
	if(g_CustomTraitSystem)
	{
		WriteCall(0x71951B, CCreature::ConstructorByOther);	//CNPC::CNPC(const CNPC& other)
		WriteCall(0x724BCC, CCreature::Constructor);	//CNPC::CNPC
		WriteCall(0x8D4389, CCreature::Constructor);	//User::User
		WriteCall(0x5285E1, CCreature::FreeSharedData);
	}
}

void CCreature::OnExtCreate()
{
	guard;

	this->pECD = new ExtCreatureData;
	if(this->pECD)
	{
		for(UINT n=0;n<TRAIT_COUNT;n++)
		{
			this->pECD->attackTrait[n] = false;
			this->pECD->defenceTrait[n] = false;
			this->pECD->perfectDefenceTrait[n] = false;
			this->pECD->attackTraitValue[n] = 1.0;
			this->pECD->defenceTraitValue[n] = 1.0;
		}

		this->lpAttackTrait = this->pECD->attackTrait;
		this->lpAttackTraitValue = this->pECD->attackTraitValue;
		this->lpDefenceTrait = this->pECD->defenceTrait;
		this->lpDefenceTraitValue = this->pECD->defenceTraitValue;
		this->lpPerfectDefenceTrait = this->pECD->perfectDefenceTrait;
		this->pECD->deathTime = 0;
		this->pECD->leftWorld = 0;
	}else
	{
		g_Log.Add(CLog::Error, "[%s] null ExtCreatureData - not enough memory?", __FUNCTION__);
		g_Log.CallStack(CLog::Error);
	}

	unguard;
}

int CCreature::FreeSharedData(LPVOID lpInstance, CCreature *pCreature)
{
	pCreature->OnExtDelete();

	typedef int (*f)(LPVOID, CCreature*);
	return f(0x524974L)(lpInstance, pCreature);
}

CCreatureSP* CCreature::GetCreature(CCreatureSP& creatureSp, UINT serverIndex)
{
	typedef CCreatureSP* (*f)(UINT64, CCreatureSP&, UINT);
	return f(0x448F14L)(0x10DE4580L, creatureSp, serverIndex);
}

void CCreature::OnExtDelete()
{
	guard;

	if(this->pECD)
	{
		delete this->pECD;
		this->pECD = 0;
	}

	unguard;
}

void CCreature::Constructor(CCreature *pCreature)
{
	guard;

	pCreature->OnExtCreate();

	typedef void(*f)(CCreature*);
	f(0x528E80L)(pCreature);

	unguard;
}

void CCreature::ConstructorByOther(CCreature *pCreature, CCreature *pOther)
{
	guard;

	pCreature->OnExtCreate();

	typedef void (*f)(CCreature*, CCreature*);
	f(0x5276E8L)(pCreature, pOther);
	unguard;
}

bool CCreature::ValidCreature()
{
	if(this)
	{
		try
		{
			DWORD dwVTable = (*(LPDWORD)this);
			switch(dwVTable)
			{
			case VT_CCREATURE :
			case VT_CNPC :
			case VT_CCOLLECTION :
			case VT_CDOPPELGANGER :
			case VT_CSUMMON :
			case VT_CPET:
			case VT_CTELEPORTER :
			case VT_CPCTRAP :
			case VT_CWORLD_TRAP :
			case VT_CBOSS :
			case VT_CZZOLDAGU :
			case VT_CMERCHANT :
			case VT_USER:
				{
					if(pSD)
					{
						return true;
					}
					break;
				}
			}
		}catch(...)
		{

		}
	}
	return false;
}

void CCreature::ValidateParameters()
{
	typedef void(*f)(CCreature*);
	f(0x55CB14L)(this);
}

SlotType CCreature::IsEquipped(int itemIndex, SlotType slot)
{
	typedef SlotType (*f)(CCreature*, int, SlotType);
	return f(0x52FD68L)(this, itemIndex, slot);
}

CCreature *CCreature::GetCreatureByObjectId(LPUINT lpObjectId)
{
	typedef CCreature* (*f)(LPUINT);
	return f(0x86E4E4L)(lpObjectId);
}

CSharedItemData* CCreature::GetEquipedWeapon()
{
	typedef CSharedItemData* (*f)(CCreature*);
	return f(0x536E38L)(this);
}


int CCreature::GetAbnormalLevel(AbnormalStatusSlotType type, bool useLock)
{
	int have = false;
	if(useLock)
		lpYieldLock->Enter(__FILEW__, __LINE__);

	typedef int (*f)(CSharedCreatureData*, AbnormalStatusSlotType);
	have = f(0x82CB70L)(pSD, type);

	if(useLock)
		lpYieldLock->Leave();

	return have;
}

int CCreature::GetAbnormalLevelBySkillId(int skillId, bool useLock)
{
	int level = -1;

	if(useLock)
		abnormalDataLock.Enter(__FILEW__, __LINE__);

	for(AbnormalDataVector::iterator Iter = vAbnormalData.begin();Iter!=vAbnormalData.end();Iter++)
	{
		if(Iter->pSI)
		{
			if(Iter->pSI->skillId == skillId)
			{
				level = Iter->pSI->level;
				break;
			}
		}
	}

	if(useLock)
		abnormalDataLock.Leave();

	return level;
}

UINT CCreature::GetItemUsedTime(int itemId)
{
	typedef UINT (*f)(CCreature*, int);
	return f(0x549FA0L)(this, itemId);
}

double CCreature::GetWeaponTraitBonus(UINT attackType)
{
	typedef double(*f)(CCreature*, UINT);
	return f(0x53E870L)(this, attackType);
}

void CCreature::DoNothing()
{
	typedef void (*f)(CCreature*);
	f(0x538FDCL)(this);
}

bool CCreature::Is(INT64 vTableOffset)
{
	if(this)
	{
		try
		{
			INT64 addr = (*(PINT64)this); //vTable
			addr += vTableOffset;
			addr = (*(PINT)addr);

			typedef bool (*f)(CCreature*);
			return f(addr)(this);
		}catch(...)
		{
		}
	}
	return false;
}

bool CCreature::IsInRange(FVector& position, double range)
{
	if(this)
	{
		try
		{
			double x = pSD->pos.x - position.x;
			double y = pSD->pos.y - position.y;
			double z = pSD->pos.z - position.z;
			x*=x;
			y*=y;
			z*=z;
			if( sqrt( (x+y+z) ) < range )
			{
				return true;
			}
		}catch(...)
		{
		}
	}
	return false;
}

INT64 CCreature::Is64(INT64 vTableOffset)
{
	if(this)
	{
		try
		{
			INT64 addr = (*(PINT64)this); //vTable
			addr += vTableOffset;
			addr = (*(PINT)addr);

			typedef INT64 (*f)(CCreature*);
			return f(addr)(this);
		}catch(...)
		{
		}
	}
	return 0;
}

void CCreature::LeaveWorld()
{
	if(this)
	{
		try
		{
			UINT64 addr = (*(PUINT64)this); //vTable
			addr += 0x850;
			addr = (*(PUINT64)addr);

			typedef void (*f)(CCreature*);
			f((PVOID)addr)(this);
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}
}

void CCreature::RemoveSkillUsableTimeAll()
{
	typedef void(*f)(CCreature*);
	if(User *pUser = CastUser())
	{
		pUser->pUserSocket->Send("cd", 0xC7, 0);
	}
	return f(0x53D60CL)(this);
}

bool CCreature::ResurrectedBy(CCreature *pOther, int expPer)
{
	if(pOther == 0)
		pOther = this;

	typedef bool (*f)(CCreature*, CCreature*, int);
	return f(0x55CC2CL)(this, pOther, expPer);
}

bool CCreature::CanBeBuffed(DebuffType debuffType, AbnormalStatusSlotType slotType)
{
	if(this)
	{
		typedef bool (*f)(CCreature*, DebuffType, AbnormalStatusSlotType);
		return f(0x54A7C4L)(this, debuffType, slotType);
	}
	return false;
}

void CCreature::ChangeTarget(CObject *pTarget, UINT reason)
{
	if(this)
	{
		UINT64 addr = (*(PUINT64)this);
		addr += 0x920;
		addr = (*(PUINT64)addr);
		typedef void (*f)(CCreature*, CObject*, UINT);
		f((LPVOID)addr)(this, pTarget, reason);

	}
}

void CCreature::CheckAbnormalVisualEffect()
{
	if(this)
	{
		CreatureAction::CheckAbnormalVisualEffect(this);
	}
}

void CCreature::CheckBuffLock()
{
	typedef void(*f)(CCreature*);
	f(0x561CB4L)(this);
}

bool CCreature::DeleteAbnormalStatus(const int index, CCreature* pCreature, CSkillInfo *pSI, bool checkBuffLock, UINT skillPumpEndType)
{
	typedef bool (*f)(CCreature*, const int, CCreature*, CSkillInfo*, bool, UINT);
	return f(0x5646D4L)(this, index, pCreature, pSI, checkBuffLock, skillPumpEndType);
}

bool CCreature::DispelByAbnormal(UINT abnormalType, bool useLock)
{
	bool update = false;
	if(useLock)
		abnormalDataLock.Enter(__FILEW__, __LINE__);

	UINT index = 0;
	for(AbnormalDataVector::iterator Iter = vAbnormalData.begin(); Iter!= vAbnormalData.end();Iter++)
	{
		CSkillInfo *pSI = Iter->pSI;
		if(pSI)
		{
			if(pSI->slotType == abnormalType)
			{
				update = true;
				DeleteAbnormalStatus(index, 0, pSI);
				break;
			}
		}
		index++;
	}

	if(useLock)
		abnormalDataLock.Leave();

	if(update && useLock)
	{
		SetAbnormalChanged(0, 0);
		return true;
	}
	return false;
}

bool CCreature::DeleteAcquireSkill(int skillId)
{
	typedef bool (*f)(CCreature*, int);
	return f(0x55E87CL)(this, skillId);
}

bool CCreature::Die(CCreature *pEnemy)
{
	if(this)
	{
		UINT64 addr = (*(PUINT64)this);	//vt
		addr += 0x890;
		addr = (*(PUINT64)addr);
		typedef bool (*f)(CCreature*, CCreature*);
		return f(reinterpret_cast<LPVOID>(addr))(this, pEnemy);
	}
	return false;
}

void CCreature::DispelAllByGM(UINT pumpEndType)
{
	typedef void(*f)(CCreature*, UINT);
	f(0x567438L)(this, pumpEndType);
}

bool CCreature::HaveSkill(int skillId, int level)
{
	typedef bool(*f)(CCreature*, int, int);
	return f(0x550410L)(this, skillId, level);
}

void CCreature::Healthy()
{
	if(ValidCreature())
	{
		pSD->hp = pSD->max_hp;
		pSD->cp = pSD->max_cp;
		pSD->mp = pSD->max_mp;
		UpdateStat(STAT_HP);
		UpdateStat(STAT_MP);
		if(User *pUser = CastUser())
		{
			pUser->UpdateStat(STAT_HPnCP);
		}
	}
}

int CCreature::GetAcquiredSkillLevel(int skillId)
{
	typedef int(*f)(CCreature*, int);
	return f(0x5502D8L)(this, skillId);
}

bool CCreature::InfectedSkillBy(CCreature *pCaster, CSkillInfo *pSI, CSkillInfo *pSI2, double timeVal, int param)
{
	typedef bool (*f)(CCreature*, CCreature*, CSkillInfo*, CSkillInfo*, double, int);

	return f(0x56B240L)(this, pCaster, pSI, pSI2, timeVal, param);
}

void CCreature::Action(CObject *pTarget, bool param, bool paramEx)
{
	if(this)
	{
		try
		{
			UINT64 addr = (*reinterpret_cast<PUINT64>(this));
			addr += 0x900;
			addr = (*reinterpret_cast<PUINT64>(addr));
			typedef void(*f)(CCreature*, CObject*, bool, bool);
			f(reinterpret_cast<LPVOID>(addr))(this, pTarget, param, paramEx);
		}catch(...)
		{
			g_Log.Add(CLog::Error, "[%s] Exception Occoured!", __FUNCTION__);
			g_Log.CallStack(CLog::Error);
		}
	}
}

bool CCreature::AcquireSkill(int skillId, int level, BOOL replace, BOOL param)
{
	typedef bool (*f)(CCreature*, int, int, BOOL, BOOL);
	return f(0x55D3B8L)(this, skillId, level, replace, param);
}

void CCreature::AddAbnormalStatus(CSkillInfo *pSI, CCreature *pCreature, bool sendInfo, bool serializeMessage)
{
	typedef void (*f)(CCreature*, CSkillInfo*, CCreature*, bool, bool);
	f(0x534CECL)(this, pSI, pCreature, sendInfo, serializeMessage);
}

void CCreature::AddItemToInventory(int itemId, INT64 count, bool param1, int param2, int param3, int param4, ItemOptionKey *pItemOptionKey, int param5, BaseAttribute* pBaseAttribute)
{
	typedef void (*f)(CCreature*, int, INT64, bool, int , int , int, ItemOptionKey*, int, BaseAttribute*);
	if(!pItemOptionKey && !pBaseAttribute)
	{
		ItemOptionKey itemOptionKey;
		BaseAttribute baseAttribute;
		f(0x533E44L)(this, itemId, count, param1, param2, param3, param4, &itemOptionKey, param5, &baseAttribute);
	}else if(!pItemOptionKey)
	{
		ItemOptionKey itemOptionKey;
		f(0x533E44L)(this, itemId, count, param1, param2, param3, param4, &itemOptionKey, param5, pBaseAttribute);
	}else if(!pBaseAttribute)
	{
		BaseAttribute baseAttribute;
		f(0x533E44L)(this, itemId, count, param1, param2, param3, param4, pItemOptionKey, param5, &baseAttribute);
	}else
	{
		f(0x533E44L)(this, itemId, count, param1, param2, param3, param4, pItemOptionKey, param5, pBaseAttribute);
	}
}

void CCreature::SetAbnormalChanged(int skillId, int level)
{
	typedef void (*f)(CCreature*, int, int);
	f(0x54A4B0L)(this, skillId, level);
}

INT64 CCreature::SetExp(INT64 newExp)
{
	typedef INT64(*f)(CCreature*, INT64);
	return f(0x52C270L)(this, newExp);
}

void CCreature::SetSelfBuff(int skillId, int level, bool selfBuff)
{
	abnormalDataLock.Enter(__FILEW__, __LINE__);
	for(AbnormalDataVector::iterator Iter = vAbnormalData.begin();Iter!=vAbnormalData.end();Iter++)
	{
		if(Iter->pSI)
		{
			if(Iter->pSI->skillId == skillId && Iter->pSI->level == level)
			{				
				Iter->selfBuff = selfBuff;
				break;
			}
		}
	}
	abnormalDataLock.Leave();
}

void CCreature::SetNewAbnormalTime(int skillId, int level, UINT abnormalTime, bool restored, bool lock)
{
	guard;
	if(lock)
	{
		abnormalDataLock.Enter(__FILEW__, __LINE__);
	}
	for(AbnormalDataVector::iterator Iter = vAbnormalData.begin();Iter!=vAbnormalData.end();Iter++)
	{
		if(Iter->pSI)
		{
			if(Iter->pSI->skillId == skillId && Iter->pSI->level == level)
			{
				if(abnormalTime > 0)
				{
					INT32 diff = abnormalTime - Iter->pSI->abnormalTime;
					Iter->castTime += diff;
				}
				
				if(restored)
					Iter->selfBuff = 0;

				break;
			}
		}
	}
	if(lock)
	{
		abnormalDataLock.Leave();
	}
	if(User *pUser = this->CastUser())
	{
		pUser->SendAbnormalStatusInfo();
	}
	unguard;
}

void CCreature::SetItemUsableTime(int itemId, ULONG* timestamp, ULONG reuse)
{
	typedef void(*f)(CCreature*, int, ULONG*, ULONG);
	f(0x549D60L)(this, itemId, timestamp, reuse);
}

void CCreature::StopMove()
{
	typedef void(*f)(CCreature*);
	f(0x537760L)(this);
}

void CCreature::UpdateStat(UPDATE_STAT type)
{
	typedef void(*f)(CCreature*, UPDATE_STAT);
	f(0x546C7CL)(this, type);
}