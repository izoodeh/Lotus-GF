#include "stdafx.h"
#include "CategoryDataDB.h"

CCategoryDataDB g_CategoryDataDB;

CCategoryDataDB::CCategoryDataDB()
{
	lpInstance = (CCategoryDataDB*) 0x00E51DF8;
}

CCategoryDataDB::~CCategoryDataDB()
{
}

bool CCategoryDataDB::IsInCategory(int classId, int categoryId)
{
	typedef bool (*f)(CCategoryDataDB*, int, int);
	return f(0x4F57A8L)(lpInstance, classId, categoryId);
}