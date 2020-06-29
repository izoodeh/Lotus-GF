#pragma once
#include "Product.h"

class CShopServer
{
	CShopServer* lpInstance;	// org instance
public:
	CShopServer();
	CProduct* GetProduct(CProductSP &productSp, int productId);
};

extern CShopServer g_ShopServer;