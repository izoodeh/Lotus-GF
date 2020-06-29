#include "StdAfx.h"
#include "ParserForSkill.h"
#include "HookManager.h"
#include "PumpEffect.h"
#include "InstantEffect.h"

CParserForSkill g_ParserForSkill;

struct AbnormalTypeData
{
	const WCHAR* wName;
	UINT64 id; 
};

AbnormalTypeData g_AbnormalTypeArray[512];
UINT64 g_TotalAbnormalTypes = 0;

CParserForSkill::CParserForSkill() : requestChangeType(false), dataType(DATA_NONE)
{

}

CParserForSkill::~CParserForSkill()
{

}

void CParserForSkill::AddEffect(wstring name, DATA_TYPE type, BASE_DATA_TYPE baseType)
{
	mName.insert(pair<wstring, DATA_TYPE>(name, type));
	mDataType.insert(pair<DATA_TYPE, BASE_DATA_TYPE>(type, baseType));
}

void CParserForSkill::Init()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//AbnormalType.txt", lpData);
	if(lpData && loaded > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		vector<AbnormalTypeData> vAbnormals;
		wstring line;
		while(getline(file, line))
		{
			if(line.find(L"at_begin") == 0)
			{
				wstring name = ParseOptionString(line, L"name");
				UINT64 id = ParseOptionInt(line, L"id");
				if(name.size() > 0 && id > 0)
				{
					WCHAR* lpName = new WCHAR[64];
					memset(lpName, 0, 128);
					wcscpy(lpName, name.c_str());
					AbnormalTypeData atd = { lpName, id };
					vAbnormals.push_back(atd);
				}
			}
		}

		if(vAbnormals.size() > 0)
		{
			g_TotalAbnormalTypes = 336 + vAbnormals.size();
			memset(g_AbnormalTypeArray, 0, sizeof(g_AbnormalTypeArray));

			//copy existing abnormals
			AbnormalTypeData* lpAbnormalType = (AbnormalTypeData*)0xE64CF0L;
			for(UINT n=0;n<336;n++)
			{
				g_AbnormalTypeArray[n].id = lpAbnormalType[n].id;
				g_AbnormalTypeArray[n].wName = lpAbnormalType[n].wName;
			}
			for(UINT n=0;n<vAbnormals.size();n++)
			{
				g_AbnormalTypeArray[336+n].id = vAbnormals[n].id;
				g_AbnormalTypeArray[336+n].wName = vAbnormals[n].wName;
			}

			WriteRelative(0x9B3772, &g_AbnormalTypeArray[0].id);
			WriteRelative(0x9B37D6, &g_AbnormalTypeArray[g_TotalAbnormalTypes].id);
		}
		delete [] lpData;
	}

	WriteCall(0x991462, CParserForSkill::GetString);
	WriteToVMT(0xCBF9D8, 0x48, CParserForSkill::ChangeToken);
	
	WriteCall(0x9BABCF, CParserForSkill::CreatePumpEffect);
	WriteNOP(0x9BABE2, 3);
	WriteCall(0x9A895A, CParserForSkill::CreatePumpEffect);
	WriteNOP(0x9A896D, 3);
	WriteCall(0x9B8DCC, CParserForSkill::CreateInstantEffect);
	WriteNOP(0x9B8DDF, 3);
	WriteCall(0x9B87BD, CParserForSkill::CreateInstantEffectOneParam);
	WriteNOP(0x9B87E0, 3);

	WriteCall(0x9A9BA3, CParserForSkill::CreatePumpEffectOneParam);
	WriteNOP(0x9A9BC4, 3);
	WriteCall(0x9BBF6A, CParserForSkill::CreatePumpEffectOneParam);
	WriteNOP(0x9BBF8B, 3);

	WriteCall(0x9A8A4C, CParserForSkill::CreateDefenceTrait);
	WriteCall(0x9BAB52, CParserForSkill::CreateDefenceTrait);

	WriteCall(0x9A8AC9, CParserForSkill::AddAttackTrait);
	WriteCall(0x9BAACC, CParserForSkill::AddAttackTrait);

	WriteJump(0x9B3E93, (UINT64)TraitExtend);

	AddEffect(L"p_block_exp", DATA_P_BLOCK_EXP, BASE_P_BLOCK_ACT);
	AddEffect(L"p_block_buff_ex", DATA_P_BLOCK_BUFF_EX, BASE_P_BLOCK_ACT);
	AddEffect(L"i_ride_wyvern", DATA_I_RIDE_WYVERN, BASE_I_INSTALL_CAMP);
	AddEffect(L"p_bonus_exp", DATA_P_BONUS_EXP, BASE_P_VAMPIRIC_ATTACK);
	AddEffect(L"p_bonus_sp", DATA_P_BONUS_SP, BASE_P_VAMPIRIC_ATTACK);
	AddEffect(L"p_bonus_drop", DATA_P_BONUS_DROP, BASE_P_VAMPIRIC_ATTACK);
	AddEffect(L"p_bonus_spoil", DATA_P_BONUS_SPOIL, BASE_P_VAMPIRIC_ATTACK);

	AddEffect(L"i_attack_me", DATA_I_ATTACK_ME, BASE_I_M_ATTACK);

	AddEffect(L"trait_water", DATA_TRAIT_WATER, BASE_TRAIT_BOSS);
	AddEffect(L"trait_fire", DATA_TRAIT_FIRE, BASE_TRAIT_BOSS);
	AddEffect(L"trait_wind", DATA_TRAIT_WIND, BASE_TRAIT_BOSS);
	AddEffect(L"trait_earth", DATA_TRAIT_EARTH, BASE_TRAIT_BOSS);
	AddEffect(L"trait_holy", DATA_TRAIT_HOLY, BASE_TRAIT_BOSS);
	AddEffect(L"trait_unholy", DATA_TRAIT_UNHOLY, BASE_TRAIT_BOSS);

}

UINT CParserForSkill::ChangeToken(CLexerForSkill* lpInstance)
{
	typedef UINT (*f)(CLexerForSkill*);
	UINT type = f(0x98FA34L)(lpInstance);

	if(g_ParserForSkill.requestChangeType)
	{
		g_ParserForSkill.requestChangeType = false;
		if(g_ParserForSkill.dataType > 0)
		{
			map<DATA_TYPE, BASE_DATA_TYPE>::iterator Iter = g_ParserForSkill.mDataType.find(g_ParserForSkill.dataType);
			if(Iter!=g_ParserForSkill.mDataType.end())
			{
				return Iter->second;
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Unknown data type[%d]!", __FUNCTION__);
			}
		}
	}

	return type;
}

const WCHAR* CParserForSkill::GetString(LPVOID param)
{
	typedef const WCHAR* (*f)(LPVOID param);
	const WCHAR* wStr = f(0x9E4210L)(param);

	map<wstring, DATA_TYPE>::iterator Iter = g_ParserForSkill.mName.find(wStr);
	if(Iter!=g_ParserForSkill.mName.end())
	{
		g_ParserForSkill.requestChangeType = true;
		g_ParserForSkill.dataType = Iter->second;
		return wStr;
	}

	return wStr;
}

CSkillEffect* CParserForSkill::CreatePumpEffect(UINT64 size)
{
	if(g_ParserForSkill.dataType == DATA_P_BLOCK_EXP)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_p_block_exp();
		return pEffect;
	}else if(g_ParserForSkill.dataType == DATA_P_BLOCK_EXP)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_p_block_buff_ex();
		return pEffect;
	}else
	{
		
		LPDWORD lpData = (LPDWORD) new char[size];
		memset(lpData, 0, size);
		(*lpData) = 0xCEB088;
		return (CSkillEffect*)lpData;
	}
}

CSkillEffect* CParserForSkill::CreatePumpEffectOneParam(UINT64 size)
{
	if(g_ParserForSkill.dataType == DATA_P_BONUS_EXP)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_p_bonus_exp();
		return pEffect;
	}else if(g_ParserForSkill.dataType == DATA_P_BONUS_SP)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_p_bonus_sp();
		return pEffect;
	}else if(g_ParserForSkill.dataType == DATA_P_BONUS_DROP)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_p_bonus_drop();
		return pEffect;
	}else if(g_ParserForSkill.dataType == DATA_P_BONUS_SPOIL)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_p_bonus_spoil();
		return pEffect;
	}else
	{
		
		LPDWORD lpData = (LPDWORD) new char[size];
		memset(lpData, 0, size);
		(*lpData) = 0xCEA178;
		return (CSkillEffect*)lpData;
	}
}


CSkillEffect* CParserForSkill::CreateInstantEffect(UINT64 size)
{
	if(g_ParserForSkill.dataType == DATA_I_RIDE_WYVERN)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect *pEffect = new CSkillEffect_i_ride_wyvern();
		return pEffect;
	}else
	{
		
		LPDWORD lpData = (LPDWORD) new char[size];
		memset(lpData, 0, size);
		(*lpData) = 0xCF7608;
		return (CSkillEffect*)lpData;
	}
}

CSkillEffect* CParserForSkill::CreateInstantEffectOneParam(UINT64 size)
{
	if(g_ParserForSkill.dataType == DATA_I_ATTACK_ME)
	{
		g_ParserForSkill.dataType = DATA_NONE;
		CSkillEffect_i_attack_me *pEffect = new CSkillEffect_i_attack_me;
		return (CSkillEffect*)pEffect;
	}else
	{
		
		LPDWORD lpData = (LPDWORD) new char[size];
		memset(lpData, 0, size);
		(*lpData) = 0xCF6B78;
		return (CSkillEffect*)lpData;
	}
}

CSkillEffect_p_defence_trait* CParserForSkill::CreateDefenceTrait(CSkillEffect_p_defence_trait *pEffect, UINT traitType, double value)
{
	typedef CSkillEffect_p_defence_trait* (*f)(CSkillEffect_p_defence_trait*, UINT, double);
	pEffect = f(0x83BBC8L)(pEffect, traitType, value);

	if(g_ParserForSkill.dataType == DATA_TRAIT_WATER)
	{
		pEffect->m_traitType = TRAIT_WATER;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_FIRE)
	{
		pEffect->m_traitType = TRAIT_FIRE;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_WIND)
	{
		pEffect->m_traitType = TRAIT_WIND;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_EARTH)
	{
		pEffect->m_traitType = TRAIT_EARTH;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_HOLY)
	{
		pEffect->m_traitType = TRAIT_HOLY;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_UNHOLY)
	{
		pEffect->m_traitType = TRAIT_UNHOLY;
		g_ParserForSkill.dataType = DATA_NONE;
	}

	return pEffect;
}


void CParserForSkill::AddAttackTrait(LPVOID lpContainer, CSkillEffect_p_attack_trait **lpEffect)
{
	typedef void(*f)(LPVOID, CSkillEffect_p_attack_trait**);

	CSkillEffect_p_attack_trait* pEffect = (*lpEffect);
	if(g_ParserForSkill.dataType == DATA_TRAIT_WATER)
	{
		pEffect->m_traitType = TRAIT_WATER;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_FIRE)
	{
		pEffect->m_traitType = TRAIT_FIRE;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_WIND)
	{
		pEffect->m_traitType = TRAIT_WIND;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_EARTH)
	{
		pEffect->m_traitType = TRAIT_EARTH;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_HOLY)
	{
		pEffect->m_traitType = TRAIT_HOLY;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_UNHOLY)
	{
		pEffect->m_traitType = TRAIT_UNHOLY;
		g_ParserForSkill.dataType = DATA_NONE;
	}

	f(0x9A31D0L)(lpContainer, lpEffect);
}

void TraitExtendWrapper(CSkillInfo *pSI)
{
	CParserForSkill::TraitSkillInfo(pSI);
}

void CParserForSkill::TraitSkillInfo(CSkillInfo *pSI)
{
	if(g_ParserForSkill.dataType == DATA_TRAIT_WATER)
	{
		pSI->traitType = TRAIT_WATER;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_FIRE)
	{
		pSI->traitType = TRAIT_FIRE;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_WIND)
	{
		pSI->traitType = TRAIT_WIND;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_EARTH)
	{
		pSI->traitType = TRAIT_EARTH;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_HOLY)
	{
		pSI->traitType = TRAIT_HOLY;
		g_ParserForSkill.dataType = DATA_NONE;
	}else if(g_ParserForSkill.dataType == DATA_TRAIT_UNHOLY)
	{
		pSI->traitType = TRAIT_UNHOLY;
		g_ParserForSkill.dataType = DATA_NONE;
	}
}