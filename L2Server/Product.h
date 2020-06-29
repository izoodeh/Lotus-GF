#pragma once
#include "SPointer.h"

struct ProductItemData
{
	int itemId;
	int count;
};

//0x6B0 size vTable: A76D28h
class CProduct : public MemoryObject
{
public:

	/* 18 */  LPVOID _unkn18[29]; //RWLock
	/* 100 */ LPVOID _unkn100[32];
	/* 200 */ LPVOID _unkn200[11];
	/* 258 */ UINT price;
	/* 25C */ UINT _unkn25C;
	/* 260 */ LPVOID _unkn260[20];
	/* 300 */ LPVOID _unkn300[32];
	/* 400 */ LPVOID _unkn400[32];
	/* 500 */ LPVOID _unkn500[32];
	/* 600 */ LPVOID _unkn600[18];
	/* 690 */ vector<ProductItemData> vData;
};

typedef CSPointer<CProduct> CProductSP;