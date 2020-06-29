#include "stdafx.h"
#include "InstantEffect.h"

extern BOOL g_CustomTraitSystem;

bool CSkillEffect_i_ride_wyvern::Instant(CCreature *pCreature, CObject *pTarget, CSkillInfo *pSI, CSkillAction2 *pAction, double distance)
{
	if(User *pUser = pCreature->CastUser())
	{
		if(pUser->pSD->yongma_type == 0)
		{
			return pUser->RideWyvern();
		}
	}
	return false;
}

bool CSkillEffect_i_ride_wyvern::InstantEx(CObject *pObject, CSkillInfo *pSI)
{ 
	if(User *pUser = pObject->CastUser())
	{
		if(pUser->pSD->yongma_type == 0)
		{
			return pUser->RideForEvent();
		}
	}
	return false;
}

bool CSkillEffect_i_attack_me::InstantEx(CObject *pObject, CSkillInfo *pSI)
{
	return true;
}

bool CSkillEffect_i_attack_me::Instant(CCreature *pCreature, CObject *pTarget, CSkillInfo *pSI, CSkillAction2 *pAction, double distance)
{
	guard;
	if(pTarget->IsNpc() || pTarget->IsUser() || pTarget->IsPet() || pTarget->IsSummon())
	{
		if(User *pUser = pTarget->CastUser())
		{
			if(pUser->pEUD->spawnProtection)	//wont work with spawn protection
			{
				unguard;
				return false;
			}

			if(pUser->serverId == pCreature->serverId)
			{
				unguard;
				return false;
			}
		}

		CCreature *pTargetCreature = reinterpret_cast<CCreature*>(pTarget);

		if(pTargetCreature->IsNpc())
		{
			if(pTargetCreature->pSD->notTargetable)
			{
				unguard;
				return false;
			}

			if(pTargetCreature->pSD->npc_class_id > 1027164 && pTargetCreature->pSD->npc_class_id < 1027168)
			{
				unguard;
				return false;
			}
		}

		double chance = m_chance;

		if(g_CustomTraitSystem)
		{
			if(pSI->traitType > 0 && pSI->traitType < TRAIT_COUNT)
			{
				if(pTargetCreature->pECD->perfectDefenceTrait[pSI->traitType])
				{
					chance = 0.0;
				}else
				{
					if(pTargetCreature->pECD->defenceTrait[pSI->traitType])
					{
						chance /= pTargetCreature->pECD->defenceTraitValue[pSI->traitType];
					}
				}
			}
		}

		chance *= static_cast<double>(pSI->magicLevel);
		chance /= static_cast<double>(pTargetCreature->pSD->level);

		if(chance > 0.0)
		{
			double dice = g_Random.RandDouble(100.0);

			if(dice < chance)
			{
				if(pTargetCreature->param12DB)
				{
					pTargetCreature->Action(pCreature, true, false);
					pTargetCreature->Action(pCreature, true, false);
				}else
				{
					pTargetCreature->Action(pCreature);
					pTargetCreature->Action(pCreature);
				}
				unguard;
				return true;
			}
		}
	}

	unguard;
	return false;
}