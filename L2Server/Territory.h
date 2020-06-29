#pragma once
#include "MemoryObject.h"

//size 78h vTable: A4E550h
class CTerritory	/* : public MemoryObject */
{
public:
//	/* 18 */ LPVOID _unkn18[12];
	/* 00 */ LPVOID _unkn[15];

	bool IsInside(FVector& pos, bool param = true);
};