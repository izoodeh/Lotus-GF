#pragma once

//148h size
class CItemInfo
{
public:
	static CItemInfo* Get(CSharedItemData *pSII);
	/* 00 */ UINT _unkn00[2];
	/* 08 */ UINT etcItemType;
	/* 0C */ UINT _unkn0C[30];
	/* 84 */ bool magicWeapon;
	/* 85 */ BYTE _align85[3];
	/* 88 */ UINT _unkn88[2];
	/* 90 */ UINT weaponType;
	/* 94 */ UINT _unkn94[29];
	/* 108 */ INT32 periodItemTime;	//herbs etc
	/* 10C */ UINT _unkn10C[14];
	/* 144 */ UINT crystalType;
};

//VT B61DB8h
class CItem : public CWorldObject
{
public:
	/* 48 */ CSharedItemData *pSID;
	/* 50 */ CItemInfo *pII;
	/* 58 */ CYieldLock* lpLock;

	/* 60 */ UINT _unkn60[4];
	/* 70 */ bool _unkn70;
	/* 71 */ bool _align71[3];
	/* 74 */ UINT _unkn74[1];
	/* 78 */ vector<CSkillInfo*> vItemSkill;
	/* 98 */ LPVOID _unkn98;
	/* A0 */ LPVOID _unknA0[3];
	/* B8 */ LPVOID _unknB8[9];
	/* 100 */ LPVOID _unkn100[6];

	/* 130 */ BaseAttribute attribute;

	bool ValidItem();
	bool IsWeapon();
	BaseAttribute* GetTotalAttribute(BaseAttribute * lpBaseAttribute);
};