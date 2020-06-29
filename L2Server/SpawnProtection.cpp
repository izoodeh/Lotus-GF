#include "StdAfx.h"
#include "SpawnProtection.h"
#include "SkillDB.h"

CSpawnProtection g_SpawnProtection;

CSpawnProtection::CSpawnProtection() : activateMsg(0), deactivateMsg(0), enabled(FALSE), skillId(0), protectionTimeout(0)
{
	SetName(L"Spawn Protection");
}

CSpawnProtection::~CSpawnProtection()
{
}

void CSpawnProtection::Init()
{
	const TCHAR* sectionName = TEXT("SpawnProtection");
	enabled = GetPrivateProfileInt(sectionName, TEXT("Enabled"), 0, g_ConfigFile);
	skillId = GetPrivateProfileInt(sectionName, TEXT("SkillId"), 0, g_ConfigFile);
	onEnterWorld = GetPrivateProfileInt(sectionName, TEXT("OnEnterWorld"), 0, g_ConfigFile);
	affectPK = GetPrivateProfileInt(sectionName, TEXT("AffectPK"), 0, g_ConfigFile);
	activateMsg = GetPrivateProfileInt(sectionName, TEXT("ActivateMsgId"), 0, g_ConfigFile);
	deactivateMsg = GetPrivateProfileInt(sectionName, TEXT("DeactivateMsgId"), 0, g_ConfigFile);
	protectionTimeout = GetPrivateProfileInt(sectionName, _T("Timeout"), 0, g_ConfigFile);

	if(enabled)
	{
		CSkillInfo *pSI = g_SkillDB.GetSkillInfo(skillId, 1, 0);
		if(pSI)
		{
			g_Log.Add(CLog::Blue, "[%S] Featue is enabled - skillId[%d]", GetName(), skillId);
		}else
		{
			g_Log.Add(CLog::Error, "[%S] Invalid skillId[%d]!", GetName(), skillId);
			enabled = FALSE;
			g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
		}
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
	}
}

void CSpawnProtection::Activate(User *pUser)
{
	guard;

	if(enabled)
	{
		if(pUser->ValidUser())
		{
			if(!pUser->pEUD->spawnProtection && !pUser->pSD->in_battlefield && !pUser->pSD->in_peacezone && pUser->pSD->alive && !pUser->olympiadUser.IsInside())
			{
				if(pUser->pSD->karma == 0 || affectPK)
				{
					pUser->AcquireSkill(skillId, 1);
					pUser->pEUD->spawnProtection = TRUE;
					pUser->ValidateParameters();
					pUser->CheckAbnormalVisualEffect();
					pUser->pUserSocket->SendSystemMessage(activateMsg);
					pUser->SendUserInfo();
					pUser->SendCharInfo();
					if(protectionTimeout > 0)
					{
						pUser->AddTimer(protectionTimeout * 1000, DEACTIVATE_SPAWN_PROTECTION_TIMER);
					}
				}
			}
		}
	}
	unguard;
}

void CSpawnProtection::Deactivate(User *pUser)
{
	guard;

	if(pUser->ValidUser() && pUser->pEUD->spawnProtection)
	{
		if(skillId > 0)
		{
			pUser->DeleteAcquireSkill(skillId);
		}
		pUser->pEUD->spawnProtection = FALSE;
		pUser->ValidateParameters();
		pUser->CheckAbnormalVisualEffect();
		pUser->pUserSocket->SendSystemMessage(deactivateMsg);
		pUser->SendUserInfo();
		pUser->SendCharInfo();
	}
	unguard;
}

BOOL CSpawnProtection::OnEnterWorld()
{
	return onEnterWorld;
}