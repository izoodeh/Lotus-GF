#include "StdAfx.h"
#include "TerritoryEx.h"
#include "KillDb.h"

CTerritoryEx::CTerritoryEx(): m_Type(TerritoryNone), m_Extend(false)
{
	m_MinPos.x = static_cast<double>(INT_MAX);
	m_MinPos.y = static_cast<double>(INT_MAX);
	m_MinPos.z = static_cast<double>(INT_MAX);
	m_MaxPos.x = static_cast<double>(INT_MIN);
	m_MaxPos.y = static_cast<double>(INT_MIN);
	m_MaxPos.z = static_cast<double>(INT_MIN);
}

CDispelAbnormalTerritory* CTerritoryEx::SafeCastDispelAbnormal()
{
	if(this)
	{
		if(m_Type == TerritoryDispelAbnormal)
		{
			return (CDispelAbnormalTerritory*)this;
		}
	}
	return 0;
}

CDispelAllTerritory* CTerritoryEx::SafeCastDispelAll()
{
	if(this)
	{
		if(m_Type == TerritoryDispelAll)
		{
			return (CDispelAllTerritory*)this;
		}
	}
	return 0;
}

CPvPRewardTerritory* CTerritoryEx::SafeCastPvPReward()
{
	if(this)
	{
		if(m_Type == TerritoryPvPReward)
		{
			return (CPvPRewardTerritory*)this;
		}
	}
	return 0;
}

CBlockItemTerritory* CTerritoryEx::SafeCastBlockItem()
{
	if(this)
	{
		if(m_Type == TerritoryBlockItem)
		{
			return (CBlockItemTerritory*)this;
		}
	}
	return 0;
}

void CTerritoryEx::Set(double x, double X, double y, double Y, double z, double Z, TerritoryType type)
{
	m_Extend = false;
	m_MinPos.x = x;
	m_MinPos.y = y;
	m_MinPos.z = z;
	m_MaxPos.x = X;
	m_MaxPos.y = Y;
	m_MaxPos.z = Z;
	m_Type = type;
}

bool CTerritoryEx::IsInside(double x, double y, double z)
{
	FVector point(x, y, z);
	return IsInside(point);
}

bool CTerritoryEx::IsInside(FVector point)
{
	if(m_Extend)
	{
		if(point.x < m_MinPos.x || point.y < m_MinPos.y || point.z < m_MinPos.z)
			return false;
		if(point.x > m_MaxPos.x || point.y > m_MaxPos.y || point.z > m_MaxPos.z)
			return false;

		UINT cutCount = 0;
		
		for(INT32 n=0;n<m_Points.size();n++)
		{
			if( CrossHalfLine(point, m_Points[n], m_Points[ (n+1) % m_Points.size() ]) )
			{
				cutCount++;
			}
		}

		return (cutCount % 2) == 1;
	}else
	{
		if( point.x >= m_MinPos.x && point.x <= m_MaxPos.x)
		{
			if( point.y >= m_MinPos.y && point.y <= m_MaxPos.y)
			{
				if( point.z >= m_MinPos.z && point.z <= m_MaxPos.z)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void CTerritoryEx::AddPoint(FVector point)
{
	m_Extend = true;
	m_Points.push_back(point);

	if(m_MaxPos.x < point.x)
		m_MaxPos.x = point.x;
	if(m_MaxPos.y < point.y)
		m_MaxPos.y = point.y;
	if(m_MaxPos.z < point.z)
		m_MaxPos.z = point.z;
	if(m_MinPos.x > point.x)
		m_MinPos.x = point.x;
	if(m_MinPos.y > point.y)
		m_MinPos.y = point.y;
	if(m_MinPos.z > point.z)
		m_MinPos.z = point.z;
}

bool CTerritoryEx::CrossHalfLine(FVector point, FVector p1, FVector p2)
{
	if(p1.y == p2.y)
	{
		return false;
	}

	//sort
	if(p1.y < p2.y)
	{
		FVector tmp = p1;
		p1 = p2;
		p2 = tmp;
	}

	if( (p1.y > point.y && p2.y > point.y) /* under halfline */ || (p1.y < point.y && p2.y < point.y) /* over halfline */ )
	{
		return false;
	}

	if( p1.x < point.x && p2.x < point.x)
	{
		return false;
	}

	if( p1.x >= point.x && p2.x >= point.x)
	{
		return true;
	}

	double t = (point.y - p1.y) / (p2.y - p1.y);
	double xp = p1.x + t*(p2.x - p1.x);
	if( xp >= point.x)
	{
		return true;
	}

	return false;
}

void CTerritoryEx::AddZRange(double z1, double z2)
{
	if(m_MaxPos.z < z1)
		m_MaxPos.z = z1;
	
	if(m_MinPos.z > z1)
		m_MinPos.z = z1;

	if(m_MaxPos.z < z2)
		m_MaxPos.z = z2;
	
	if(m_MinPos.z > z2)
		m_MinPos.z = z2;
}

CPvPRewardTerritory::CPvPRewardTerritory() : m_CheckHWID(0), m_KillReuse(0), m_MinLevel(0), m_MaxLevel(80)
{

}

void CPvPRewardTerritory::AddReward(int itemId, int minAmount, int maxAmount, double chance, BOOL stealFromVictim)
{
	PvPRewardItem pi;
	pi.chance = chance;
	pi.itemId = itemId;
	pi.minAmount = minAmount;
	pi.maxAmount = maxAmount;
	pi.stealFromVictim = stealFromVictim;
	m_Items.push_back(pi);
}

void CPvPRewardTerritory::Handle(User *pUser, User* pAttacker)
{
	if(pUser->ValidUser() && pAttacker->ValidUser())
	{
		if(pUser->pSD->level >= m_MinLevel && pUser->pSD->level <= m_MaxLevel && pAttacker->pSD->level >= m_MinLevel && pAttacker->pSD->level <= m_MaxLevel)
		{
			if(m_CheckHWID)
			{
				if(pUser->pUserSocket->hardwareId.IsEqual(pAttacker->pUserSocket->hardwareId))
				{
					return;
				}
			}
			if(m_KillReuse > 0)
			{
				if(g_KillDb.IsInReuse(pAttacker->databaseId, pUser->databaseId, m_KillReuse))
				{
					return;
				}
			}

			if(pAttacker->IsNowTrade())
			{
				pAttacker->TradeCancel();
			}

			//Iterate through rewards
			for(UINT n=0;n<m_Items.size();n++)
			{
				PvPRewardItem& pi = m_Items[n];
				double dice = g_Random.RandDouble(100);
				if(dice < pi.chance)
				{
					INT64 amount = pi.minAmount;
					if(pi.maxAmount > pi.minAmount)
					{
						amount += g_Random.RandInt(pi.maxAmount - pi.minAmount + 1);
					}
					if(amount > 0)
					{
						if(pi.stealFromVictim)
						{
							if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(pi.itemId))
							{
								if(pItem->pSID->amount < amount)
								{
									amount = pItem->pSID->amount;
								}
								pUser->DeleteItemInInventory(pi.itemId, amount);
								pAttacker->AddItemToInventory(pi.itemId, amount);
							}
						}else
						{
							if(pAttacker->inventory.CheckAddable(pi.itemId, amount))
							{
								pAttacker->AddItemToInventory(pi.itemId, amount);
							}
						}
					}
				}
			}
		}
	}
}
