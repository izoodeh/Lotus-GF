#pragma once
#include "Npc.h"

class CSummon : public CNpc
{
public:
	/* 1CB0 */ LPVOID _unkn1CB0[9];
	/* 1CF8 */ BYTE _unkn1CF8[2];
	/* 1CFA */ WCHAR wSummonName[25];
	User* GetMaster();
};