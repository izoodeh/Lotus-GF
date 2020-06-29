#include "stdafx.h"
#include "PremiumService.h"
#include "UserDb.h"
#include "HookManager.h"
#include "ShopServer.h"

LPBYTE lpCashShopPause = (LPBYTE)0xF0C7FF;

CPremiumService* lpPremiumService = (CPremiumService*)0x10BA2C10;

extern INT32 g_CustomPremiumShopItemId;
extern INT32 g_ProtocolVersion;

void CPremiumService::Init()
{
	g_Log.Add(CLog::Blue, "[Premium Service] Initializing");

	WriteBYTE(0xBE6100, 'c');
	WriteBYTE(0xBE61A8, 'c');
	WriteCall(0x9122A3, CPremiumService::RequestGetGamePoint);
	WriteCall(0x912526, CPremiumService::SendBuyItemRequest);
	WriteCall(0x485610, CPremiumService::RequestSubGamePoint);
	WriteCall(0x4854C8, CPremiumService::RequestAddGamePoint);

	Bind(0x01, CPremiumService::ReplyGetGamePoint);
	Bind(0x02, CPremiumService::ReplyBuyItem);
	Bind(0x03, CPremiumService::ReplySubGamePoint);
	Bind(0x04, CPremiumService::ReplyNetPing);
	Bind(0x05, CPremiumService::ReplyAddGamePoint);
}

bool CPremiumService::Bind(UINT id, PremiumCallback func)
{
	typedef bool (*f)(INT64, UINT, PremiumCallback);
	return f(0x7E9A64L)(0x10BA2CC0, id, func);
}

bool CPremiumService::ReplyNetPing(CPremiumServiceSocket *pSocket, const unsigned char *packet)
{
	UINT serverTick = 0;
	Disassemble(packet, "d", &serverTick);
	
	pSocket->Send("cd", 0x04, serverTick);
	return false;
}

void CPremiumService::RequestAddGamePoint(CPremiumService* lpInstance, UINT userId, INT64 points)
{
	if(lpInstance->connected && lpInstance->verified && (*lpCashShopPause) == 0)
	{
		User *pUser = g_UserDb.FindByServerId(userId);
		if(pUser->ValidUser())
		{
			if(pUser->IsNowTrade())
			{
				pUser->TradeCancel();
			}
			if(!g_CustomPremiumShopItemId)
			{
				lpInstance->pSocket->Send("cdddd", 0x05, pUser->pUserSocket->accountId, pUser->databaseId, PointDonate, points);
			}else
			{
				INT64 leftPoints = 0;
				if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(g_CustomPremiumShopItemId))
				{
					leftPoints = pItem->pSID->amount;
				}
				leftPoints += points;
				pUser->AddItemToInventory(g_CustomPremiumShopItemId, points);
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA7, pUser->databaseId, leftPoints, 0);
				}else
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA6, pUser->databaseId, leftPoints, 0);
				}

			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid user[%d]", __FUNCTION__, userId);
		}
	}
}

bool CPremiumService::ReplyAddGamePoint(CPremiumServiceSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, result = 0, type = 0;
	INT32 points = 0;
	Disassemble(packet, "dddd", &charId, &result, &type, &points);

	User *pUser = g_UserDb.FindByDatabaseId(charId);
	if(pUser->ValidUser())
	{
		if(result == 0)
		{
			if(type == PointDonate)
			{
				pUser->donatePoints = points;
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA7, pUser->databaseId, points, type);
				}else
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA6, pUser->databaseId, points, type);
				}
			}else if(type == PointVote)
			{
				pUser->pEUD->votePoints = points;
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid point type[%d] charId[%d]", __FUNCTION__, type, charId);
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] result error[%d] charId[%d] points[%d]", __FUNCTION__, result, charId, points);
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Invalid user[%d]", __FUNCTION__, charId);
	}
	return false;
}

void CPremiumService::RequestSubGamePoint(CPremiumService* lpInstance, UINT userId, INT64 points)
{
	if(lpInstance->connected && lpInstance->verified && (*lpCashShopPause) == 0)
	{
		User *pUser = g_UserDb.FindByServerId(userId);
		if(pUser->ValidUser())
		{
			if(pUser->IsNowTrade())
			{
				pUser->TradeCancel();
			}
			if(!g_CustomPremiumShopItemId)
			{
				lpInstance->pSocket->Send("cdddd", 0x03, pUser->pUserSocket->accountId, pUser->databaseId, PointDonate, points);
			}else
			{
				INT64 leftPoints = 0;
				if(CItem *pItem = pUser->inventory.GetFirstItemByClassID(g_CustomPremiumShopItemId))
				{
					if(pItem->pSID->amount >= points)
					{
						leftPoints = pItem->pSID->amount - points;
						pUser->DeleteItemInInventory(g_CustomPremiumShopItemId, points);
					}else
					{
						g_Log.Add(CLog::Error, "[%s] Not enough points[%d][%d] user[%S]", __FUNCTION__, pItem->pSID->amount, points, pUser->pSD->wName);
					}
				}
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA7, pUser->databaseId, leftPoints, 0);
				}else
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA6, pUser->databaseId, leftPoints, 0);
				}
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid user[%d]", __FUNCTION__, userId);
		}
	}
}

bool CPremiumService::ReplySubGamePoint(CPremiumServiceSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, result = 0, type = 0;
	INT32 points = 0;
	Disassemble(packet, "dddd", &charId, &result, &type, &points);

	User *pUser = g_UserDb.FindByDatabaseId(charId);
	if(pUser->ValidUser())
	{
		if(result == 0)
		{
			if(type == PointDonate)
			{
				pUser->donatePoints = points;
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA7, pUser->databaseId, points, type);
				}else
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA6, pUser->databaseId, points, type);
				}
			}else if(type == PointVote)
			{
				pUser->pEUD->votePoints = points;
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid point type[%d] charId[%d]", __FUNCTION__, type, charId);
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] result error[%d] charId[%d] points[%d]", __FUNCTION__, result, charId, points);
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Invalid user[%d]", __FUNCTION__, charId);
	}
	return false;
}

int CPremiumService::SendBuyItemRequest(CPremiumService* lpInstance, int userId, int productId, int count)
{
	if(lpInstance->connected && lpInstance->verified && (*lpCashShopPause) == 0)
	{
		User *pUser = g_UserDb.FindByServerId(userId);
		if(pUser->ValidUser())
		{
			CProductSP productSp;
			g_ShopServer.GetProduct(productSp, productId);
			if(CProduct* pProduct = productSp.get())
			{
				if(count > 0 && count < 100)
				{
					pUser->pEUD->bilingJob.productId = productId;
					pUser->pEUD->bilingJob.count = count;

					if(!g_CustomPremiumShopItemId)
					{
						lpInstance->pSocket->Send("cddddd", 0x02, pUser->pUserSocket->accountId, pUser->databaseId, productId, count, pProduct->price);
					}else
					{
						UINT result = 3;
						INT64 price = pProduct->price * count;
						g_Log.Add(CLog::Blue, "[%s] check for price[%d] [%d] * [%d]", __FUNCTION__, price, count, pProduct->price);
						if(pUser->inventory.HaveItemByClassId(g_CustomPremiumShopItemId, price))
						{
							result = 0;
						}

						pUser->EndBuyProduct();
						if(result == 0 && count > 0)
						{
							if(pUser->IsNowTrade())
								pUser->TradeCancel();

							CProductSP productSp;
							g_ShopServer.GetProduct(productSp, productId);
							if(CProduct* pProduct = productSp.get())
							{
								for(INT32 n = 0; n < count ; n++)
								{
									for(vector<ProductItemData>::iterator Iter = pProduct->vData.begin(); Iter!= pProduct->vData.end();Iter++)
									{
										if(pUser->inventory.CheckAddable(Iter->itemId, Iter->count))
										{
											pUser->AddItemToInventory(Iter->itemId, Iter->count);
										}else
										{
											g_Log.Add(CLog::Error, "[%s] Cannot add item to inventory[%d][%d] user[%S]", __FUNCTION__, Iter->itemId, Iter->count, pUser->pSD->wName);
										}
									}
								}
								if(g_ProtocolVersion > 83)
								{
									pUser->pUserSocket->Send("chd", 0xFE, 0xAA, 1);
								}else
								{
									pUser->pUserSocket->Send("chd", 0xFE, 0xA9, 1);
								}
								pUser->DeleteItemInInventory(g_CustomPremiumShopItemId, price);
								pUser->donatePoints -= price;
							}else
							{
								g_Log.Add(CLog::Error, "[%s] Invalid productId[%d] userId[%d]", __FUNCTION__, productId, pUser->pSD->dbid);
								if(g_ProtocolVersion > 83)
								{
									pUser->pUserSocket->Send("chd", 0xFE, 0xAA, -2);
								}else
								{
									pUser->pUserSocket->Send("chd", 0xFE, 0xA9, -2);
								}
							}
						}else if(result == 3)
						{
							//not enough donate points
							if(g_ProtocolVersion > 83)
							{
								pUser->pUserSocket->Send("chd", 0xFE, 0xAA, -1);
							}else
							{
								pUser->pUserSocket->Send("chd", 0xFE, 0xA9, -1);
							}
						}
					}

					return 1;
				}else
				{
					g_Log.Add(CLog::Error, "[%s] Invalid product count[%d] productId[%d] - user[%S]", __FUNCTION__, count, productId, pUser->pSD->wName);
					return -2;
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] GetProduct fail productId[%d] - user[%S]", __FUNCTION__, productId, pUser->pSD->wName);
				return -2;
			}			
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Invalid user[%d]!", __FUNCTION__, userId);
			return -9;
		}
	}
	return -6;
}

bool CPremiumService::ReplyBuyItem(CPremiumServiceSocket* pSocket, const unsigned char* packet)
{
	UINT charId = 0, result = 0;
	INT32 productId = 0, count = 0, pointsLeft = 0;
	Disassemble(packet, "ddddd", &charId, &result, &productId, &count, &pointsLeft);

	User *pUser = g_UserDb.FindByDatabaseId(charId);
	if(pUser->ValidUser())
	{
		pUser->EndBuyProduct();
		if(result == 0)
		{
			if(pUser->IsNowTrade())
				pUser->TradeCancel();

			CProductSP productSp;
			g_ShopServer.GetProduct(productSp, productId);
			if(CProduct* pProduct = productSp.get())
			{
				for(INT32 n = 0; n < count ; n++)
				{
					for(vector<ProductItemData>::iterator Iter = pProduct->vData.begin(); Iter!= pProduct->vData.end();Iter++)
					{
						if(pUser->inventory.CheckAddable(Iter->itemId, Iter->count))
						{
							pUser->AddItemToInventory(Iter->itemId, Iter->count);
						}else
						{
							g_Log.Add(CLog::Error, "[%s] Cannot add item to inventory[%d][%d] user[%S]", __FUNCTION__, Iter->itemId, Iter->count, pUser->pSD->wName);
						}
					}
				}

				INT32 price = pProduct->price * count;
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chd", 0xFE, 0xAA, 1);
				}else
				{
					pUser->pUserSocket->Send("chd", 0xFE, 0xA9, 1);
				}
				pSocket->Send("cdddd", 0x03, pUser->pUserSocket->accountId, pUser->databaseId, PointDonate, price);
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid productId[%d] userId[%d]", __FUNCTION__, productId, charId);
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chd", 0xFE, 0xAA, -2);
				}else
				{
					pUser->pUserSocket->Send("chd", 0xFE, 0xA9, -2);
				}
			}
		}else if(result == 3)
		{
			//not enough donate points
			if(g_ProtocolVersion > 83)
			{
				pUser->pUserSocket->Send("chd", 0xFE, 0xAA, -1);
			}else
			{
				pUser->pUserSocket->Send("chd", 0xFE, 0xA9, -1);
			}
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Invalid user[%d] productId[%d] count[%d] pointsLeft[%d] result[%d]", __FUNCTION__, charId, productId, count, result, pointsLeft);
	}

	return false;
}

void CPremiumService::RequestGetGamePoint(CPremiumService* lpInstance, UINT userId, UINT type)
{
	if(lpInstance->connected && lpInstance->verified && (*lpCashShopPause) == 0)
	{
		User *pUser = g_UserDb.FindByServerId(userId);
		if(pUser->ValidUser())
		{
			if(!g_CustomPremiumShopItemId)
			{
				pUser->pEUD->donatePoints = 0;
				lpInstance->pSocket->Send("cddd", 0x01, pUser->pUserSocket->accountId, pUser->databaseId, PointDonate);
			}else
			{
				CItem *pItem = pUser->inventory.GetFirstItemByClassID(g_CustomPremiumShopItemId);
				INT64 points = 0;
				if(pItem)
				{
					points = pItem->pSID->amount;
				}
				pUser->donatePoints = points;
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA7, pUser->databaseId, points, 0);
				}else
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA6, pUser->databaseId, points, 0);
				}
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find user[%d]", __FUNCTION__, userId);
		}
	}
}

bool CPremiumService::ReplyGetGamePoint(CPremiumServiceSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, error = 0;
	INT32 type = 0, points = 0;

	Disassemble(packet, "dddd", &charId, &error, &type, &points);

	if(charId)
	{
		User *pUser = g_UserDb.FindByDatabaseId(charId);
		if(pUser->ValidUser())
		{
			if(type == PointDonate)
			{
				pUser->donatePoints = points;
				if(g_ProtocolVersion > 83)
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA7, pUser->databaseId, points, type);
				}else
				{
					pUser->pUserSocket->Send("chdQd", 0xFE, 0xA6, pUser->databaseId, points, type);
				}
			}else if(type == PointVote)
			{
				pUser->pEUD->votePoints = points;
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid point type[%d] !", __FUNCTION__, type);
			}
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find user[%d] !", __FUNCTION__, charId);
		}
	}

	return false;
}