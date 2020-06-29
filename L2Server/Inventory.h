#pragma once

class ItemFunctor;
typedef bool (*ItemFunctorFunction)(ItemFunctor*, CItem*);

class ItemFunctor
{
	ItemFunctorFunction func;
public:
	ItemFunctor(ItemFunctorFunction function);
	~ItemFunctor();
};

class CSlotIndexManager
{
public:
	/* 00 */ map<INT32, INT32> mData; //serverId, slotIndex ?
	/* 18 */ INT32 maxSlots;
	/* 1C */ INT32 _unkn1C;

};

//B8 size
class CInventory
{
public:
	/* 00 */ UINT creatureSharedIndex;
	/* 04 */ INT32 _unkn04;
	/* 08 */ CSharedRWLock *lpRWLock;
	/* 10 */ INT32 nCurrentWeight;
	/* 14 */ INT32 _unkn14;
	/* 18 */ map<INT32, INT64> mItems;	//?? to check
	/* 30 */ CSharedRWLock slotIndexMangerLock;
	/* 60 */ LPVOID _unkn60;
	/* 68 */ CSlotIndexManager slotIndexManager;
	/* 88 */ INT32 _unkn88;
	/* 8C */ INT32 _unkn8C;
	/* 90 */ INT32 _unkn90;
	/* 94 */ INT32 _unkn94;
	/* 98 */ LPVOID _unkn98;
	/* A0 */ LPVOID _unknA0;
	/* A8 */ LPVOID _unknA8;
	/* B0 */ LPVOID _unknB0;

	enum InventoryUpdateType
	{
		ADD = 1,
		UPDATE = 2,
		DEL = 3
	};

	CItem* GetByDBID(int databaseId);
	CItem* GetByServerID(int serverId);
	CItem* GetByIndex(int index);
	CCreature* GetOwner();
	INT64 GetAdenaAmount();
	int GetAdenaDBID();
	CItem* GetFirstItemBy(ItemFunctor& functor); /* ItemFunctor contains functions to check if item matches requirements */
	CItem* GetNextItemBy(int index, ItemFunctor& functor);
	CItem* GetHeroItem(bool notEquipped);
	CItem* GetFirstItemByClassID(int classId, bool bUnkn = false);
	CItem* GetNextItemByClassID(int index, bool bUnkn = false);
	bool HaveItemByClassId(int classId, INT64 count, bool bUnkn = false);
	bool CheckAddable(int classId, INT64 count, bool bUnkn = false);
	bool SetInventoryChanged(CItem* pItem, InventoryUpdateType updateType);
};