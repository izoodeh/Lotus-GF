#include "stdafx.h"
#include "Npc.h"

bool CNpc::KillNpc()
{
	if(ValidCreature())
	{
		this->killedByGM = true;
		this->pContributeData->Clear();
		return this->Die(0);
	}
	return false;
}

CNpc* CNpc::GetNpc(LPUINT lpTargetId)
{
	typedef CNpc* (*f)(LPUINT);
	return f(0x86E554L)(lpTargetId);
}