#include "StdAfx.h"
#include "Inventory.h"

ItemFunctor::ItemFunctor(ItemFunctorFunction function) : func(function) 
{
}

ItemFunctor::~ItemFunctor()
{
}

CItem* CInventory::GetByDBID(int databaseId)
{
	typedef CItem*(*f)(CInventory*, int);
	return f(0x686B54L)(this, databaseId);
}

CItem* CInventory::GetByServerID(int serverId)
{
	typedef CItem*(*f)(CInventory*, int);
	return f(0x686D2CL)(this, serverId);
}

CItem* CInventory::GetByIndex(int index)
{
	typedef CItem*(*f)(CInventory*, int);
	return f(0x686F04L)(this, index);
}

CCreature* CInventory::GetOwner()
{
	typedef CCreature* (*f)(CInventory*);
	return f(0x6873A0L)(this);
}

INT64 CInventory::GetAdenaAmount()
{
	typedef INT64 (*f)(CInventory*);
	return f(0x687FC8L)(this);
}

int CInventory::GetAdenaDBID()
{
	typedef int (*f)(CInventory*);
	return f(0x688240L)(this);
}

CItem* CInventory::GetFirstItemBy(ItemFunctor &functor)
{
	typedef CItem* (*f)(CInventory*, ItemFunctor&);
	return f(0x6884B8L)(this, functor);
}

CItem* CInventory::GetNextItemBy(int index, ItemFunctor &functor)
{
	typedef CItem* (*f)(CInventory*, int, ItemFunctor&);
	return f(0x688788L)(this, index, functor);
}

CItem* CInventory::GetHeroItem(bool notEquipped)
{
	typedef CItem*(*f)(CInventory*, bool);
	return f(0x688B58L)(this, notEquipped);
}

CItem* CInventory::GetFirstItemByClassID(int classId, bool bUnkn)
{
	typedef CItem*(*f)(CInventory*, int, bool);
	return f(0x688E80L)(this, classId, bUnkn);
}

CItem* CInventory::GetNextItemByClassID(int index, bool bUnkn)
{
	typedef CItem* (*f)(CInventory*, int, bool);
	return f(0x688F88L)(this, index, bUnkn);
}

bool CInventory::HaveItemByClassId(int classId, INT64 count, bool bUnkn)
{
	typedef bool (*f)(CInventory*, int, INT64, bool);
	return f(0x68A650L)(this, classId, count, bUnkn);
}

bool CInventory::CheckAddable(int classId, INT64 count, bool bUnkn)
{
	typedef bool (*f)(CInventory*, int, INT64, bool);
	return f(0x68D4F4L)(this, classId, count, bUnkn);
}

bool CInventory::SetInventoryChanged(CItem *pItem, InventoryUpdateType updateType)
{
	typedef bool (*f)(CInventory*, CItem*, InventoryUpdateType);
	return f(0x68DEC8L)(this, pItem, updateType);
}