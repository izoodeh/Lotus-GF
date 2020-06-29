#include "StdAfx.h"
#include "SkillFx.h"
#include "HookManager.h"
#include "SkillDB.h"

CSkillFx g_SkillFx;

BOOL g_PledgeSkillAsAlliance = FALSE;

CSkillFx::CSkillFx() : antiBuffSkillId(0), stealOnlySelfbuff(0)
{
	SetName(L"SkillFx");
}

CSkillFx::~CSkillFx()
{

}

void CSkillFx::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	WriteJump(0x9BE9A5, (INT64)CSkillEffect_i_dispel_by_category_new_slot_type);
	antiBuffSkillId = GetPrivateProfileInt(_T("SkillFx"), _T("AntiBuffSkillId"), 0, g_ConfigFile);
	stealOnlySelfbuff = GetPrivateProfileInt(_T("SkillFx"), _T("StealOnlySelfbuff"), 0, g_ConfigFile);
	g_PledgeSkillAsAlliance = GetPrivateProfileInt(_T("SkillFx"), _T("PledgeAsAlliance"), 0, g_ConfigFile);

	if(g_PledgeSkillAsAlliance)
	{
		WriteJump(0x5586C7, (UINT64)CheckAllianceHook, 1);
	}
	if(antiBuffSkillId)
	{
		WriteJump(0x83FC78, (INT64)CSkillFx::CSkillEffect_p_block_buff_Pump, 95);
	}
	if(stealOnlySelfbuff)
	{
		WriteCall(0x8D5DD2, CSkillFx::CanBeStealed);
	}
	WriteJump(0x90A425, (INT64)AugmentationGlowBlockUserInfo, 5);
	WriteJump(0x90C947, (INT64)AugmentationGlowBlockCharInfo, 5);
	WriteCall(0x83B381, CSkillFx::UnequipWeapon, 1);
}

void CSkillFx::UnequipWeapon(CCreature *pCreature)
{
	try
	{
		if(pCreature->ValidCreature())
		{
			if(CSharedItemData *pWD = pCreature->GetEquipedWeapon())
			{
				if(CItemInfo *pWI = CItemInfo::Get(pWD))
				{
					if(pWI->weaponType == WeaponFlag || pWD->itemId == 8190 || pWD->itemId == 8689)
					{
						return;
					}
				}
			}

			UINT64 address = (*(PUINT64)pCreature);
			address += 0x780;
			address = (*(PUINT64)address);
			typedef void(*f)(CCreature*);
			f(reinterpret_cast<PVOID>(address))(pCreature);
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid Creature[%p]!", __FUNCTION__, pCreature);
		}
	}catch(...)
	{
		EXCEPTION_LOG;
	}
}

bool CSkillFx::CanBeStealed(AbnormalData& ad, UINT type)
{
	typedef bool (*f)(AbnormalData&, UINT);
	if(f(0x88B294L)(ad, type))
	{
		if(ad.pSI->skillId == 1323 )
		{
			return false;
		}
		if(ad.pSI->skillId == 1410 )
		{
			return false;
		}
		if(ad.pSI->skillId == 438 )
		{
			return false;
		}
		if(!ad.selfBuff)
		{
			return false;
		}
		return true;
	}

	return false;
}

double CSkillFx::GetDispelChance(double baseChance, CSkillInfo* pSIBuff, CSkillInfo *pSI, LPVOID lpSkillMod)
{
	typedef double(*f)(double, CSkillInfo*, CSkillInfo*, LPVOID);
	return f(0x52EF94L)(baseChance, pSIBuff, pSI, lpSkillMod);
}

bool CSkillFx::CSkillEffect_p_block_buff_Pump(CSkillEffect *pEffect, CCreature *pCreature, CSkillInfo *pSI, int param)
{
	guard;

	if(pSI)
	{
		if(pSI->skillId == g_SkillFx.antiBuffSkillId)
		{
			if(User *pUser = pCreature->CastUser())
			{
				pUser->pEUD->blockBuffEx = TRUE;
				unguard;
				return true;
			}
		}
	}
	pCreature->blockBuff = true;
	unguard;
	return true;
}