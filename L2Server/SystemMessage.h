#pragma once

class CSystemMessage
{
	enum Type
	{
		ZONE_NAME = 7,
		SKILL_NAME = 4,
		ITEM_NAME = 3,
		NPC_NAME = 2,
		NUMBER = 1,
		TEXT = 0
	};

	CHAR buff[8192];
	SIZE_T size;
	LPUINT lpVars;
	UINT varCount;
public:
	CSystemMessage(UINT id);
	void AddText(const WCHAR* wsText);
	void AddNumber(UINT number);
	void AddNpc(int npcClassId);
	void AddItem(int itemClassId);
	void AddSkill(int id, int level);
	void AddZone(int x, int y, int z);
	void AddZone(double x, double y, double z);
	PCHAR GetBuff();
	SIZE_T GetSize();
};