#include "StdAfx.h"
#include "Item.h"

CItemInfo* CItemInfo::Get(CSharedItemData *pSII)
{
	typedef CItemInfo* (*f)(CSharedItemData*);
	return f(0x812A98L)(pSII);
}

bool CItem::IsWeapon()
{
	if(this)
	{
		UINT64 addr = (*(PUINT64)this);
		addr+= 0x240;
		addr = (*(PUINT64)addr);
		typedef bool (*f)(CItem*);
		return f(addr)(this);
	}
	return false;
}

bool CItem::ValidItem()
{
	if(this)
	{
		try
		{
			DWORD dwVTable = (*(LPDWORD)this);
			switch(dwVTable)
			{
			case VT_CITEM :
			case VT_CACCESSARY :
			case VT_CASSET :
			case VT_CARMOR :
			case VT_CETCITEM :
			case VT_CQUESTITEM:
			case VT_CWEAPON :
				{
					if(pSID)
					{
						return true;
					}
					break;
				}
			}
		}catch(...)
		{

		}
	}
	return false;
}

BaseAttribute* CItem::GetTotalAttribute(BaseAttribute *lpBaseAttribute)
{
	typedef BaseAttribute* (*f)(CItem*, BaseAttribute*);
	return f(0x693E4CL)(this, lpBaseAttribute);
}