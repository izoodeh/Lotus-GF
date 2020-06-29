#pragma once
#pragma pack( push, 1 )

class CSharedPledgeData
{
public:
	/* 0000 */ int _n0000;
	/* 0004 */ int _n0004;
	/* 0008 */ int _n0008;
	/* 000C */ int _n000C;
	/* 0010 */ int _n0010;
	/* 0014 */ int _n0014;
	/* 0018 */ int _n0018;
	/* 001C */ int _n001C;
	/* 0020 */ int _n0020;
	/* 0024 */ int _n0024;
	/* 0028 */ int _n0028;
	/* 002C */ int _n002C;
	/* 0030 */ int _n0030;
	/* 0034 */ int _unk0034;		
	/* 0038 */ int _unk0038;
	/* 003C */ int _unk003C;
	/* 0040 */ int _unk0040;
	/* 0044 */ int _unk0044;
	/* 0048 */ int _unk0048;
	/* 004C */ int _unk004C;
	/* 0050 */ int _unk0050;
	/* 0054 */ WORD _unk0054;
	/* 0056 */ WORD _unk0056;
	/* 0058 */ int _unk0058;
	/* 005C */ int _unk005C;
	/* 0060 */ int _unk0060;
	/* 0064 */ UINT databaseId;
	/* 0068 */ WCHAR wName[24];
	/* 0098 */ UINT _align98;
	/* 009C */ WCHAR wLeaderName[25];
	/* 00CE */ WORD _unk00CE;
	/* 00D0 */ int _unk00D0;
	/* 00D4 */ int _unk00D4;
	/* 00D8 */ int _unk00D8;
	/* 00DC */ int activeLevel;
	/* 00E0 */ int _n00E0;
	/* 00E4 */ int level;


	/* 130 */ UINT _masterServerId;
};


#pragma pack( pop )