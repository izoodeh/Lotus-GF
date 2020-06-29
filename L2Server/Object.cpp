#include "StdAfx.h"
#include "Object.h"

bool CObject::IsNpc()
{
	return Valid(VT_CNPC);
}

bool CObject::IsUser()
{
	return Valid(VT_USER);
}

User* CObject::CastUser()
{
	if(Valid(VT_USER))
	{
		return (User*)this;
	}
	return 0;
}

bool CObject::IsSummon()
{
	return Valid(VT_CSUMMON);
}

CSummon* CObject::CastSummon()
{
	if(Valid(VT_CSUMMON))
	{
		return (CSummon*)this;
	}
	return 0;
}

bool CObject::IsPet()
{
	return Valid(VT_CPET);
}

CPet* CObject::CastPet()
{
	if(Valid(VT_CPET))
	{
		return (CPet*)this;
	}
	return 0;
}

bool CObject::IsBoss()
{
	return Valid(VT_CBOSS);
}

CBoss* CObject::CastBoss()
{
	if(Valid(VT_CBOSS))
	{
		return (CBoss*)this;
	}
	return 0;
}

bool CObject::IsZZoldagu()
{
	return Valid(VT_CZZOLDAGU);
}

CZZoldagu* CObject::CastZZoldagu()
{
	if(Valid(VT_CZZOLDAGU))
	{
		return (CZZoldagu*)this;
	}
	return 0;
}

bool CObject::IsPCTrap()
{
	return Valid(VT_CPCTRAP);
}

CPCTrap* CObject::CastPCTrap()
{
	if(Valid(VT_CPCTRAP))
	{
		return (CPCTrap*)this;
	}
	return 0;
}

CObject* CObject::GetObject(UINT serverId)
{
	typedef CObject* (*f)(UINT);
	return f(0x75A8D8L)(serverId);
}

CInstantZone* CObject::GetInstantZone()
{
	typedef CInstantZone* (*f)(CObject*);
	return f(0x75AAD0L)(this);
}