#pragma once
#include "SkillEffect.h"

class CSkillEffect_i_ride_wyvern : public CSkillEffect
{
public:
/* 00h */ virtual void CalcSuccess(CCreature *pCreature, CObject *pTarget,CSkillInfo *pSI, CSkillAction2 *pAction) {};	//ok
/* 08h */ virtual bool Instant(CCreature *pCreature, CObject *pTarget, CSkillInfo *pSI, CSkillAction2 *pAction, double distance);	//ok
/* 10h */ virtual bool InstantEx(CObject *pObject, CSkillInfo *pSI); //ok
/* 18h */ virtual bool Tick(CObject *pObject, double param1, unsigned int param2) { return true; };	//ok
/* 20h */ virtual double GetTickValue() { return 0.0; }; // ok
/* 28h */ virtual bool CheckPumpCondition(CCreature *pCreature) { return true; };	//ok
/* 30h */ virtual bool Pump(CCreature *pCreature, CSkillInfo *pSI, int param) { return false; };	//ok
/* 38h */ virtual bool PumpStart(CCreature *pCreature, CCreature *pOther, CSkillInfo *pSI, SkillPumpStartType type) { return true; };	//ok
/* 40h */ virtual bool PumpEnd(CCreature *pCreature, CCreature *pOther, CSkillInfo *pSI, SkillPumpEndType type) { return true; };	//ok
/* 48h */ virtual bool Consume(CObject *pObject, double param) { return true; }; //ok
/* 50h */ virtual bool Unkn50() { return false; };
/* 58h */ virtual bool Unkn58() { return false; };
/* 60h */ virtual CSkillEffect* CastUnkn() { return NULL; };
/* 68h */ virtual CSkillEffect* CastConsume() { return NULL; };
/* 70h */ virtual void Unkn70() {};
};

class CSkillEffect_i_attack_me : public CSkillEffect
{
	double m_chance;
public:
/* 00h */ virtual void CalcSuccess(CCreature *pCreature, CObject *pTarget,CSkillInfo *pSI, CSkillAction2 *pAction) {};	//ok
/* 08h */ virtual bool Instant(CCreature *pCreature, CObject *pTarget, CSkillInfo *pSI, CSkillAction2 *pAction, double distance);	//ok
/* 10h */ virtual bool InstantEx(CObject *pObject, CSkillInfo *pSI); //ok
/* 18h */ virtual bool Tick(CObject *pObject, double param1, unsigned int param2) { return true; };	//ok
/* 20h */ virtual double GetTickValue() { return 0.0; }; // ok
/* 28h */ virtual bool CheckPumpCondition(CCreature *pCreature) { return true; };	//ok
/* 30h */ virtual bool Pump(CCreature *pCreature, CSkillInfo *pSI, int param) { return false; };	//ok
/* 38h */ virtual bool PumpStart(CCreature *pCreature, CCreature *pOther, CSkillInfo *pSI, SkillPumpStartType type) { return true; };	//ok
/* 40h */ virtual bool PumpEnd(CCreature *pCreature, CCreature *pOther, CSkillInfo *pSI, SkillPumpEndType type) { return true; };	//ok
/* 48h */ virtual bool Consume(CObject *pObject, double param) { return true; }; //ok
/* 50h */ virtual bool Unkn50() { return false; };
/* 58h */ virtual bool Unkn58() { return false; };
/* 60h */ virtual CSkillEffect* CastUnkn() { return NULL; };
/* 68h */ virtual CSkillEffect* CastConsume() { return NULL; };
/* 70h */ virtual void Unkn70() {};
};