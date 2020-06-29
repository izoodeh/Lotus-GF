#include "StdAfx.h"
#include "SharedCreatureData.h"

void CSharedCreatureData::DeleteAbnormalStatus(AbnormalStatusSlotType slotType)
{
	typedef void(*f)(CSharedCreatureData*, AbnormalStatusSlotType);
	f(0x52A15CL)(this, slotType);
}

bool CSharedCreatureData::HaveMemo(int id)
{
	typedef bool (*f)(CSharedCreatureData*, int);
	return f(0x44879CL)(this, id);
}

bool CSharedCreatureData::SetMemo(int id)
{
	typedef bool (*f)(CSharedCreatureData*, int);
	return f(0x44900CL)(this, id);
}

int CSharedCreatureData::GetMemoState(int id, BOOL exState)
{
	typedef int (*f)(CSharedCreatureData*, int, BOOL);
	return f(0x737FE4L)(this, id, exState);
}

bool CSharedCreatureData::SetMemoState(int id, int slot, int state)
{
	typedef bool (*f)(CSharedCreatureData*, int, int, int);
	return f(0x449154L)(this, id, slot, state);
}

bool CSharedCreatureData::SetFlagJurnal(int id, int flag)
{
	typedef bool(*f)(CSharedCreatureData*, int, int);
	return f(0x448444L)(this, id, flag);
}

bool CSharedCreatureData::HaveNRMemo(int id)
{
	typedef bool (*f)(CSharedCreatureData*, int);
	return f(0x448C84L)(this, id);
}

void CSharedCreatureData::SetNRMemo(int id)
{
	typedef void(*f)(CSharedCreatureData*, int);
	f(0x4492ECL)(this, id);
}