#include "StdAfx.h"
#include "UserAction.h"
#include "HookManager.h"
#include "SchemeBuffer.h"
#include "UserDb.h"
#include "DB.h"
#include "Protector.h"
#include "SpawnProtection.h"
#include "OfflineShop.h"
#include "VoteSystem.h"
#include "DonateSystem.h"
#include "PvPSystem.h"
#include "FactionSystem.h"
#include "ItemAutoConsume.h"
#include "Championship.h"
#include "CliExt.h"
#include "TvT.h"
#include "Dungeons.h"
#include "HTMLCacheManager.h"
#include "BuilderCmd.h"
#include "AutoLoot.h"
#include "AutoAnnounce.h"
#include "ItemLock.h"
#include "ItemReuseManager.h"
#include "OlympiadSystem.h"
#include "TerritoryData.h"
#include "PCTrap.h"
#include "SkillReuseDb.h"
#include "DominionManager.h"
#include "AllInOne.h"

double g_MaxUserAttackSpeed = 1500.0;
double g_MaxUserMUseSpeed = 6.006006006006006;
double g_MaxUserPUseSpeed = 6.006006006006006;
double g_MaxUserSpeed = 250.0;
double g_MaxUserFlySpeed = 350.0;
double g_MaxCriticalDamageMul = 50.0;
double g_MaxCriticalDamageAdd = 10000;

BYTE g_ExpertiseLevelD = 20;
BYTE g_ExpertiseLevelC = 40;
BYTE g_ExpertiseLevelB = 52;
BYTE g_ExpertiseLevelA = 61;
BYTE g_ExpertiseLevelS = 76;
BYTE g_ExpertiseLevelS80 = 80;
BYTE g_ExpertiseLevelS84 = 84;

UINT g_PvPFlagTimeShort = 60;
BYTE g_PvPFlagTimeLongParam = 60;
BOOL g_DisableSubJobLevelLimit = FALSE;
BOOL g_CertificationSkillsOnSubJob = FALSE;
BOOL g_DeleteYongmaAbnormalStatus = TRUE;
vector<UINT> g_BlockedRace;

INT64* g_ExpTable = (INT64*)0xAC9448;
double g_ExpCurveMultipler[100];
BOOL g_KilledFlagOwner[32];
User* g_VictimUser[32];
User* g_EnemyUser[32];

extern BOOL g_ShowEnterWorldHtml;

void UserAction::Init()
{
	const TCHAR* sectionName = _T("UserStatCap");
	g_MaxUserAttackSpeed = GetPrivateProfileDouble(sectionName, _T("AttackSpeed"), 1500.0 , g_ConfigFile);
	g_MaxUserMUseSpeed = GetPrivateProfileDouble(sectionName, _T("MUseSpeed"), 6.006006006006006, g_ConfigFile);
	g_MaxUserPUseSpeed = GetPrivateProfileDouble(sectionName, _T("PUseSpeed"), 6.006006006006006, g_ConfigFile);
	g_MaxUserSpeed = GetPrivateProfileDouble(sectionName, _T("Speed"), 250.0, g_ConfigFile);
	g_MaxUserFlySpeed = GetPrivateProfileDouble(sectionName, _T("FlySpeed"), 350.0, g_ConfigFile);
	g_DisableSubJobLevelLimit = GetPrivateProfileInt(sectionName, _T("DisableSubJobLevelLimit"), FALSE, g_ConfigFile);
	g_CertificationSkillsOnSubJob = GetPrivateProfileInt(sectionName, _T("CertificationSkillsOnSubJob"), FALSE, g_ConfigFile);
	g_DeleteYongmaAbnormalStatus = GetPrivateProfileInt(sectionName, _T("DeleteYongmaAbnormalStatus"), TRUE, g_ConfigFile);
	g_MaxCriticalDamageMul = GetPrivateProfileDouble(sectionName, _T("CriticalDamageMul"), 50.0, g_ConfigFile);
	g_MaxCriticalDamageAdd = GetPrivateProfileDouble(sectionName, _T("CriticalDamageAdd"), 10000, g_ConfigFile);


	TCHAR temp[8190];
	memset(temp, 0, sizeof(temp));

	for(UINT n=0;n<32;n++)
	{
		g_KilledFlagOwner[n] = FALSE;
		g_VictimUser[n] = 0;
		g_EnemyUser[n] = 0;
	}

	if(GetPrivateProfileString(sectionName, _T("BlockedRaceList"), 0, temp, 8190, g_ConfigFile))
	{
		tstringstream sstr;
		sstr << temp;
		UINT race = 0;
		while(sstr >> race)
		{
			g_BlockedRace.push_back(race);
		}
	}

	g_PvPFlagTimeShort = GetPrivateProfileInt(_T("Setting"), _T("PvPFlagTimeShort"), 60, g_ConfigFile);
	g_PvPFlagTimeLongParam = GetPrivateProfileInt(_T("Setting"), _T("PvPFlagTimeLongParam"), 60, g_ConfigFile);
	
	g_ExpertiseLevelD = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseD"), 20, g_ConfigFile);
	g_ExpertiseLevelC = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseC"), 40, g_ConfigFile);
	g_ExpertiseLevelB = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseB"), 52, g_ConfigFile);
	g_ExpertiseLevelA = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseA"), 61, g_ConfigFile);
	g_ExpertiseLevelS = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseS"), 76, g_ConfigFile);
	g_ExpertiseLevelS80 = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseS80"), 80, g_ConfigFile);
	g_ExpertiseLevelS84 = GetPrivateProfileInt(_T("Setting"), _T("ExpertiseS84"), 84, g_ConfigFile);

	for(int n=0;n<100;n++)
	{
		g_ExpCurveMultipler[n] = 1.0;
	}

	for(int n=0;n<100;n++)
	{
		wstringstream key;
		key << L"ExpCurveParam_" << n;
		g_ExpCurveMultipler[n] = GetPrivateProfileDouble(sectionName, key.str().c_str(), 1.0, g_ConfigFile);
	}


	INT32 maxMainLevel = GetPrivateProfileInt(sectionName, _T("MaxMainLevel"), 85, g_ConfigFile);
	INT32 maxSubjobLevel = GetPrivateProfileInt(sectionName, _T("MaxSubjobLevel"), 80, g_ConfigFile);



	if(maxMainLevel < 99)
	{
		//CCreature::SetExp - Main
		INT64 exp = g_ExpTable[maxMainLevel + 1];
		WriteQWORD(0x52C2B5, exp);
		exp--;
		WriteQWORD(0x52C2C4, exp);
		//Creature::ValidateLevel - Main
		WriteDWORD(0x562C8C, maxMainLevel);
	}
	if(maxSubjobLevel < 99)
	{
		//CCreature::SetExp - Subjob
		INT64 exp = g_ExpTable[maxSubjobLevel + 1];
		WriteQWORD(0x52C29A, exp);
		exp--;
		WriteQWORD(0x52C2A9, exp);
		//Creature::ValidateLevel - Subjob
		WriteDWORD(0x562C9E, maxSubjobLevel);
	}

	if(g_DisableSubJobLevelLimit)
	{
		WriteJump(0x52C293, 0x52C2B3);	//CCreature::SetExp
		WriteJump(0x562C97, 0x562CA9, 1);	//CCreature::ValidateLevel
	}

	WriteBYTE(0x4773EA, g_ExpertiseLevelD);
	WriteBYTE(0x4773F3, g_ExpertiseLevelC);
	WriteBYTE(0x4773FF, g_ExpertiseLevelB);
	WriteBYTE(0x47740B, g_ExpertiseLevelA);
	WriteBYTE(0x477417, g_ExpertiseLevelS);
	WriteBYTE(0x477423, g_ExpertiseLevelS80);
	WriteBYTE(0x477431, g_ExpertiseLevelS84);

	WriteDWORD(0x88B809, g_PvPFlagTimeShort);
	WriteBYTE(0x88B80F, g_PvPFlagTimeLongParam);

	WriteRelative(0x88AD5F, &g_MaxUserAttackSpeed);
	WriteDOUBLE(0x88AD67, g_MaxUserAttackSpeed);

	WriteRelative(0x88AEF9, &g_MaxUserMUseSpeed);
	WriteDOUBLE(0x88AF01, g_MaxUserMUseSpeed);

	WriteRelative(0x88AE31, &g_MaxUserPUseSpeed);
	WriteDOUBLE(0x88AE39, g_MaxUserPUseSpeed);

	WriteRelative(0x8981FD, &g_MaxUserSpeed);
	WriteRelative(0x8981D9, &g_MaxUserFlySpeed);
	

	WriteCall(0x8D5616, UserAction::OnInit);
	WriteCall(0x5E186D, UserAction::CreateUser);
	WriteCall(0x8D4340, UserAction::Destructor);
	WriteCall(0x8CEB30, UserAction::OnRestartPointPacket);

	WriteCall(0x5A14FA, UserAction::OnChangeSubjobEnd);
	WriteCall(0x5CDAC6, UserAction::OnChangeSubjobEnd);
	WriteCall(0x5CE8B2, UserAction::OnChangeSubjobEnd);

	WriteCall(0x49B8AE, UserAction::OnChangeSubjobBegin);
	WriteCall(0x49BB49, UserAction::OnChangeSubjobBegin);
	WriteCall(0x49BC69, UserAction::OnChangeSubjobBegin);
	WriteCall(0x8A9566, UserAction::OnChangeSubjobBegin);
	WriteCall(0x8A9764, UserAction::OnChangeSubjobBegin);
	WriteCall(0x8A98F0, UserAction::OnChangeSubjobBegin);

	WriteCall(0x52E5A0, UserAction::GetRelationTo);
	WriteCall(0x569DE3, UserAction::GetRelationTo);
	WriteCall(0x8CD039, UserAction::GetRelationTo);
	WriteCall(0x8CD267, UserAction::GetRelationTo);
	WriteCall(0x8CE2AD, UserAction::GetRelationTo);
	WriteCall(0x8D0291, UserAction::GetRelationTo);
	WriteCall(0x8D05A7, UserAction::GetRelationTo);
	WriteCall(0x907FB0, UserAction::GetRelationTo);
	WriteCall(0x9082FF, UserAction::GetRelationTo);
	WriteCall(0x9088C3, UserAction::GetRelationTo);

	WriteCall(0x7CDC69, UserAction::CanAcquirePledgeSkill);
	WriteCall(0x4A237F, UserAction::StealInfectedSkills);
	WriteCall(0x837AEA, UserAction::StealInfectedSkills);

	WriteToVMT(VT_USER, 32, UserAction::OnTimerExpired);
	WriteToVMT(VT_USER, 2288, UserAction::OnLevelChanged);
	WriteToVMT(VT_USER, 0x868, UserAction::OnTeleportToLocation);
	WriteToVMT(VT_USER, 0x890, UserAction::Die);
	WriteToVMT(VT_USER, 0x570, UserAction::ExpInc);
	WriteToVMT(VT_USER, 0x848, UserAction::EnterWorld);
	WriteToVMT(VT_USER, 0x850, UserAction::LeaveWorld);
	WriteToVMT(VT_USER, 0x768, UserAction::EquipItem);
	WriteToVMT(VT_USER, 0x598, UserAction::SetKarma);
	WriteToVMT(VT_USER, 0xB10, UserAction::UseItem);
	WriteToVMT(VT_USER, 0x4F0, UserAction::ChangeClass);
	WriteToVMT(VT_USER, 0xC70, UserAction::OnNpcCreateSubJobPacket);	//C54828
	WriteToVMT(VT_USER, 0xC80, UserAction::OnNpcRenewSubJobPacket);		//C54838

	WriteCall(0x90A6B5, UserAction::GetEventMatchTeamId);
	WriteCall(0x90CBE5, UserAction::GetEventMatchTeamId);

	WriteCall(0x8CFA40, UserAction::DeleteYongmaAbnormalStatus);
	WriteCall(0x8FD944, UserAction::DeleteYongmaAbnormalStatus);
	WriteCall(0x8FE172, UserAction::DeleteYongmaAbnormalStatus);

	//new boost
	WriteCall(0x75C69D, UserAction::ComputeExpBoost);
	WriteCall(0x75C8A9, UserAction::ComputeExpBoost);
	WriteCall(0x75E93F, UserAction::ComputeExpBoost);
	WriteCall(0x75FEDC, UserAction::ComputeExpBoost);
	WriteCall(0x7602B8, UserAction::ComputeExpBoost);

	WriteCall(0x75C6C7, UserAction::ComputeSpBoost);
	WriteCall(0x75C8D3, UserAction::ComputeSpBoost);
	WriteCall(0x75E950, UserAction::ComputeSpBoost);
	WriteCall(0x75FF1F, UserAction::ComputeSpBoost);
	WriteCall(0x7602E2, UserAction::ComputeSpBoost);

	
	WriteCall(0x8525A7, UserAction::ComputeSpoilBoost);

	WriteCall(0x75D6F8, UserAction::ComputeDropBoost);
	WriteCall(0x75D79B, UserAction::ComputeDropBoost);
	WriteCall(0x75D913, UserAction::ComputeDropBoost);

	WriteCall(0x60B530, UserAction::ValidateHeroSkills);
	WriteCall(0x718E09, UserAction::ValidateHeroSkills);
	WriteCall(0x8CF45A, UserAction::ValidateHeroSkills);
	WriteCall(0x9069EE, UserAction::ValidateHeroSkills);

	WriteCall(0x767038, UserAction::EscapeFromOlympiad);
	WriteCall(0x76D1A4, UserAction::EscapeFromOlympiad);
	
	WriteCall(0x76A597, UserAction::ReadyForMatch);

	WriteCall(0x5FEB6C, UserAction::CheckDuelStartCondition);
	WriteCall(0x5FEBB8, UserAction::CheckDuelStartCondition);
	WriteCall(0x5FF6C1, UserAction::CheckDuelStartCondition);
	WriteCall(0x5FF834, UserAction::CheckDuelStartCondition);

	WriteCall(0x772024, UserAction::OnStartOlympiadMatch);
	WriteCall(0x569590, UserAction::IncreaseKarma);
	WriteCall(0x56963F, UserAction::IncreaseDuelCount);

	WriteCall(0x735EDE, UserAction::IsCallPCEnabled);
	WriteCall(0x847561, UserAction::IsCallPCEnabled);
	WriteCall(0x855797, UserAction::IsCallPCEnabled);
	WriteCall(0x8F4426, UserAction::IsCallPCEnabled);

//	WriteCall(0x91E46F, UserAction::BasicAction); IconAction
}

bool UserAction::IsCallPCEnabled(User *pUser, User *pTarget, PUINT lpResult)
{
	guard;

	bool ret = false;
	if(pTarget->ValidUser() && pUser->ValidUser())
	{
		(*lpResult) = 5;
		if(CSharedItemData *pWD = pTarget->GetEquipedWeapon())
		{
			if(CItemInfo *pWI = CItemInfo::Get(pWD))
			{
				if(pWI->weaponType == WeaponFlag)
				{
					//1843	1	a,$c1 is engaged in combat and cannot be summoned.\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
					CSystemMessage msg(1843);
					msg.AddText(pTarget->pSD->wName);
					pUser->pUserSocket->SendSystemMessage(msg);
					unguard;
					return false;
				}
			}
		}

	}

	typedef bool (*f)(User*, User*, PUINT);
	ret = f(0x8F3BC8L)(pUser, pTarget, lpResult);

	unguard;
	return ret;
}

bool UserAction::OnNpcRenewSubJobPacket(User *pUser, UINT npcId, UINT subjobId, UINT newClass)
{
	guard;
	bool ret = false;

	if((newClass == 129 || newClass == 133) && pUser->pSD->sex == 0)
	{
		int n = newClass;
		g_Log.Add(CLog::Error, "[%s] Male user[%S] tries to get female subjob class[%d] fixing to [%d]", __FUNCTION__, pUser->pSD->wName, n, --newClass);
	}
	
	if((newClass == 128 || newClass == 132) && pUser->pSD->sex == 1)
	{
		int n = newClass;
		g_Log.Add(CLog::Error, "[%s] Female user[%S] tries to get male subjob class[%d] fixing to [%d]", __FUNCTION__, pUser->pSD->wName, n, ++newClass);
	}

	typedef bool (*f)(User*, UINT, UINT, UINT);
	ret = f(0x8A97C4L)(pUser, npcId, subjobId, newClass);

	unguard;
	return ret;
}

bool UserAction::OnNpcCreateSubJobPacket(User *pUser, UINT npcId, UINT newClass)
{
	guard;
	bool ret = false;
	
	if((newClass == 129 || newClass == 133) && pUser->pSD->sex == 0)
	{
		int n = newClass;
		g_Log.Add(CLog::Error, "[%s] Male user[%S] tries to get female subjob class[%d] fixing to [%d]", __FUNCTION__, pUser->pSD->wName, n, --newClass);
	}
	
	if((newClass == 128 || newClass == 132) && pUser->pSD->sex == 1)
	{
		int n = newClass;
		g_Log.Add(CLog::Error, "[%s] Female user[%S] tries to get male subjob class[%d] fixing to [%d]", __FUNCTION__, pUser->pSD->wName, n, ++newClass);
	}

	typedef bool (*f)(User*, UINT, UINT);
	ret = f(0x8A93D4L)(pUser, npcId, newClass);

	unguard;
	return ret;
}

void UserAction::OnStartOlympiadMatch(User *pUser, const WCHAR* wSound)
{
	guard;

	typedef void(*f)(User*, const WCHAR*);
	f(0x88E748L)(pUser, wSound);

	/* Do Something with user here */



	/* End */

	unguard;
}

UINT UserAction::CheckDuelStartCondition(User *pUser, bool party)
{
	guard;

	if(pUser->olympiadUser.status != 0)
	{
		//6508	1	a,$c1 cannot duel because $c1 is participing in olympiad.\0	0	79	9B	B0	FF	a,ItemSound3.sys_impossible\0	a,	0	0	0	0	0	a,	a,none\0
		unguard;
		return 6508;
	}

	if(pUser->pEUD->tvtUser.status != TvT::UserNone)
	{
		//1931	1	a,$c1 has declined your challenge to a duel.\0	0	79	9B	B0	FF	a,	a,	0	0	0	0	0	a,	a,none\0
		unguard;
		return 1931;
	}

	typedef UINT (*f)(User*, bool);
	UINT err = f(0x8B5324L)(pUser, party);


	unguard;
	return err;
}

void UserAction::ReadyForMatch(COlympiadUser *pOU, FVector *lpPos)
{
	guard;

	//Olympiad entering - stuff done before it ports you to stadion like leave party, despawn pet etc
	if(User *pUser = pOU->pUser->CastUser())
	{
		//Do something with user
		g_OlympiadSystem.ValidateEquippedItems(pUser);
	}

	typedef void(*f)(COlympiadUser*, FVector*);
	f(0x76A224L)(pOU, lpPos);
	unguard;
}

void UserAction::EscapeFromOlympiad(COlympiadUser *pOU)
{
	guard;
	
	//Leaving olympiad stadion after match
	if(User *pUser = pOU->pUser->CastUser())
	{
		//Do Something with user
	}

	typedef void(*f)(COlympiadUser*);
	f(0x76BF88L)(pOU);
	unguard;
}

bool UserAction::ChangeClass(User *pUser, int classId)
{
	typedef bool (*f)(User*, int);
	if(f(0x8A0C2CL)(pUser, classId))
	{

		return true;
	}

	return false;
}

bool UserAction::UseItem(User *pUser, CItem *pItem, int param1, bool param2)
{
	if(pUser->ValidUser())
	{
		TerritoryData& td = pUser->pEUD->territoryData;
		if(td.territoryId[TerritoryBlockItem])
		{
			if(CBlockItemTerritory* pTerritory = g_TerritoryChecker.GetTerritory(td.territoryId[TerritoryBlockItem])->SafeCastBlockItem())
			{
				if(pTerritory->IsBlocked(pItem->pSID->itemId))
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot use this item here!");
					return false;
				}
			}
		}

		if(pUser->pEUD->championshipUser.state > Championship::UserTeleporting)
		{
			if(!g_Championship.CanUse(pItem))
			{
				return false;
			}
		}
		if(pUser->pEUD->tvtUser.status == TvT::UserPreparing || pUser->pEUD->tvtUser.status == TvT::UserFighting)
		{
			if(!g_TvT.CanUse(pItem))
			{
				return false;
			}
		}

		if(pUser->olympiadUser.IsInside())
		{
			if(!g_OlympiadSystem.CanUse(pItem))
			{
				//1508	1	a,You cannot use that item in a Grand Olympiad Games match.\0	0	79	9B	B0	FF	a,ItemSound3.sys_shortage\0	a,	0	0	0	0	0	a,	a,none\0
				pUser->pUserSocket->SendSystemMessage(1508);
				return false;
			}
		}
	}

	typedef bool (*f)(User*, CItem*, int, bool);
	return f(0x8DD00CL)(pUser, pItem, param1, param2);
}

double UserAction::ComputeExpBoost(User *pUser)
{
	typedef double (*f)(User*);
	double boost = f(0x47193CL)(pUser);
	if(pUser->ValidUser())
	{
		boost += (pUser->pEUD->bonusExp - 1.0);
	}
	return boost;
}

double UserAction::ComputeSpBoost(User *pUser)
{
	typedef double (*f)(User*);
	double boost = f(0x471954L)(pUser);
	if(pUser->ValidUser())
	{
		boost += (pUser->pEUD->bonusSp - 1.0);
	}
	return boost;
}

double UserAction::ComputeSpoilBoost(User *pUser)
{
	typedef double (*f)(User*);
	double boost = f(0x47196CL)(pUser);
	if(pUser->ValidUser())
	{
		boost += (pUser->pEUD->bonusSpoil - 1.0);
	}
	return boost;
}

double UserAction::ComputeDropBoost(User *pUser)
{
	typedef double (*f)(User*);
	double boost = f(0x471984L)(pUser);
	if(pUser->ValidUser())
	{
		boost += (pUser->pEUD->bonusDrop - 1.0);
	}
	return boost;
}

void UserAction::DeleteYongmaAbnormalStatus(User *pUser, bool param)
{
	if(g_DeleteYongmaAbnormalStatus)
	{
		typedef void (*f)(User*, bool);
		f(0x8FD128L)(pUser, param);
	}
}

char UserAction::GetEventMatchTeamId(User *pUser)
{
	typedef char (*f)(User*);
	char teamId = f(0x907694L)(pUser);
	if(pUser->ValidUser())
	{
		if(pUser->pEUD->faction.team == TeamBlue)
			teamId = 1;
		else if(pUser->pEUD->faction.team == TeamRed)
			teamId = 2;

		if(pUser->pEUD->championshipUser.state == Championship::UserFighting)
		{
			teamId = pUser->pEUD->championshipUser.teamType;
		}
		if(pUser->pEUD->tvtUser.status == TvT::UserPreparing || pUser->pEUD->tvtUser.status == TvT::UserFighting)
		{
			teamId = pUser->pEUD->tvtUser.team;
		}
	}
	return teamId;
}

void UserAction::IncreaseDuelCount(User *pUser)
{
	guard;

	UINT threadIndex = GetThreadIndex();
	if(threadIndex < 32)
	{
		if(User *pVictim = g_VictimUser[threadIndex]->CastUser())
		{
			g_AutoAnnounce.PvPMessage(pVictim, pUser);
		}
	}

	typedef void(*f)(User*);
	f(0x88BE18L)(pUser);

	unguard;
}

void UserAction::IncreaseKarma(User *pUser, bool withPk)
{
	guard;

	UINT threadIndex = GetThreadIndex();
	if(threadIndex < 32)
	{
		if(g_KilledFlagOwner[threadIndex])
		{
			unguard;
			return;
		}
	}

	if(pUser->pEUD->inBattleZone == FALSE)
	{
		if(withPk)
		{
			if(threadIndex < 32)
			{
				if(User *pVictim = g_VictimUser[threadIndex]->CastUser())
				{
					g_AutoAnnounce.PKMessage(pVictim, pUser);
				}
			}
		}
		typedef void (*f)(User*, bool);
		f(0x899314L)(pUser, withPk);
	}

	unguard;
}

void UserAction::SetKarma(User *pUser, int karma)
{
	typedef void (*f)(User*, int);
	f(0x88DAF0L)(pUser, karma);
}

bool UserAction::BasicAction(User *pUser, int id, bool ctrl, bool shift, UINT stopType)
{
	typedef bool (*f)(User*, int, bool, bool, UINT);
	return f(0x8F29A4L)(pUser, id, ctrl, shift, stopType);
}

bool UserAction::EquipItem(User *pUser, CItem *pItem, SlotType slotType)
{
	typedef bool (*f)(User*, CItem*, SlotType);

	return f(0x8E7248L)(pUser, pItem, slotType);
}

void UserAction::StealInfectedSkills(User *pUser, int slotType, int count)
{
	typedef void(*f)(User*, int, int);

	f(0x8D6030L)(pUser, slotType, count);
}

bool UserAction::CanAcquirePledgeSkill(LPVOID lpInstance, int skillId, int level, User *pUser)
{
	typedef bool (*f)(LPVOID, int, int, User*);

	if(pUser->ValidUser())
	{
		if(pUser->olympiadUser.status == 1 || pUser->olympiadUser.status == 2)	//in olympiad match
		{
			return false;
		}
	}

	return f(0x8235F8L)(lpInstance, skillId, level, pUser);
}

UINT UserAction::GetRelationTo(User *pUser, User *pOther)
{
	guard;
	typedef UINT (*f)(User*, User*);
	UINT relation = f(0x8CC944L)(pUser, pOther);

	if(pUser->ValidUser() && pOther->ValidUser())
	{
		if(pUser->pEUD->inBattleZone && pOther->pEUD->inBattleZone)
		{
			if( (pUser->pSD->party_id != 0 && pUser->pSD->party_id == pOther->pSD->party_id) ||
				(pUser->pSD->pledge_id != 0 && pUser->pSD->pledge_id == pOther->pSD->pledge_id) ||
				(pUser->pSD->alliance_id != 0 && pUser->pSD->alliance_id == pOther->pSD->alliance_id) )
			{
				unguard;
				return 0x20;	//RelationInMyParty
			}else
			{
				unguard;
				return 0x6C4;	//RelationPK
			}
		}

		if(pUser->pEUD->championshipUser.state == Championship::UserFighting)
		{
			if(pUser->pEUD->championshipUser.matchId == pOther->pEUD->championshipUser.matchId)
			{
				if(pUser->pEUD->championshipUser.teamType == pOther->pEUD->championshipUser.teamType)
				{
					relation |= 0x20;
				}
			}
		}

		if(pUser->pEUD->tvtUser.status == TvT::UserFighting && pUser->pEUD->tvtUser.team == pOther->pEUD->tvtUser.team)
		{
			relation |= 0x20;
		}

		if(pUser->pSD->alliance_id != 0 && !pUser->olympiadUser.IsInside() && pUser->pSD->alliance_id == pOther->pSD->alliance_id)
		{
			relation |= 0x00800;
		}

		//TW fix
		if(pUser->pSD->alliance_id != 0 && pOther->pSD->alliance_id == pUser->pSD->alliance_id)
		{
			if(relation & 0x800)
			{
				if(g_DominionManager.GetGlobalState() == 2)
				{
					relation ^= 0x800;
					if(pUser->olympiadUser.IsInside() == false)
					{
						relation |= 0x20;	//party relation
					}
				}
			}
		}

		if(pUser->inSSQZone && pOther->inSSQZone && pUser->pSD->pledge_id > 0 && pOther->pSD->pledge_id > 0)
		{
			if(pUser->pSD->alliance_id != 0 && pUser->pSD->alliance_id == pOther->pSD->alliance_id)
			{
				relation |= 0x20;
			}
			if(pUser->pSD->pledge_id == pOther->pSD->pledge_id)
			{
				relation |= 0x20;
			}
		}
	}

	unguard;
	return relation;
}

INT64 UserAction::ExpInc(User *pUser, INT64 newExp, bool notAffectKarma)
{
	if(pUser->ValidUser())
	{
		if((pUser->pEUD->blockExp || pUser->pEUD->blockExpPump) && newExp > 0)
		{
			return pUser->pSD->exp;
		}
	}

	if(!notAffectKarma)	// not by resurrection
	{
		if(newExp > 0)
		{
			double fExp = static_cast<double>(newExp);
			fExp *= g_ExpCurveMultipler[pUser->pSD->level];
			newExp = static_cast<INT64>(fExp);
		}
	}

	typedef INT64 (*f)(User*, INT64, bool);
	return f(0x88BF30L)(pUser, newExp, notAffectKarma);
}

bool UserAction::Die(User *pUser, CCreature *pEnemy)
{
	guard;

	typedef bool (*f)(User*, CCreature*);
	UINT threadIndex = GetThreadIndex();
	if(pUser->ValidUser())
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
		if(threadIndex < 32)
		{
			g_VictimUser[threadIndex] = pUser;
			g_EnemyUser[threadIndex] = pKiller;
		}

		if(g_PvPSystem.IsEnabled())
		{
			g_PvPSystem.OnDie(pUser, pKiller);
		}
		g_Championship.OnDie(pUser);
		g_TvT.OnDie(pUser);

		//PvP Reward Territory
		UINT territoryId = pUser->pEUD->territoryData.territoryId[TerritoryPvPReward];
		if(territoryId > 0 && pEnemy->ValidCreature())
		{
			if(pKiller->ValidUser())
			{

				bool inWar = false;
				
				if(pUser->pSD->pledge_id > 0 && pKiller->pSD->pledge_id > 0 && pUser->pSD->pledge_id != pKiller->pSD->pledge_id)
				{
					if(CPledge *pPledge = pUser->GetPledge())
					{
						if(pPledge->InWar(pKiller->pledgeServerId))
						{
							inWar = true;
						}
					}
				}
				if(pUser->pSD->is_guilty || pUser->pSD->in_battlefield || inWar || pUser->pEUD->inBattleZone)
				{
					if(CPvPRewardTerritory *pTerritory = g_TerritoryChecker.GetTerritory(territoryId)->SafeCastPvPReward())
					{
						pTerritory->Handle(pUser, pKiller);
					}
				}
			}
		}
		
		if(threadIndex < 32)
		{
			if(CSharedItemData *pWD = pUser->GetEquipedWeapon())
			{
				if(CItemInfo *pWI = CItemInfo::Get(pWD))
				{
					if(pWI->weaponType == WeaponFlag)
					{
						g_KilledFlagOwner[threadIndex] = TRUE;
					}
				}
			}
		}

		//Resurrection exploit fix
		typedef void(*fR)(CCreature*, bool, int);
		fR(0x923548L)(pUser, false, 0);
	}

	bool ret = f(0x8D0640L)(pUser, pEnemy);

	if(threadIndex < 32)
	{
		g_KilledFlagOwner[threadIndex] = FALSE;
		g_VictimUser[threadIndex] = 0;
		g_EnemyUser[threadIndex] = 0;
	}

	unguard;
	return ret;
}

bool UserAction::OnTeleportToLocation(User *pUser, int x, int y, int z, UINT instantZoneId, UINT param)
{
	typedef bool (*f)(User*, int, int, int, UINT, UINT);

	if(pUser->ValidUser() && !pUser->pSD->in_battlefield && pUser->pSD->in_peacezone)
	{
		pUser->pEUD->activateSpawnProtection = TRUE;
	}

	pUser->pEUD->teleportTick = GetTickCount();

	return f(0x8A3DF8L)(pUser, x, y, z, instantZoneId, param);
}

void UserAction::OnLevelChanged(User *pUser)
{
	typedef void(*f)(User*);
	f(0x894610L)(pUser);

	if(pUser->ValidUser())
	{
		/* On Changed level start */

		/* On Changed level end */
	}
}

void UserAction::OnTimerExpired(User *pUser, int id)
{
	guard;

	if(id == VALIDATE_AUTO_HP_TIMER)
	{
		g_ItemAutoConsume.OnTiggerHp(pUser);
		unguard;
		return;
	}
	if(id == VALIDATE_AUTO_CP_TIMER)
	{
		g_ItemAutoConsume.OnTiggerCp(pUser);
		unguard;
		return;
	}
	if(id == VALIDATE_AUTO_MP_TIMER)
	{
		g_ItemAutoConsume.OnTiggerMp(pUser);
		unguard;
		return;
	}

	if(id == DEACTIVATE_SPAWN_PROTECTION_TIMER)
	{
		g_SpawnProtection.Deactivate(pUser);
		unguard;
		return;
	}

	typedef void(*f)(User*, int);
	f(0x8BE4C8L)(pUser, id);

	if(id == 0)
	{
		if(pUser->ValidUser())
		{
			if(!pUser->pSD->alive)
			{
				pUser->pEUD->deathTick = GetTickCount();
			}

			if(pUser->inPeaceZone)
			{
				pUser->pEUD->lastPeaceZonePos.x = (int)pUser->pSD->pos.x;
				pUser->pEUD->lastPeaceZonePos.y = (int)pUser->pSD->pos.y;
				pUser->pEUD->lastPeaceZonePos.z = (int)pUser->pSD->pos.z;
			}

			pUser->StoredAbnormalTimerExpired();

			g_Protector.TimerExpired(pUser);
			
			g_ItemAutoConsume.OnTiggerHp(pUser);
			g_ItemAutoConsume.OnTiggerCp(pUser);
			g_ItemAutoConsume.OnTiggerMp(pUser);

			if(pUser->pEUD->activateSpawnProtection)
			{
				g_SpawnProtection.Activate(pUser);
				pUser->pEUD->activateSpawnProtection = FALSE;
			}
			if(pUser->pEUD->offlineShop)
			{
				g_OfflineShop.TimerExpired(pUser);
			}
			if(pUser->pEUD->updateDbTick < GetTickCount())
			{
				if(g_VoteSystem.IsEnabled())
				{
					g_DB.RequestLoadVotePoints(pUser->databaseId, pUser->pUserSocket->accountId);
				}
				if(g_DonateSystem.IsEnabled())
				{
					g_DB.RequestLoadDonatePoints(pUser->databaseId, pUser->pUserSocket->accountId);
				}
				pUser->pEUD->updateDbTick = GetTickCount() + 30000;
			}
			if(pUser->pEUD->pvpData.saveFlag && g_PvPSystem.IsEnabled())
			{
				g_DB.RequestSavePvPPoints(pUser->databaseId, pUser->pEUD->pvpData.points);
				pUser->pEUD->pvpData.saveFlag = FALSE;
			}

			g_TvT.TimerExpired(pUser);
			g_DungeonSystem.TimerExpired(pUser);

			TerritoryData& td = pUser->pEUD->territoryData;
			td.clear();
			g_TerritoryChecker.GetTerritories(pUser, td);

			if(td.territoryId[TerritoryDispelAbnormal] != 0)
			{
				if(CDispelAbnormalTerritory* pT =  g_TerritoryChecker.GetTerritory(td.territoryId[TerritoryDispelAbnormal])->SafeCastDispelAbnormal())
				{
					if(UINT abnormalType = pT->GetAbnormalType())
					{
						bool hasAbnormal = false;
						for(UINT n=0;n<40;n++)
						{
							if(pUser->pSD->abnormalTable[n][0] == abnormalType)
							{
								pUser->DispelByAbnormal(abnormalType, true);
								break;
							}
						}
					}
				}
			}

			//Battle Zone
			if(td.territoryId[TerritoryBattleZone] != 0)
			{
				if(pUser->pEUD->inBattleZone == FALSE)
				{
					pUser->pEUD->inBattleZone = TRUE;
					pUser->SetUserInfoChanged();
					pUser->SetCharInfoChanged();
					pUser->pUserSocket->SendSystemMessage(L"You have entered a Battle Zone.");
				}
			}else
			{
				if(pUser->pEUD->inBattleZone)
				{
					pUser->pEUD->inBattleZone = FALSE;
					pUser->SetPvPFlag();
					pUser->SetUserInfoChanged();
					pUser->SetCharInfoChanged();
					pUser->pUserSocket->SendSystemMessage(L"You have left a Battle Zone.");
				}
			}

			//DISPEL_ALL territory
			if(td.territoryId[TerritoryDispelAll] != 0)
			{
				if(CDispelAllTerritory* pT =  g_TerritoryChecker.GetTerritory(td.territoryId[TerritoryDispelAll])->SafeCastDispelAll())
				{
					if(pUser->pEUD->dispelAllDone == FALSE || pT->IsOnEnterOnly() == FALSE)
					{
						pUser->pEUD->dispelAllDone = TRUE;
						pUser->DispelAllByGM();
					}
				}
			}else
			{
				if(pUser->pEUD->dispelAllDone)
				{
					pUser->pEUD->dispelAllDone = FALSE;
				}
			}
			//TerritoryBlockItem
			if(td.territoryId[TerritoryBlockItem] != 0)
			{
				if(pUser->pEUD->unequipDone == FALSE)
				{
					if(CBlockItemTerritory* pTerritory = g_TerritoryChecker.GetTerritory(td.territoryId[TerritoryBlockItem])->SafeCastBlockItem())
					{
						pUser->pEUD->unequipDone = TRUE;
						//check equipped items
						for(UINT n=0;n<29;n++)
						{
							INT32 itemIndex = pUser->pSD->equipment[n];
							if(itemIndex > 0)
							{
								if(CItem *pItem = pUser->inventory.GetByIndex(itemIndex))
								{
									if(pTerritory->IsBlocked(pItem->pSID->itemId))
									{
										pUser->UnequipItem(pItem);
									}
								}
							}
						}
					}
				}
			}else
			{
				if(pUser->pEUD->unequipDone)
				{
					pUser->pEUD->unequipDone = FALSE;
				}
			}

			if(pUser->pEUD->builderUser.isRecordingRange)
			{
				if(pUser->pEUD->builderUser.rangeZone[0][0] > pUser->pSD->pos.x)
					pUser->pEUD->builderUser.rangeZone[0][0] = pUser->pSD->pos.x;
				if(pUser->pEUD->builderUser.rangeZone[0][1] < pUser->pSD->pos.x)
					pUser->pEUD->builderUser.rangeZone[0][1] = pUser->pSD->pos.x;

				if(pUser->pEUD->builderUser.rangeZone[1][0] > pUser->pSD->pos.y)
					pUser->pEUD->builderUser.rangeZone[1][0] = pUser->pSD->pos.y;
				if(pUser->pEUD->builderUser.rangeZone[1][1] < pUser->pSD->pos.y)
					pUser->pEUD->builderUser.rangeZone[1][1] = pUser->pSD->pos.y;

				if(pUser->pEUD->builderUser.rangeZone[2][0] > pUser->pSD->pos.z)
					pUser->pEUD->builderUser.rangeZone[2][0] = pUser->pSD->pos.z;
				if(pUser->pEUD->builderUser.rangeZone[2][1] < pUser->pSD->pos.z)
					pUser->pEUD->builderUser.rangeZone[2][1] = pUser->pSD->pos.z;
			}

			if(g_AllInOne.Enabled())
			{
				g_AllInOne.OnTimerExpired(pUser);
			}

			if(InterlockedExchange(&pUser->pEUD->userInfoChanged, FALSE))
			{
				pUser->SendUserInfo();
			}
			if(InterlockedExchange(&pUser->pEUD->charInfoChanged, FALSE))
			{
				pUser->SendCharInfo();
			}

			if(pUser->pEUD->kickTick)
			{
				pUser->pEUD->kickTick--;
				if(pUser->pEUD->kickTick == 0)
				{
					pUser->Kick();
				}
			}
		}
	}
	unguard;
}

void UserAction::OnChangeSubjobBegin(User* pUser)
{
	typedef void(*f)(User*);
	f(0x906198L)(pUser);

	if(pUser->ValidUser())
	{
		if(g_SchemeBuffer.IsEnabled())
		{
			g_DB.RequestSaveSchemeBuffList(pUser);
		}
		pUser->DeleteStoredAbnormals();
	}
}

void UserAction::OnChangeSubjobEnd(User* pUser, bool param)
{
	typedef void(*f)(User*, bool);
	f(0x909858L)(pUser, param);

	if(pUser->ValidUser())
	{
		if(g_SchemeBuffer.IsEnabled())
		{
			pUser->pEUD->schemeBuffList.Clear();
			pUser->pEUD->schemeBuffList.Clear(true);
			g_DB.RequestLoadSchemeBuffList(pUser);
		}
		if(g_CertificationSkillsOnSubJob && pUser->pSD->subjob_id > 0)
		{
			g_DB.RequestLoadCertificationSkills(pUser->databaseId);
		}
		g_ItemAutoConsume.TurnOff(pUser, 0);
	}
}

void UserAction::EnterWorld(User *pUser)
{
	guard;

	typedef void(*f)(User*);
	f(0x8CF0E4L)(pUser);

	if(pUser->ValidUser())
	{
		g_UserDb.Add(pUser);
		if(g_SchemeBuffer.IsEnabled())
		{
			g_DB.RequestLoadSchemeBuffList(pUser);
		}
		if(g_VoteSystem.IsEnabled())
		{
			pUser->pEUD->Lock();
			pUser->pEUD->votePoints = 0; //to avoid dupes
			pUser->pEUD->Unlock();
			g_DB.RequestLoadVotePoints(pUser->databaseId, pUser->pUserSocket->accountId);
		}
		if(g_DonateSystem.IsEnabled())
		{
			pUser->pEUD->Lock();
			pUser->pEUD->donatePoints = 0;
			pUser->pEUD->Unlock();
			g_DB.RequestLoadDonatePoints(pUser->databaseId, pUser->pUserSocket->accountId);
		}
		if(g_PvPSystem.IsEnabled())
		{
			g_DB.RequestLoadPvPData(pUser->databaseId);
		}
		if(g_AllInOne.Enabled())
		{
			g_DB.RequestLoadAIO(pUser->databaseId);
		}
		if(g_SpawnProtection.OnEnterWorld())
		{
			pUser->pEUD->activateSpawnProtection = TRUE;
		}
		if(g_CertificationSkillsOnSubJob && pUser->pSD->subjob_id > 0)
		{
			g_DB.RequestLoadCertificationSkills(pUser->databaseId);
		}
		g_FactionSystem.OnEnterWorld(pUser);
		g_Championship.OnEnterWorld(pUser);
		if(pUser->pSD->builder_level > 0)
		{
			pUser->gmWhisper = true;
			pUser->pUserSocket->SendSystemMessage(178);
		}

		if(g_ShowEnterWorldHtml)
		{
			const WCHAR* wHtml = g_HTMLCacheManager.GetHTMLFile(L"EnterWorld.htm", pUser->pSD->language);
			if(wHtml)
			{
				pUser->ShowHTML(L"EnterWorld.htm", wHtml, 777);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Cannot load EnterWorld.htm! - check HTML directory!", __FUNCTION__);
			}
		}

		if(g_AutoLoot.IsEnabled())
		{
			if(g_AutoLoot.IsActiveByDefault())
			{
				pUser->pEUD->autoLoot = TRUE;
			}
		}
		if(g_CliExt.IsEnabled())
		{
			g_DB.RequestSaveHardwareId(pUser->databaseId, pUser->pUserSocket->hardwareId.hash);
	//		g_DB.RequestSaveRealIp(pUser->databaseId, pUser->pUserSocket->realIp.full);
			g_Logger.Add(L"Enter world user[%s][%d] account[%s][%d] ip[%d.%d.%d.%d] HWID[%s]", pUser->pSD->wName, pUser->databaseId, pUser->pUserSocket->accountNameW, pUser->pUserSocket->accountId, pUser->pUserSocket->address.S_un.S_un_b.s_b1, pUser->pUserSocket->address.S_un.S_un_b.s_b2, pUser->pUserSocket->address.S_un.S_un_b.s_b3, pUser->pUserSocket->address.S_un.S_un_b.s_b4, pUser->pUserSocket->hardwareId.ToString().c_str());
			
		}
		
		g_AutoAnnounce.OnEnterWorld(pUser);
		g_ItemLock.OnEnterWorld(pUser);
		if(g_ItemReuseManager.IsEnabled())
		{
			g_ItemReuseManager.OnEnterWorld(pUser);
		}
		g_SkillReuseDb.OnEnterWorld(pUser);
	}
	unguard;
}

void UserAction::LeaveWorld(User *pUser)
{
	guard;
	if(pUser->ValidUser())
	{
		pUser->DeleteStoredAbnormals();
		if(g_SchemeBuffer.IsEnabled())
		{
			g_DB.RequestSaveSchemeBuffList(pUser);
		}

		g_FactionSystem.OnLeaveWorld(pUser);
		g_Championship.OnLeaveWorld(pUser);
		g_TvT.OnExit(pUser);

		g_UserDb.Del(pUser);
		g_BuilderCmd.OnLeaveWorld(pUser);
		g_ItemLock.OnLeaveWorld(pUser);
	}
	typedef void(*f)(User*);
	f(0x8CB090L)(pUser);
	unguard;
}

bool UserAction::OnRestartPointPacket(User *pUser, RestartPoint restartPoint)
{
	BOOL alive = pUser->pSD->alive;

	typedef bool (*f)(User*, RestartPoint);
	bool ret = f(0x8C09B4L)(pUser, restartPoint);

	if(pUser->ValidUser() && !alive)
	{
		if((restartPoint == RP_SIEGEHQ || restartPoint == RP_CASTLE || restartPoint == RP_CLANHALL) && pUser->pSD->alive )
		{
			if( pUser->pSD->hp < (pUser->pSD->max_hp * 0.7) )
			{
				pUser->pSD->hp = pUser->pSD->max_hp * 0.7;
			}
			if( pUser->pSD->mp < (pUser->pSD->max_mp * 0.7) )
			{
				pUser->pSD->mp = pUser->pSD->max_mp * 0.7;
			}
			if( pUser->pSD->cp < (pUser->pSD->max_cp * 0.7) )
			{
				pUser->pSD->cp = pUser->pSD->max_cp * 0.7;
			}
		}
	}

	return ret;
}

void UserAction::OnInit(User *pUser)
{
	if(pUser->ValidUser())
	{
		pUser->pEUD->schemeBuffList.Init(g_SchemeBuffer.GetMaxListSize(), pUser->databaseId);
	}

	typedef void(*f)(User*);
	f(0x88A6D8L)(pUser);
}

User * UserAction::CreateUser(UINT size)
{
	guard;

	typedef User* (*f)(UINT);
	User *pUser = f(0x5AEEA8L)(size);
	if(pUser)
	{
		pUser->pEUD = new CExtUserData();
	}

	unguard;

	return pUser;
}

void UserAction::Destructor(User *pUser)
{
	typedef void(*f)(User*);
	f(0x8D33C0L)(pUser);

	if(pUser->pEUD)
		delete pUser->pEUD;

	pUser->pEUD = 0;
}

void UserAction::ValidateHeroSkills(User *pUser)
{
	guard;
	typedef void(*f)(User*);
	f(0x8ACCB8L)(pUser);

	unguard;
}