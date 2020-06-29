#pragma once
#include "Creature.h"

class CNpc : public CCreature
{
public:
	bool KillNpc();
	static CNpc* GetNpc(LPUINT lpTargetId);
};