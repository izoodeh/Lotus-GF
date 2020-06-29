#include "StdAfx.h"
#include "MemoryObject.h"

bool MemoryObject::Valid(DWORD vTable)
{
	if(this)
	{
		try
		{
			if( (*(LPDWORD)this) == vTable )
			{
				return true;
			}
		}catch(...)
		{
			return false;
		}
	}
	return false;
}

bool CBaseObject::AddTimer(INT64 delay, INT32 id)
{
	typedef bool (*f)(CBaseObject*, INT64, INT32);
	return f(0x68E54CL)(this, delay, id);
}