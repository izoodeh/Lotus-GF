#include "StdAfx.h"
#include "SkillInfo.h"

CSkillKey::CSkillKey() : skillId(0), level(0)
{
}

CSkillKey::CSkillKey(WORD id, WORD lvl) : skillId(id), level(lvl)
{
}

bool CSkillKey::operator ==(const CSkillKey &other) const
{
	if(skillId == other.skillId && level == other.level)
		return true;
	return false;
}

bool CSkillKey::operator !=(const CSkillKey &other) const
{
	if(skillId != other.skillId || level != other.level)
		return true;

	return false;
}

bool CSkillKey::operator< (const CSkillKey& other) const
{
	if (skillId < other.skillId)
		return true;

	if (skillId > other.skillId)
		return false;

	return level < other.level;
}