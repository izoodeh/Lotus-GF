#include "stdafx.h"
#include "World.h"

CWorld g_World;

CWorld::CWorld()
{
	lpInstance = (CWorld*)0x124239A0;
}

CWorld::~CWorld()
{
}

void CWorld::DeleteInZoneItem(CItem *pItem)
{
	typedef void (*f)(CWorld*, CItem*);
	f(0x966EACL)(lpInstance, pItem);
}

void CWorld::DoAsyncPickItem(CItem *pItem)
{
	typedef void (*f)(CWorld*, CItem*);
	f(0x971D10L)(lpInstance, pItem);
}

void CWorld::PickAndDeleteItem(CItem *pItem)
{
	typedef void (*f)(CWorld*, CItem*);
	f(0x971B10L)(lpInstance, pItem);
}