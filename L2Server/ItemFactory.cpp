#include "StdAfx.h"
#include "ItemFactory.h"
#include "HookManager.h"

void CItemFactory::Init()
{
	WriteCall(0x9A337D, CItemFactory::FindInTable);
	WriteCall(0x9A50AB, CItemFactory::FindInTable);
	WriteCall(0x9A6CB6, CItemFactory::FindInTable);
	WriteJump(0x8A89A8, (UINT64)SafeScrollEtcTypeHook, 4);
}

INT64 CItemFactory::FindInTable(UINT field, WCHAR* str)
{
	if(field == 168)
	{
		if(wcscmp(str, L"scrl_safe_enchant_am") == 0)
		{
			return 35;
		}else if(wcscmp(str, L"scrl_safe_enchant_wp") == 0)
		{
			return 36;
		}else if (wcscmp(str, L"scrl_safe_enchant_weapon_attr") == 0)
		{
			return ETCITEM_SCRL_SAFE_ENCHANT_WEAPON_ATTR;
		}
		else if (wcscmp(str, L"scrl_safe_enchant_armor_attr") == 0)
		{
			return ETCITEM_SCRL_SAFE_ENCHANT_ARMOR_ATTR;
		}
	}
	typedef UINT (*f)(UINT, WCHAR*);
	return f(0x9A2F7CL)(field, str);
}