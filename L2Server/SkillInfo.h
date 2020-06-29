#pragma once

class CSkillKey
{
public:
	WORD skillId;
	WORD level;
	CSkillKey();
	CSkillKey(WORD id, WORD lvl);
	bool operator == (const CSkillKey& other) const;
	bool operator != (const CSkillKey& other) const;
	bool operator< (const CSkillKey& other) const;
};

class CSkillAction2;
class CSkillEffect;
class CSkillOperateCondition;
class CTraitInfo;

enum EffectActivateTiming
{
	eat_none = 0
};

//size: 398h vTable: BFEDD8h
class CSkillInfo
{
public:
	/* 000 */ LPVOID _vTable;
	/* 008 */ const WCHAR* lpName;
	/* 010 */ int skillId;
	/* 014 */ int level;
	/* 018 */ int magicLevel; //used in abnormalTable
	/* 01C */ int isMagic;
	/* 020 */ int _unkn020;	//bool
	/* 024 */ OPERATE_TYPE operateType; //10 default
	/* 028 */ int mpConsume1;
	/* 02C */ int mpConsume2;
	/* 030 */ LPVOID _unkn030[12];
	/* 090 */ double skillHitTime;
	/* 098 */ double skillCoolTime;
	/* 0A0 */ double skillHitCancelTime;
	/* 0A8 */ double reuseDelay;
	/* 0B0 */ double _unkn0B0;
	/* 0B8 */ vector<CSkillOperateCondition*> vOperateCondition0;
	/* 0D8 */ vector<CSkillOperateCondition*> vOperateCondition1;
	/* 0F8 */ vector<CSkillOperateCondition*> vOperateCondition2;
	/* 118 */ vector<int> vConditionMsg0;
	/* 138 */ vector<int> vConditionMsg1;
	/* 158 */ vector<int> vConditionMsg2;
	/* 178 */ LPVOID _unkn178;
	/* 180 */ int _unkn180;
	/* 184 */ BASIC_PROPERTY basicProperty;
	/* 188 */ int abnormalTime;
	/* 18C */ int abnormalLevel;
	/* 190 */ AbnormalStatusSlotType slotType;
	/* 194 */ UINT ave;
	/* 198 */ UINT aveEx;
	/* 19C */ UINT _unkn19C;
	/* 1A0 */ LPVOID _unkn1A0;
	/* 1A8 */ INT32 attackType;
	/* 1AC */ INT32 attackValue;
	/* 1B0 */ TraitType traitType;
	/* 1B4 */ INT32 effectPoint;
	/* 1B8 */ DebuffType debuffType;
	/* 1BC */ BOOL irreplaceableBuff;
	/* 1C0 */ UINT nextAction;	/* none - 0, stand - 1, sit - 2, fake_death - 2 , attack - 4 */
	/* 1C4 */ BOOL fakeDeath;
	/* 1C8 */ LPVOID _unkn1C0[6];
	/* 1F8 */ vector<CTraitInfo*> vTrait;
	/* 218 */ vector<CSkillEffect*> vEffect0;
	/* 238 */ vector<CSkillEffect*> vEffect1;
	/* 258 */ vector<CSkillEffect*> vEffect2;
	/* 278 */ vector<CSkillEffect*> vEffect3;
	/* 298 */ vector<CSkillEffect*> vEffect4;
	/* 2B8 */ vector<CSkillEffect*> vEffect5;
	/* 2D8 */ vector<CSkillEffect*> vEffect6;
	/* 2F8 */ vector<CSkillEffect*> vEffect7;
	/* 318 */ vector<int> vRelatedItem;
	/* 338 */ UINT _unkn338;
	/* 33C */ UINT _unkn33C;
	/* 340 */ UINT _unkn340;
	/* 344 */ UINT affectObjectType;
	/* 348 */ LPVOID _unkn348[10];
//	/* 354 */ UINT skillRange;
//  /* 378 */ UINT reuseDelayType; //M = 0, S = 1
//  /* 390 */ UINT transformTypeFlag;
//  /* 394 */ UINT br_ave;

	void ActivateSkill(CCreature* pCaster, CObject* pTarget, double smth = 0, CSkillAction2* pAction2 = 0, EffectActivateTiming timing = eat_none, double timeMultipler = 0);
};