#pragma once

class CWorld
{
	CWorld *lpInstance;	//org instance
public:
	CWorld();
	~CWorld();
	void DeleteInZoneItem(CItem *pItem);
	void DoAsyncPickItem(CItem *pItem);
	void PickAndDeleteItem(CItem *pItem);
};

extern CWorld g_World;