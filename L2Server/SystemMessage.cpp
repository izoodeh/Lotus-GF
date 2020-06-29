#include "StdAfx.h"
#include "SystemMessage.h"

CSystemMessage::CSystemMessage(UINT id)
{
	memset(buff, 0, 8192);
	size = 0;
	varCount = 0;
	buff[0] = 0x62;
	size++;

	(*(LPUINT)(&buff[size])) = id;
	size+=4;
	//Amount of vars
	lpVars = (LPUINT)&buff[size];
	size+=4;
};

void CSystemMessage::AddText(const WCHAR* wsText)
{
	try
	{
		(*(LPUINT)(&buff[size])) = TEXT;
		size+=4;
		size_t nLen = wcslen(wsText);
		nLen *= 2; nLen += 2;
		wcscpy_s((PWCHAR)(&buff[size]), (8192 - size), wsText);
		size += nLen;
		varCount++;
		(*lpVars) = varCount;
	}
	catch(...)
	{
		g_Log.Add(CLog::Error, "[%s] Exception detected - wsText[%S]", __FUNCTION__, wsText);
	}
};

void CSystemMessage::AddNumber(UINT nNumber)
{
	(*(LPUINT)(&buff[size])) = NUMBER;
	size+=4;
	(*(LPUINT)(&buff[size])) = nNumber;
	size+=4;
	varCount++;
	(*lpVars) = varCount;
};

void CSystemMessage::AddNpc(int nClassID)
{
	(*(LPUINT)(&buff[size])) = NPC_NAME;
	size+=4;
	(*(LPUINT)(&buff[size])) = nClassID;
	size+=4;
	varCount++;
	(*lpVars) = varCount;
};

void CSystemMessage::AddItem(int nClassID)
{
	(*(LPUINT)(&buff[size])) = ITEM_NAME;
	size+=4;
	(*(LPUINT)(&buff[size])) = nClassID;
	size+=4;
	varCount++;
	(*lpVars) = varCount;
};

void CSystemMessage::AddSkill(int nSkillID, int nLevel)
{
	(*(LPUINT)(&buff[size])) = SKILL_NAME;
	size+=4;
	(*(LPUINT)(&buff[size])) = nSkillID;
	size+=4;
	(*(LPUINT)(&buff[size])) = nLevel;
	size+=4;
	varCount++;
	(*lpVars) = varCount;
};

void CSystemMessage::AddZone(int x, int y, int z)
{
	(*(LPUINT)(&buff[size])) = ZONE_NAME;
	size+=4;
	(*(LPINT)(&buff[size])) = x;
	size+=4;
	(*(LPINT)(&buff[size])) = y;
	size+=4;
	(*(LPINT)(&buff[size])) = z;
	size+=4;
	varCount++;
	(*lpVars) = varCount;
};

PCHAR CSystemMessage::GetBuff()
{
	return buff;
};

size_t CSystemMessage::GetSize()
{
	return size;
};