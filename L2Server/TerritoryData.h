#pragma once
#include "TerritoryEx.h"

class CTerritoryChecker
{
	map<int, CTerritoryEx*> m_Data;
public:
	CTerritoryChecker();
	~CTerritoryChecker();
	void Init();
	void ReadData(string sFile);
	void GetTerritories(CCreature *pCreature, TerritoryData& td);
	int IsInTerritory(CCreature *pCreature, TerritoryType type);
	int IsInTerritory(FVector& Pos, TerritoryType type);
	CTerritoryEx* GetTerritory(int id);
};

extern CTerritoryChecker g_TerritoryChecker;