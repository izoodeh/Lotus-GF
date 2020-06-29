#include "stdafx.h"
#include "SkillAcquireDB.h"

CSkillAcquireDB g_SkillAcquireDB;
CSkillAcquireDbEx g_SkillAcquireDbEx;

CSkillAcquireDB::CSkillAcquireDB()
{
	lpInstance = (CSkillAcquireDB*)0x10F69500;
}

CSkillAcquireDB::~CSkillAcquireDB()
{

}

const vector<SkillAcquireInfo*>& CSkillAcquireDB::GetAcquirableEtcSkills(EtcSkillAcquireType type)
{
	typedef const vector<SkillAcquireInfo*>& (*f)(CSkillAcquireDB*, EtcSkillAcquireType);
	return f(0x8229E4L)(lpInstance, type);
}

const vector<SkillAcquireInfo*>& CSkillAcquireDB::GetAcquirableSkills(UINT classType)
{
	typedef const vector<SkillAcquireInfo*>& (*f)(CSkillAcquireDB*, UINT);
	return f(0x822978L)(lpInstance, classType);
}

void CSkillAcquireDbEx::Init()
{
	CTL;
	m_ClassId[L"fighter"] = 0;
	m_ClassId[L"warrior"] = 1;
	m_ClassId[L"warlord"] = 3;
	m_ClassId[L"dreadnought"] = 89;
	m_ClassId[L"gladiator"] = 2;
	m_ClassId[L"duelist"] = 88;
	m_ClassId[L"knight"] = 4;
	m_ClassId[L"paladin"] = 5;
	m_ClassId[L"phoenix_knight"] = 90;
	m_ClassId[L"dark_avenger"] = 6;
	m_ClassId[L"hell_knight"] = 91;
	m_ClassId[L"rogue"] = 7;
	m_ClassId[L"treasure_hunter"] = 8;
	m_ClassId[L"adventurer"] = 93;
	m_ClassId[L"hawkeye"] = 9;
	m_ClassId[L"sagittarius"] = 92;
	m_ClassId[L"elven_fighter"] = 18;
	m_ClassId[L"elven_knight"] = 19;
	m_ClassId[L"temple_knight"] = 20;
	m_ClassId[L"evas_templar"] = 99;
	m_ClassId[L"swordsinger"] = 21;
	m_ClassId[L"sword_muse"] = 100;
	m_ClassId[L"elven_scout"] = 22;
	m_ClassId[L"plain_walker"] = 23;
	m_ClassId[L"plains_walker"] = 23;
	m_ClassId[L"wind_rider"] = 101;
	m_ClassId[L"silver_ranger"] = 24;
	m_ClassId[L"moonlight_sentinel"] = 102;
	m_ClassId[L"dark_fighter"] = 31;
	m_ClassId[L"palus_knight"] = 32;
	m_ClassId[L"shillien_knight"] = 33;
	m_ClassId[L"shillien_templar"] = 106;
	m_ClassId[L"bladedancer"] = 34;
	m_ClassId[L"spectral_dancer"] = 107;
	m_ClassId[L"assasin"] = 35;
	m_ClassId[L"assassin"] = 35;
	m_ClassId[L"abyss_walker"] = 36;
	m_ClassId[L"ghost_hunter"] = 108;
	m_ClassId[L"phantom_ranger"] = 37;
	m_ClassId[L"ghost_sentinel"] = 109;
	m_ClassId[L"orc_fighter"] = 44;
	m_ClassId[L"orc_raider"] = 45;
	m_ClassId[L"destroyer"] = 46;
	m_ClassId[L"titan"] = 113;
	m_ClassId[L"orc_monk"] = 47;
	m_ClassId[L"tyrant"] = 48;
	m_ClassId[L"grand_khavatari"] = 114;
	m_ClassId[L"dwarven_fighter"] = 53;
	m_ClassId[L"scavenger"] = 54;
	m_ClassId[L"bounty_hunter"] = 55;
	m_ClassId[L"fortune_seeker"] = 117;
	m_ClassId[L"artisan"] = 56;
	m_ClassId[L"warsmith"] = 57;
	m_ClassId[L"maestro"] = 118;
	m_ClassId[L"kamael_m_soldier"] = 123;
	m_ClassId[L"trooper"] = 125;
	m_ClassId[L"berserker"] = 127;
	m_ClassId[L"doombringer"] = 131;
	m_ClassId[L"m_soul_breaker"] = 128;
	m_ClassId[L"m_soul_hound"] = 132;
	m_ClassId[L"kamael_f_soldier"] = 124;
	m_ClassId[L"warder"] = 126;
	m_ClassId[L"arbalester"] = 130;
	m_ClassId[L"trickster"] = 134;
	m_ClassId[L"f_soul_breaker"] = 129;
	m_ClassId[L"f_soul_hound"] = 133;
	m_ClassId[L"inspector"] = 135;
	m_ClassId[L"judicator"] = 136;
	m_ClassId[L"mage"] = 10;
	m_ClassId[L"wizard"] = 11;
	m_ClassId[L"sorcerer"] = 12;
	m_ClassId[L"archmage"] = 94;
	m_ClassId[L"necromancer"] = 13;
	m_ClassId[L"soultaker"] = 95;
	m_ClassId[L"warlock"] = 14;
	m_ClassId[L"arcana_lord"] = 96;
	m_ClassId[L"cleric"] = 15;
	m_ClassId[L"bishop"] = 16;
	m_ClassId[L"cardinal"] = 97;
	m_ClassId[L"bishop_sharing"] = 97;
	m_ClassId[L"prophet"] = 17;
	m_ClassId[L"hierophant"] = 98;
	m_ClassId[L"elven_mage"] = 25;
	m_ClassId[L"elven_wizard"] = 26;
	m_ClassId[L"spellsinger"] = 27;
	m_ClassId[L"mystic_muse"] = 103;
	m_ClassId[L"elemental_summoner"] = 28;
	m_ClassId[L"elemental_master"] = 104;
	m_ClassId[L"oracle"] = 29;
	m_ClassId[L"elder"] = 30;
	m_ClassId[L"evas_saint"] = 105;
	m_ClassId[L"elder_sharing"] = 105;
	m_ClassId[L"dark_mage"] = 38;
	m_ClassId[L"dark_wizard"] = 39;
	m_ClassId[L"spellhowler"] = 40;
	m_ClassId[L"storm_screamer"] = 110;
	m_ClassId[L"phantom_summoner"] = 41;
	m_ClassId[L"spectral_master"] = 111;
	m_ClassId[L"shillien_oracle"] = 42;
	m_ClassId[L"shillien_elder"] = 43;
	m_ClassId[L"shillien_saint"] = 112;
	m_ClassId[L"silen_elder_sharing"] = 112;
	m_ClassId[L"orc_mage"] = 49;
	m_ClassId[L"orc_shaman"] = 50;
	m_ClassId[L"overlord"] = 51;
	m_ClassId[L"dominator"] = 115;
	m_ClassId[L"warcryer"] = 52;
	m_ClassId[L"doomcryer"] = 116;
	
	LoadData();

	g_Log.Add(CLog::Blue, "[%s] Loaded [%d] acquirable skills for [%d] classes.", __FUNCTION__, m_AcquirableSkills.size(), m_ClassSkills.size());

}

wstring CSkillAcquireDbEx::GetSkillNameById(SkillPchId pch)
{
	for(map<wstring, SkillPchId>::iterator it = m_SkillPch.begin(); it != m_SkillPch.end(); it++)
	{
		if(it->second.pchId == pch.pchId)
		{
			return it->first;
		}
	}
	return L"InvalidSkill";
}

UINT CSkillAcquireDbEx::GetClassByName(wstring name)
{
	CTL;

	UINT cl = UINT_MAX;
	map<wstring, UINT>::iterator it = m_ClassId.find(name);
	if(it != m_ClassId.end())
	{
		cl = it->second;
	}

	return cl;
}

bool CSkillAcquireDbEx::GetSkillIdByName(wstring name, SkillPchId& pchId)
{
	CTL;

	bool ret = false;

	map<wstring, SkillPchId>::iterator it = m_SkillPch.find(name);
	if(it!=m_SkillPch.end())
	{
		pchId = it->second;
		ret = true;
	}
	

	return ret;
}

UINT CSkillAcquireDbEx::GetMaxAcquirableSkillLevel(int skillId)
{
	CTL;
	UINT maxLevel = 0;

	for(UINT n=0;n<m_MaxAcquirableLevel.size();n++)
	{
		if(m_MaxAcquirableLevel[n].skillId == skillId)
		{
			if(maxLevel < m_MaxAcquirableLevel[n].level)
			{
				maxLevel = m_MaxAcquirableLevel[n].level;
			}
		}
	}

	return maxLevel;
};

bool CSkillAcquireDbEx::IsAcquirableSkill(int skillId, int level)
{
	CTL;
	bool ret = false;
	SkillPchId pch(skillId, level);
	for(UINT n=0;n<m_AcquirableSkills.size();n++)
	{
		if(m_AcquirableSkills[n].pchId == pch.pchId)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

int CSkillAcquireDbEx::GetMaxAvailableLevel(UINT classId, int skillId, int level)
{
	CTL;
	int acquirableLevel = 0;
	vector<SkillAcquireInfoEx>& skills = m_ClassSkills[classId];
	for(UINT n=0;n<skills.size();n++)
	{
		SkillAcquireInfoEx &sa = skills[n];
		if(sa.skill.skillId == skillId)
		{
			if(sa.requiredLevel <= level)
			{
				acquirableLevel = sa.skill.level;
			}
		}
	}

	return acquirableLevel;
}

bool CSkillAcquireDbEx::CanAcquire(UINT classId, int skillId, int skillLevel, int level)
{
	CTL;
	bool ret = false;

	SkillPchId pch(skillId, skillLevel);

	vector<SkillAcquireInfoEx>& skills = m_ClassSkills[classId];
	for(UINT n=0;n<skills.size();n++)
	{
		SkillAcquireInfoEx &sa = skills[n];
		if(sa.skill.pchId == pch.pchId)
		{
			if(level + 9 >= sa.requiredLevel)
			{
				ret = true;
			}
			break;
		}
	}

	return ret;
}

void CSkillAcquireDbEx::LoadSkillPch()
{
	CTL;

	LPBYTE lpData = 0;
	UINT size = ReadFileBinary("..\\Script\\Skill_Pch.txt", lpData);
	if(size > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		wstring line;
		while(getline(file, line))
		{
			if(line.find(L"\\") == 0)
			{
				continue;
			}
			line = Parser::Replace(line, L'=', L' ');
			wstringstream paramStream(line);
			wstring name;
			SkillPchId id;
			if(paramStream >> name >> id.pchId)
			{
				m_SkillPch[name] = id;
			}
		}
	}
	if(lpData)
	{
		delete [] lpData;
	}

}

void CSkillAcquireDbEx::LoadData()
{
	CTL;

	LoadSkillPch();
	m_AcquirableSkills.clear();
	m_ClassSkills.clear();
	LPBYTE lpData = 0;
	UINT size = ReadFileBinary("..\\Script\\SkillAcquire.txt", lpData);
	if(size > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		wstring line;
		wstring currentClassName;
		UINT currentClass = UINT_MAX;
		while(getline(file, line))
		{
			wstring tempLine = Parser::Trim(line);
			if(line.find(L"\\") == 0 || line.find(L"//") == 0)
			{
				continue;
			}

			if(tempLine.find(L"skill_begin") == 0)
			{
				if(currentClass != UINT_MAX)
				{
					SkillAcquireInfoEx sa;
					wstring name = Parser::ParseOptionString(line, L"skill_name");
					sa.requiredLevel = Parser::ParseOptionInt(line, L"get_lv");
					if(GetSkillIdByName(name, sa.skill))
					{
						//update shared skills
						if(currentClassName == L"bishop_sharing" || currentClassName == L"elder_sharing" || currentClassName == L"silen_elder_sharing")
						{
							m_SharedSkills[currentClass].push_back(sa);
						}else
						{
							m_ClassSkills[currentClass].push_back(sa);
						}

						//update m_AcquirableSkills
						bool exist = false;
						for(UINT n=0;n<m_AcquirableSkills.size();n++)
						{
							if(m_AcquirableSkills[n].pchId == sa.skill.pchId)
							{
								exist = true;
								break;
							}
						}
						if(!exist)
						{
							m_AcquirableSkills.push_back(sa.skill);
						}

						bool addMaxSkill = true;
						//check max level
						for(UINT n=0;n<m_MaxAcquirableLevel.size();n++)
						{
							if(m_MaxAcquirableLevel[n].skillId == sa.skill.skillId)
							{
								if(m_MaxAcquirableLevel[n].level <= sa.skill.level)
								{
									m_MaxAcquirableLevel[n].level = sa.skill.level;
									addMaxSkill = false;
									break;
								}
							}
						}
						if(addMaxSkill)
						{
							m_MaxAcquirableLevel.push_back(sa.skill);
						}
					}else
					{
						g_Log.Add(CLog::Error, "[%s] Cannot find skill_pch data for skill[%S]!", __FUNCTION__, name.c_str());
					}
				}
				//parse skill
			}else if(tempLine.find(L"_begin") != wstring::npos)
			{
				//parse new class
				currentClassName = tempLine.substr(0, tempLine.find(L"_begin"));
				currentClass = GetClassByName(currentClassName);
				if(currentClass == UINT_MAX)
				{
					if(currentClassName != L"fishing" && currentClassName != L"fishing_nondwarf" && currentClassName != L"pledge" && currentClassName != L"sub_pledge" && currentClassName != L"transform" && currentClassName != L"subjob" && currentClassName != L"collect")
					{
						g_Log.Add(CLog::Error, "[%s] Cannot find classId for name[%S]!", __FUNCTION__, currentClassName.c_str());
					}
				}
			}else if(tempLine.find(L"include_") == 0)
			{
				if(currentClass != UINT_MAX)
				{
					//add skills to new class
					wstring subName = tempLine.substr(8, tempLine.size() - 8);
					UINT subClassId = GetClassByName(subName);
					if(subClassId != UINT_MAX)
					{
						vector<SkillAcquireInfoEx>& subVector = m_ClassSkills[subClassId];
						vector<SkillAcquireInfoEx>& classVector = m_ClassSkills[currentClass];
						classVector.insert(classVector.end(), subVector.begin(), subVector.end());
					}else
					{
						g_Log.Add(CLog::Error, "[%s] Cannot find classId for sub class[%S]!", __FUNCTION__, subName.c_str());
					}
				}
			}else if(tempLine.find(L"_end") != wstring::npos)
			{
				//end current class
				currentClass = UINT_MAX;
			}
			
		}
	}
	if(lpData)
	{
		delete [] lpData;
	}

}

bool CSkillAcquireDbEx::IsSharedSkill(UINT classId, int skillId, int skillLevel)
{
	CTL;
	bool ret = false;

	
	map<UINT, vector<SkillAcquireInfoEx>>::iterator it = m_SharedSkills.find(classId);
	if(it!=m_SharedSkills.end())
	{
		SkillPchId pch(skillId, skillLevel);
		vector<SkillAcquireInfoEx>& skills = it->second;
		for(UINT n=0;n<skills.size();n++)
		{
			if(pch.pchId == skills[n].skill.pchId)
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}