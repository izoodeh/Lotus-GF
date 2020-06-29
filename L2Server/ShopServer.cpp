#include "stdafx.h"
#include "ShopServer.h"

CShopServer g_ShopServer;

CShopServer::CShopServer()
{
	lpInstance = (CShopServer*)0xF04A30;
}

CProduct* CShopServer::GetProduct(CProductSP &productSp, int productId)
{
	typedef CProduct* (*f)(CShopServer*, CProductSP&, int);
	return f(0x47642CL)(lpInstance, productSp, productId);
}