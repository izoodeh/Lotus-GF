#pragma once

struct SchemeBuffData
{
	INT32 skillId;
	INT32 level;
	ItemReq price;
	INT32 minLevel;
	INT32 maxLevel;
	INT32 customTime;
};

class CSchemeBuffer : public CFeature
{
	INT32 toLowLevelMsg;
	INT32 toHighLevelMsg;
	INT32 notEnoughAdenaMsg;
	INT32 addToListMsg;
	INT32 addToSummonListMsg;
	BOOL overrideListPrice;
	ItemReq listPrice;
	BOOL allowOlympiad;
	BOOL allowPk;
	BOOL allowCombat;
	BOOL allowGuilty;
	BOOL enabled;
	BOOL validateTalkTarget;
	BOOL htmlLinkCheck;
	UINT maxListSize;
	map<SkillPchId, SchemeBuffData> mBuffs;
public:
	CSchemeBuffer();
	~CSchemeBuffer();
	void Init();
	void LoadData();
	void BuffPlayer(User *pUser, int skillClassId, bool byList = false);
	void BuffSummon(User *pUser, int skillClassId, bool byList = false);
	void AddBuff(User *pUser, int skillClassId, bool summon = false);
	void UseList(User *pUser, bool summon = false);
	UINT GetMaxListSize();
	BOOL IsEnabled();
	BOOL IsValidateTalkTarget();
	inline BOOL IsAllowCombat() { return allowCombat; };
	inline BOOL IsAllowGuilty() { return allowGuilty; };
	inline BOOL IsAllowPk() { return allowPk; };
	inline BOOL IsHtmlLinkCheck() { return htmlLinkCheck; };
};

extern CSchemeBuffer g_SchemeBuffer;