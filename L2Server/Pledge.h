#pragma once
#include "Object.h"

typedef vector<CPledge*> PledgeVector;


//vt: 0xBE08C8
class CPledge : public CObject
{
public:
	/* 040 */ CSharedPledgeData *pSD;
	/* 048 */ CYieldLock *pLock;
	/* 050 */ LPVOID _unkn50[3];
	/* 0068 */ int _unk0068;
	/* 006C */ int _unk006C;
	/* 0070 */ int _unk0070;
	/* 0074 */ int _unk0074;
	/* 0078 */ LPVOID _unkn78;
	/* 0080 */ LPVOID _unkn80;
	/* 0088 */ int _unk0088;
	/* 008C */ int _unk008C;
	/* 0090 */ int _unk0090;
	/* 0094 */ int _unk0094;
	/* 0098 */ int _unk0098;
	/* 009C */ int _unk019C;
	/* 00A0 */ int _unk00A0;
	/* 00A4 */ int _unk00A4;
	/* 00A8 */ int _unk00A8;
	/* 00AC */ int _unk00AC;
	/* 00B0 */ int _unk00B0;
	/* 00B4 */ int _unk00B4;
	/* 00B8 */ int _unk00B8;
	/* 00BC */ int _unk00BC;
	/* 00C0 */ int _unk00C0;
	/* 00C4 */ int _unk00C4;
	/* 00C8 */ int _unk00C8;
	/* 00CC */ int _unk00CC;
	/* 00D0 */ int _unk00D0;
	/* 00D4 */ int _unk00D4;
	/* 00D8 */ int _unk00D8;
	/* 00DC */ int _unk00DC;
	/* 00E0 */ int _unk00E0;
	/* 00E4 */ int _unkn0E4;
	/* 00E8 */ int _unk00E8;
	/* 00EC */ int _unk00EC;
	/* 00F0 */ int _unk00F0;
	/* 00F4 */ int _unk00F4;
	/* 00F8 */ int _unk00F8;
	/* 00FC */ int _unk00FC;
	/* 0100 */ int _unk0100;
	/* 0104 */ int _unk0104;
	/* 0108 */ int _unk0108;
	/* 010C */ int _unk010C;
	/* 0110 */ int _unk0110;
	/* 0114 */ int _unk0114;
	/* 0118 */ int _unk0118;
	/* 011C */ int _unk011C;
	/* 0120 */ int _unk0120;
	/* 0124 */ int _unk0124;
	/* 0128 */ map<UINT, UINT> m_WarList;
	/* 0140 */ set<UINT> m_AttackerList;

	static void ExtInit();
	static CPledge* GetPledge(UINT sid);
	static void SetPledgeLevel(CPledge *pPledge, int level, bool param);
	void AcquirePledgeSkillAndSave(int skillId, int level, bool param = 0, bool param2 = 0);
	void AcquireSubPledgeSkillAndSave(UINT pledgeType, int skillId, int level, bool param = 0, bool param2 = 0);
	void AutoAcquirePledgeSkills(int pledgeLevel);
	void AutoAcquireSubPledgeSkills(int pledgeLevel);
	bool HasSubPledgeType(UINT pledgeType);
	bool InWar(UINT sid);
};