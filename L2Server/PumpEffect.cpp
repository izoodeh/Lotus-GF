#include "StdAfx.h"
#include "PumpEffect.h"

bool CSkillEffect_p_block_buff_ex::Pump(CCreature *pCreature, CSkillInfo *pSI, int param)
{
	if(User *pUser = pCreature->CastUser())
	{
		pUser->pEUD->blockBuffExPump = TRUE;
	}
	return false;
}

bool CSkillEffect_p_block_exp::Pump(CCreature *pCreature, CSkillInfo *pSI, int param)
{
	if(User *pUser = pCreature->CastUser())
	{
		pUser->pEUD->blockExpPump = TRUE;
	}
	return false;
}

bool CSkillEffect_p_bonus_exp::CheckPumpCondition(CCreature *pCreature)
{
	if(User *pUser = pCreature->CastUser())
	{
		return true;
	}
	return false;
}

bool CSkillEffect_p_bonus_exp::Pump(CCreature *pCreature, CSkillInfo *pSI, int param)
{
	guard;
	if(User *pUser = pCreature->CastUser())
	{
		double value = m_value + 100.0;
		value /= 100.0;
		pUser->pEUD->bonusExp *= value;
	}
	unguard;
	return false;
}

bool CSkillEffect_p_bonus_sp::CheckPumpCondition(CCreature *pCreature)
{
	if(User *pUser = pCreature->CastUser())
	{
		return true;
	}
	return false;
}

bool CSkillEffect_p_bonus_sp::Pump(CCreature *pCreature, CSkillInfo *pSI, int param)
{
	guard;
	if(User *pUser = pCreature->CastUser())
	{
		double value = m_value + 100.0;
		value /= 100.0;
		pUser->pEUD->bonusSp *= value;
	}
	unguard;
	return false;
}

bool CSkillEffect_p_bonus_drop::CheckPumpCondition(CCreature *pCreature)
{
	if(User *pUser = pCreature->CastUser())
	{
		return true;
	}
	return false;
}

bool CSkillEffect_p_bonus_drop::Pump(CCreature *pCreature, CSkillInfo *pSI, int param)
{
	guard;
	if(User *pUser = pCreature->CastUser())
	{
		double value = m_value + 100.0;
		value /= 100.0;
		pUser->pEUD->bonusDrop *= value;
	}
	unguard;
	return false;
}

bool CSkillEffect_p_bonus_spoil::CheckPumpCondition(CCreature *pCreature)
{
	if(User *pUser = pCreature->CastUser())
	{
		return true;
	}
	return false;
}

bool CSkillEffect_p_bonus_spoil::Pump(CCreature *pCreature, CSkillInfo *pSI, int param)
{
	guard;
	if(User *pUser = pCreature->CastUser())
	{
		double value = m_value + 100.0;
		value /= 100.0;
		pUser->pEUD->bonusSpoil *= value;
	}
	unguard;
	return false;
}

