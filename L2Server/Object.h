#pragma once
#include "MemoryObject.h"

class CInstantZone;	//CObject->CGeneralObject->CInstantZone

//vTable: BA91D8h
class CObject : public CBaseObject
{
public:
	/* 18 */ UINT serverId; //2+
	/* 1C */ UINT objectType;
	/* 20 */ INT32 _unkn20;
	/* 24 */ bool _unkn24;
	/* 25 */ bool _aling25[3];
	/* 28 */ UINT instantZoneId;
	/* 2C */ INT32 _unkn2C;
	/* 30 */ INT32 _unkn30;
	/* 34 */ INT32 _unkn34;
	/* 38 */ UINT databaseId;
	/* 3C */ INT32 _unkn3C;

	bool IsUser();
	User* CastUser();
	bool IsSummon();
	CSummon* CastSummon();
	bool IsPet();
	CPet* CastPet();
	bool IsBoss();
	CBoss* CastBoss();
	bool IsZZoldagu();
	CPCTrap* CastPCTrap();
	bool IsPCTrap();
	CZZoldagu* CastZZoldagu();
	bool IsNpc();
	static CObject* GetObject(UINT serverId);
	CInstantZone* GetInstantZone();
};