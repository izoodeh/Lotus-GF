#include "StdAfx.h"
#include "ObjectDB.h"

CObjectDB g_ObjectDB;

CObjectDB::CObjectDB()
{
	lpInstance = (CObjectDB*)0xE3DB6D0;
}

CObjectDB::~CObjectDB()
{
}

int CObjectDB::GetClassIdFromName(const WCHAR *wName)
{
	typedef int(*f)(CObjectDB*, const WCHAR*);
	return f(0x76199CL)(lpInstance, wName);
}

const WCHAR* CObjectDB::GetName(UINT classId)
{
	typedef const WCHAR* (*f)(CObjectDB*, UINT);
	return f(0x761854L)(lpInstance, classId);
}