#pragma once
#pragma pack( push, 1 )

#include "WorldObject.h"
#include "SharedRWLock.h"
#include "Inventory.h"
#include "SkillInfo.h"
#include "ContributeData.h"
#include "SPointer.h"

class AtomicJob;

struct ToggleSkillData
{
	CSkillInfo *pSI;
	bool InUse;
};

typedef map<int, ToggleSkillData> ToggleSkillMap;
typedef map<CSkillKey, DWORD> SkillReuseMap;

struct ExtCreatureData
{
	bool attackTrait[TRAIT_COUNT];
	double attackTraitValue[TRAIT_COUNT];
	bool defenceTrait[TRAIT_COUNT];
	bool perfectDefenceTrait[TRAIT_COUNT];
	double defenceTraitValue[TRAIT_COUNT];
	UINT deathTime;
	BOOL leftWorld;
};

//vTable: AC87E8h size: 1CB0h
class CCreature : public CWorldObject
{
public:
	/* 0048 */ LPVOID _unkn48[23];
	/* 0100 */ LPVOID _unkn100[32];
	/* 0200 */ LPVOID _unkn200[32];
	/* 0300 */ LPVOID _unkn300[32];
	/* 0400 */ LPVOID _unkn400[32];
	/* 0500 */ LPVOID _unkn500[32];
	/* 0600 */ LPVOID _unkn600[32];
	/* 0700 */ LPVOID _unkn700[32];
	/* 0800 */ LPVOID _unkn800[32];
	/* 0900 */ LPVOID _unkn900[32];
	/* 0A00 */ LPVOID _unknA00[18];
	/* 0A90 */ CSharedCreatureData* pSD;
	/* 0A98 */ UINT sharedIndex;
	/* 0A9C */ INT32 _unknA9C;
	/* 0AA0 */ CYieldLock* lpYieldLock;
	/* 0AA8 */ CInventory inventory;
	/* 0B60 */ LPVOID _unknB60[2];
	/* 0B70 */ bool killedByGM;
	/* 0B71 */ bool _alignb71[7];
	/* 0B78 */ LPVOID _unknB78;
	/* 0B80 */ LPVOID _unknB80[11];

//SkillMod
	/* 0BD8 */ INT32 _unknBD8[2][2];
	/* 0BE8 */ LPVOID _unknBE8[3];

	/* 0C00 */ LPVOID _unknC00[32];
	/* 0D00 */ LPVOID _unknD00[5];

	/* D28 */ double attackRangeM;
	/* D30 */ double attackRangeA;
	/* D38 */ double criticalDamageM;
	/* D40 */ double criticalDamageA;

	/* D48 */ PVOID _unknD48[23];
	//EA8 - resist_dd_magic
	/* 0E00 */ LPVOID _unknE00[32];
	/* 0F00 */ LPVOID _unknF00[20];
	/* 0FA0 */ double reuseDelayMultipler[4];
	/* 0FC0 */ LPVOID _unknFC0[8];
	/* 1000 */ LPVOID _unkn1000[32];
	/* 1100 */ LPVOID _unkn1100[13];
	/* 1168 */ bool _unkn1168;
	/* 1169 */ bool _unkn1169;
	/* 116A */ bool _unkn116A;
	/* 116B */ bool blockBuff;
	/* 116C */ bool blockDebuff;
	/* 116D */ bool _unkn116D;
	/* 116E */ bool _unkn116E;
	/* 116F */ bool _unkn116F;
	/* 1170 */ LPVOID _unkn1170[12];
	/* 11D0 */ UINT enlargedBuffSlot;
	/* 11D4 */ BOOL pkProtect;
	/* 11D8 */ LPVOID _unkn11D8[5];
	/* 1200 */ LPVOID _unkn1200[2];
	/* 1210 */ bool hideMode;
	/* 1211 */ bool _align1211[7];
	/* 1218 */ LPVOID _unkn1218[24];

	/* 12D8 */ bool _unkn12D8;
	/* 12D9 */ bool _unkn12D9;
	/* 12DA */ bool _unkn12DA;
	/* 12DB */ bool param12DB;	//combat zone?
	/* 12DC */ bool _unkn12DC;
	/* 12DD */ bool _unkn12DD;
	/* 12DE */ bool _unkn12DE;
	/* 12DF */ bool _unkn12DF;
	
	/* 12E0 */ bool inSSQZone;
	/* 12E1 */ bool _unkn12E1;
	/* 12E2 */ bool inPeaceZone;
	/* 12E3 */ bool _unkn12E3;
	/* 12E4 */ INT32 _unkn12E4;
	/* 12E8 */ LPVOID _unkn12E8[3];

	/* 1300 */ LPVOID _unkn1300[15];
	/* 1378 */ double spiritShotParam;
	/* 1380 */ bool _unkn1380[3];
	/* 1383 */ bool combatMode;
	/* 1384 */ UINT _unkn1384;
	/* 1388 */ LPVOID _unkn1388[15];
	/* 1400 */ LPVOID _unkn1400[7];
	/* 1438 */ bool blockAct;
	/* 1439 */ bool _align1439[7];
	/* 1440 */ LPVOID _unkn1440[3];
	/* 1458 */ UINT talkTargetId;
	/* 145C */ UINT chairId;
	/* 1460 */ UINT targetId;
	/* 1464 */ UINT _unkn1464;
	/* 1468 */ LPVOID _unkn1460[2];
	/* 1478 */ CYieldLock m_AcquiredSkillLock;
	/* 1484 */ UINT _align1484;
	/* 1488 */ map<INT32, UINT64> m_AcquiredSkills;
	/* 14A0 */ LPVOID _unkn14A0[12];
	/* 1500 */ LPVOID _unkn1500[3];
	/* 1518 */ CYieldLock abnormalDataLock;
	/* 1524 */ UINT _unkn1524;
	/* 1528 */ AbnormalDataVector vAbnormalData;
	/* 1548 */ UINT cubicMastery;
	/* 154C */ CYieldLock cubicLock;
	/* 1558 */ LPVOID _unkn1558[3];	//cubic map
	/* 1570 */ AbnormalData hpRegenAbnormal;
	/* 15B0 */ CYieldLock regenHpLock;
	/* 15BC */ CYieldLock skillMapLock;
	/* 15C8 */ SkillReuseMap mSkillReuse; 
	/* 15E0 */ LPVOID _unkn15E0[4];

	/* 1600 */ ToggleSkillMap mToggleSkill;
	/* 1618 */ LPVOID _unkn1618[19];
	/* 16B0 */ UINT abnormalVisualEffect;
	/* 16B4 */ UINT _unkn16B4;
	/* 16B8 */ LPVOID _unkn16B8[9];
	/* 1700 */ LPVOID _unkn1700[6];
	/* 1730 */ UINT _unkn1730;
	/* 1734 */ bool *lpAttackTrait;
	/* 173C */ UINT _align173C;
	/* 1740 */ ExtCreatureData* pECD;
	/* 1748 */ LPVOID _align1748[3];
	/* 1760 */ double *lpAttackTraitValue; //trait attack value
	/* 1768 */ LPVOID _align1768[41];
	/* 18B0 */ bool *lpDefenceTrait;
	/* 18B8 */ BYTE _align18B8[34];
	/* 18DA */ bool *lpPerfectDefenceTrait;
	/* 18E2 */ BYTE _align18E2[34];
	/* 1904 */ UINT _align1904;
	/* 1908 */ double *lpDefenceTraitValue;
	/* 1910 */ LPVOID _align1910[41];
//SkillMod end
	/* 1A58 */ LPVOID _unkn1A58[7];
	/* 1A90 */ CContributeData *pContributeData;
	/* 1A98 */ LPVOID _unkn1A98[13];
	/* 1B00 */ LPVOID _unkn1B00[32];
	/* 1C00 */ LPVOID _unkn1C00[22];

	static void Init();
	static void ConstructorByOther(CCreature* pCreature, CCreature *pOther);
	static void Constructor(CCreature *pCreature);
	static int FreeSharedData(LPVOID lpInstance, CCreature *pCreature);
	static CCreatureSP* GetCreature(CCreatureSP& creatureSp, UINT serverIndex);
	void OnExtCreate();
	void OnExtDelete();


	void Action(CObject *pTarget, bool param = false, bool paramEx = false);
	bool AcquireSkill(int skillId, int level, BOOL replace = 0, BOOL param = 1);
	void AddAbnormalStatus(CSkillInfo* pSI, CCreature* pCreature = 0, bool sendInfo = true, bool serializeMessage = true);
	void AddItemToInventory(int itemId, INT64 count, bool param1 = 0, int param2 = 0, int param3 = 0, int param4 = 0, ItemOptionKey* pItemOption = 0, int param5 = 0, BaseAttribute* pAttribute = NULL);
	bool CanBeBuffed(DebuffType debuffType, AbnormalStatusSlotType slotType);
	void ChangeTarget(CObject *pTarget, UINT reason);
	void CheckAbnormalVisualEffect();
	void CheckBuffLock();
	bool DeleteAbnormalStatus(const int index, CCreature* pCreature = 0, CSkillInfo* pSI = 0, bool checkBuffLock = true, UINT skillPumpEndType = 2);
	bool DispelByAbnormal(UINT abnormalType, bool useLock = false);
	bool DeleteAcquireSkill(int skillId);
	bool Die(CCreature *pEnemy);	//virtual call
	void DispelAllByGM(UINT pumpEndType = 0);
	void DoNothing();
	int GetAcquiredSkillLevel(int skillId);
	static CCreature* GetCreatureByObjectId(LPUINT lpObjectId);
	CSharedItemData* GetEquipedWeapon();
	int GetAbnormalLevel(AbnormalStatusSlotType type, bool useLock = false);
	int GetAbnormalLevelBySkillId(int skillId, bool useLock = true);
	UINT GetItemUsedTime(int itemId);
	double GetWeaponTraitBonus(UINT attackType);
	bool HaveSkill(int skillId, int level);
	void Healthy();
	bool InfectedSkillBy(CCreature *pCaster, CSkillInfo *pSI, CSkillInfo *pSI2 = 0, double timeVal = 0, int param = 0);
	SlotType IsEquipped(int itemIndex, SlotType slot);
	bool Is(INT64 vTableOffset);
	bool IsInRange(FVector& position, double range);
	INT64 Is64(INT64 vTableOffset);
	void LeaveWorld();
	void RemoveSkillUsableTimeAll();
	bool ResurrectedBy(CCreature *pOther = 0, int expPer = 100);
	void SetAbnormalChanged(int skillId = 0, int level = 0);
	INT64 SetExp(INT64 newExp);
	void SendHome();
	void SetNewAbnormalTime(int skillId, int level, UINT abnormalTime, bool restored = false, bool lock = true);
	void SetItemUsableTime(int itemId, ULONG* timestamp, ULONG reuse);
	void SetSelfBuff(int skillId, int level, bool selfBuff);
	void StopMove();
	void UpdateStat(UPDATE_STAT type);
	bool ValidCreature();
	void ValidateParameters();
};


#pragma pack( pop )