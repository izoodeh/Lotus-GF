#pragma once


class CCategoryDataDB
{
	CCategoryDataDB* lpInstance;
public:
	CCategoryDataDB();
	~CCategoryDataDB();
	bool IsInCategory(int classId, int categoryId);
};

extern CCategoryDataDB g_CategoryDataDB;