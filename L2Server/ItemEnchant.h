#pragma once


extern "C"
{
	void AttributeEnchantValueWeaponHook();
	WORD GetEnchantAttributeValueWrapper();
	void AttributeEnchantLevelWeaponCheckHook();

	void AttributeEnchantLevelArmorCheckHook();
	void AttributeEnchantValueArmorHook();
};

struct ItemEnchantRate
{
	UINT crystalType;
	UINT level;
	double armor;
	double armorBlessed;
	double armorAncient;
	double weapon;
	double weaponBlessed;
	double weaponAncient;
	double magicWeapon;
	double magicWeaponBlessed;
	double magicWeaponAncient;
};

struct ItemEnchantConfig
{
	UINT crystalType;
	UINT maxArmorLevel;
	UINT maxWeaponLevel;
	UINT maxMagicWeaponLevel;
	UINT maxSafeArmorLevel;
	UINT maxSafeWeaponLevel;
	UINT maxSafeMagicWeaponLevel;
};

class CItemEnchant : public CFeature
{
	BOOL enabled;
	void LoadINI();
	void LoadData();
	static double GetRateArmor(UINT crystalType, UINT level, UINT scrollEtcType);
	static double GetRateWeapon(UINT crystalType, UINT level, UINT scrollEtcType);
	static double GetRateMagicWeapon(UINT crystalType, UINT level, UINT scrollEtcType);
public:
	CItemEnchant();
	void Init();
	~CItemEnchant();
	bool CanEnchant(User *pUser, CItem *pItem, CItem *pScroll);
	static bool EnchantItem(CItem* pScroll, CItem* pItem, User* pUser, UINT changeType, double param);
	static bool EnchantAttributeItemWeapon(CItem *pScroll, CItem* pItem, User *pUser, UINT attributeTypeEnum, INT32 levelLimit);
	static double EnchantAttributeItemWeaponDice();
	static bool EnchantAttributeItemArmor(CItem *pScroll, CItem* pItem, User *pUser, UINT attributeTypeEnum, INT32 levelLimit);
	static double EnchantAttributeItemArmorDice();
	static void SetSafeAttribute(UINT value);
	static UINT GetSafeAttribute();
	static void SendEnchantSucceededPacket(User *pUser);
};

extern CItemEnchant g_ItemEnchant;