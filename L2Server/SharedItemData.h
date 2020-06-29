#pragma once
#pragma pack( push, 1 )

//size A0h
class CSharedItemData
{
public:
	/* 00 */ LPVOID _vt;
	/* 08 */ FVector pos;
	/* 20 */ INT32 _unkn20;
	/* 24 */ INT32 _unkn24;
	/* 28 */ INT32 _unkn28;
	/* 2C */ INT32 _unkn2C;
	/* 30 */ INT32 _unkn30;
	/* 34 */ INT32 _unkn34;
	/* 38 */ INT32 _unkn38;
	/* 3C */ INT32 _unkn3C;
	/* 40 */ INT32 _unkn40;
	/* 44 */ INT32 _unkn44;
	/* 48 */ INT32 _unkn48;
	/* 4C */ INT32 _unkn4C;
	/* 50 */ bool canBePickedUp;
	/* 51 */ bool _align51[3];
	/* 54 */ INT32 _unkn54;
	/* 58 */ INT32 _unkn58;
	/* 5C */ INT32 _unkn5C;
	/* 60 */ INT32 _unkn60;
	/* 64 */ INT32 _unkn64;
	/* 68 */ INT64 amount;
	/* 70 */ INT32 _unkn70;
	/* 74 */ INT32 _unkn74;
	/* 78 */ INT32 itemId;
	/* 7C */ INT32 itemType;
	/* 80 */ UINT slotType;
	/* 84 */ UINT blessed;
	/* 88 */ UINT consumeType;
	/* 8C */ INT32 _unkn8C;
	/* 90 */ INT32 enchantLevel;
	/* 94 */ INT32 _unkn94;
	/* 98 */ bool isAugmented;
	/* 99 */ bool _align99[3];
	/* 9C */ INT32 _unkn9C;
};

#pragma pack( pop )