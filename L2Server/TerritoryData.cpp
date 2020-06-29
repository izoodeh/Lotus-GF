#include "StdAfx.h"
#include "TerritoryData.h"

CTerritoryChecker g_TerritoryChecker;

CTerritoryChecker::CTerritoryChecker()
{
}

CTerritoryChecker::~CTerritoryChecker()
{

}

void CTerritoryChecker::Init()
{
	g_Log.Add(CLog::Blue, "[%s] Loading Data...", __FUNCTION__);
	ReadData("..\\Script\\TerritoryData.txt");
}

void CTerritoryChecker::ReadData(string sFile)
{
	try
	{
		LPBYTE lpFile = 0;
		UINT len = Utils::ReadFileBinary(sFile.c_str(), lpFile);

		if(lpFile && len > 2)
		{
			wstring sLine;
			wstringstream file((PWCHAR)&lpFile[2]);
			try
			{
				if(m_Data.size() > 0)
				{
					m_Data.clear();
				}
			}catch(...)
			{
				EXCEPTION_LOG;
			}
			INT32 ID = 0;
			while(getline(file, sLine))
			{
				if(sLine.find(L"begin") == 0)
				{
					ID++;
					vector<FVector> vPoints;
					double z[2] = {0, 0};
					double zRange[2] = {0, 0};
					//begin_ex	Points={{46519;185624;-3550;-3540};{45893;186328;-3570;-3560};{45323;186305;-3580;-3570};{44754;184903;-3520;-3510};{44783;183980;-3530;-3510};{45427;181958;-3490;-3470};{46216;181782;-3490;-3480};{47796;182653;-3510;-3500};{48153;184005;-3550;-3540};{47604;184686;-3550;-3540};{47623;185092;-3510;-3500};{47198;185330;-3500;-3490};{46973;184349;-3560;-3550}}	Type=MINING_ZONE	MiningDataId=1	end_ex
					wstring points = ParseOptionString(sLine, L"Points");
					points = ReplaceChar(points, L"{", L" ");
					points = ReplaceChar(points, L"}", L" ");
					points = ReplaceChar(points, L";", L" ");
					bool first = true;
					FVector pos;
					wstringstream paramStream;
					paramStream << points;
					while(paramStream >> pos.x >> pos.y >> z[0] >> z[1])
					{
						pos.z = (z[0] + z[1]) / 2;
						vPoints.push_back(pos);
						if(first)
						{
							if(z[0] < z[1])
							{
								zRange[0] = z[0];
								zRange[1] = z[1];
							}else
							{
								zRange[1] = z[0];
								zRange[0] = z[0];
							}
						}else
						{
							if(z[0] < z[1])
							{
								if(zRange[0] > z[0])
								{
									zRange[0] = z[0];
								}
								if(zRange[1] < z[1])
								{
									zRange[1] = z[1];
								}
							}else
							{
								if(zRange[0] > z[1])
								{
									zRange[0] = z[1];
								}
								if(zRange[1] < z[0])
								{
									zRange[1] = z[0];
								}
							}
						}
						

					}

					wstring sType = ParseOptionString(sLine, L"Type");

					if(sType == L"TerritoryBattleZone")
					{
						CTerritoryEx *pTerritory = new CTerritoryEx();
						pTerritory->SetType(TerritoryBattleZone);
						for(INT32 n=0;n<vPoints.size();n++)
						{
							pTerritory->AddPoint(vPoints[n]);
						}
						pTerritory->AddZRange(zRange[0], zRange[1]);
						m_Data.insert(pair<int, CTerritoryEx*>(ID, pTerritory));
					}else if(sType == L"TerritoryDispelAll")
					{
						CDispelAllTerritory *pTerritory = new CDispelAllTerritory();
						pTerritory->SetType(TerritoryDispelAll);
						for(INT32 n=0;n<vPoints.size();n++)
						{
							pTerritory->AddPoint(vPoints[n]);
						}
						BOOL onEnter = ParseOptionInt(sLine, L"OnEnterOnly");
						pTerritory->SetOnEnterOnly(onEnter);
						pTerritory->AddZRange(zRange[0], zRange[1]);
						m_Data.insert(pair<int, CTerritoryEx*>(ID, pTerritory));
					}else if(sType == L"TerritoryDispelAbnormal")
					{
						CDispelAbnormalTerritory *pTerritory = new CDispelAbnormalTerritory();
						pTerritory->SetType(TerritoryDispelAbnormal);
						for(INT32 n=0;n<vPoints.size();n++)
						{
							pTerritory->AddPoint(vPoints[n]);
						}
						UINT abnormalType = ParseOptionInt(sLine, L"AbnormalType");
						pTerritory->SetAbnormalType(abnormalType);
						pTerritory->AddZRange(zRange[0], zRange[1]);
						m_Data.insert(pair<int, CTerritoryEx*>(ID, pTerritory));
					}else if(sType == L"TerritoryPvPReward")
					{
						CPvPRewardTerritory *pTerritory = new CPvPRewardTerritory();
						pTerritory->SetType(TerritoryPvPReward);
						for(INT32 n=0;n<vPoints.size();n++)
						{
							pTerritory->AddPoint(vPoints[n]);
						}
						pTerritory->SetMinLevel(ParseOptionInt(sLine, L"MinLevel", 0));
						pTerritory->SetMaxLevel(ParseOptionInt(sLine, L"MaxLevel", 85));
						pTerritory->SetCheckingHWID(ParseOptionInt(sLine, L"CheckHWID"));
						pTerritory->SetKillReuse(ParseOptionInt(sLine, L"KillReuse"));
						//RewardList={{item_id;min_amount;max_amount;chance(ex. 33.3333);steal_from_victim(0/1)}}
						wstring sReward = ParseOptionString(sLine, L"RewardList");
						sReward = ReplaceChar(sReward, L";", L" ");
						sReward = ReplaceChar(sReward, L"{", L" ");
						sReward = ReplaceChar(sReward, L"}", L" ");
						int itemId, minAmount, maxAmount, steal;
						double chance;
						wstringstream rewardStream(sReward);
						while(rewardStream >> itemId >> minAmount >> maxAmount >> chance >> steal)
						{
							pTerritory->AddReward(itemId, minAmount, maxAmount, chance, steal);
						}
						pTerritory->AddZRange(zRange[0], zRange[1]);
						m_Data.insert(pair<int, CTerritoryEx*>(ID, pTerritory));
					}else if(sType == L"TerritoryBlockItem")
					{
						CBlockItemTerritory *pTerritory = new CBlockItemTerritory();
						pTerritory->SetType(TerritoryBlockItem);
						for(INT32 n=0;n<vPoints.size();n++)
						{
							pTerritory->AddPoint(vPoints[n]);
						}
						wstring sItemList = ParseOptionString(sLine, L"ItemList");
						sItemList = ReplaceChar(sItemList, L";", L" ");
						sItemList = ReplaceChar(sItemList, L"{", L" ");
						sItemList = ReplaceChar(sItemList, L"}", L" ");
						wstringstream itemStream(sItemList);
						int itemId = 0;
						while(itemStream >> itemId)
						{
							pTerritory->AddItem(itemId);
						}
						
						pTerritory->AddZRange(zRange[0], zRange[1]);
						m_Data.insert(pair<int, CTerritoryEx*>(ID, pTerritory));
					}else
					{
						g_Log.Add(CLog::Error, "[%s] Unknown Type[%S] territory_id[%d]", __FUNCTION__, sType.c_str(), ID);
					}
				}
			}
			g_Log.Add(CLog::Blue, "[%s] Loaded[%d] Territories", __FUNCTION__, m_Data.size());
			delete [] lpFile;
		}else
			g_Log.Add(CLog::Error, "[%s] Cannot find file[%s]", __FUNCTION__, sFile.c_str());
	}catch(...)
	{
		EXCEPTION_LOG;
	}
}

void CTerritoryChecker::GetTerritories(CCreature *pCreature, TerritoryData &td)
{
	if(pCreature->ValidCreature())
	{
		map<int, CTerritoryEx*>::iterator Iter = m_Data.begin();
		for(;Iter!=m_Data.end();Iter++)
		{
			if(Iter->second->IsInside(pCreature->pSD->pos.x, pCreature->pSD->pos.y, pCreature->pSD->pos.z))
			{
				if(Iter->second->GetType() != TerritoryNone)
				{
					td.territoryId[Iter->second->GetType()] = Iter->first;
				}
			}
		}
	}
}

int CTerritoryChecker::IsInTerritory(FVector& Pos, TerritoryType type)
{
	map<int, CTerritoryEx*>::iterator Iter = m_Data.begin();
	for(;Iter!=m_Data.end();Iter++)
	{
		if(Iter->second->GetType() == type)
		{
			if(Iter->second->IsInside(Pos.x, Pos.y, Pos.z))
			{
				return Iter->first;
			}
		}
	}
	return 0;
}

int CTerritoryChecker::IsInTerritory(CCreature *pCreature, TerritoryType type)
{
	if(pCreature->ValidCreature())
	{
		map<int, CTerritoryEx*>::iterator Iter = m_Data.begin();
		for(;Iter!=m_Data.end();Iter++)
		{
			if(Iter->second->GetType() == type)
			{
				if(Iter->second->IsInside(pCreature->pSD->pos.x, pCreature->pSD->pos.y, pCreature->pSD->pos.z))
				{
					return Iter->first;
				}
			}
		}
	}
	return 0;
}


CTerritoryEx *CTerritoryChecker::GetTerritory(int ID)
{
	map<int, CTerritoryEx*>::iterator Iter = m_Data.find(ID);
	if(Iter!=m_Data.end())
		return Iter->second;
	return NULL;
}