#include "StdAfx.h"
#include "ItemEnchant.h"
#include "HookManager.h"
#include "DB.h"

CItemEnchant g_ItemEnchant;

ItemEnchantConfig g_ItemEnchantConfig[CRYSTAL_TYPE_COUNT];
ItemEnchantRate g_ItemEnchantRate[CRYSTAL_TYPE_COUNT][30];

UINT g_SafeAttributeValue[32]; // per thread index


double g_ArmorEnchantRate[30];
double g_NormalWeaponEnchantRate[30];
double g_MagicWeaponEnchantRate[30];
UINT g_MaxArmorEnchant = 20;
UINT g_MaxNormalWeaponEnchant = 30;
UINT g_MaxMagicWeaponEnchant = 30;

CItemEnchant::CItemEnchant() : enabled(FALSE)
{
	SetName(L"Item Enchant");
}

CItemEnchant::~CItemEnchant()
{
}

void CItemEnchant::Init()
{
	memset(g_SafeAttributeValue, 0, sizeof(g_SafeAttributeValue));

	//Init base rates
	for(int n=0;n<CRYSTAL_TYPE_COUNT;n++)
	{
		g_ItemEnchantConfig[n].crystalType = n;
		g_ItemEnchantConfig[n].maxArmorLevel = 20;
		g_ItemEnchantConfig[n].maxWeaponLevel = 30;
		g_ItemEnchantConfig[n].maxMagicWeaponLevel = 30;
		g_ItemEnchantConfig[n].maxSafeArmorLevel = 10;
		g_ItemEnchantConfig[n].maxSafeWeaponLevel = 10;
		g_ItemEnchantConfig[n].maxSafeMagicWeaponLevel = 10;

		for(int m=0;m<30;m++)
		{
			g_ItemEnchantRate[n][m].crystalType = n;
			g_ItemEnchantRate[n][m].level = m;
			if(m < 3)
			{
				g_ItemEnchantRate[n][m].armor = 100.0;
				g_ItemEnchantRate[n][m].armorBlessed = 100.0;
				g_ItemEnchantRate[n][m].armorAncient = 100.0;
				g_ItemEnchantRate[n][m].weapon = 100.0;
				g_ItemEnchantRate[n][m].weaponBlessed = 100.0;
				g_ItemEnchantRate[n][m].weaponAncient = 100.0;
				g_ItemEnchantRate[n][m].magicWeapon = 100.0;
				g_ItemEnchantRate[n][m].magicWeaponBlessed = 100.0;
				g_ItemEnchantRate[n][m].magicWeaponAncient = 100.0;
			}else
			{
				g_ItemEnchantRate[n][m].armor = 66.666666;
				g_ItemEnchantRate[n][m].armorBlessed = 66.666666;
				g_ItemEnchantRate[n][m].armorAncient = 66.666666;
				g_ItemEnchantRate[n][m].weapon = 66.666666;
				g_ItemEnchantRate[n][m].weaponBlessed = 66.666666;
				g_ItemEnchantRate[n][m].weaponAncient = 66.666666;
				g_ItemEnchantRate[n][m].magicWeapon = 66.666666;
				g_ItemEnchantRate[n][m].magicWeaponBlessed = 66.666666;
				g_ItemEnchantRate[n][m].magicWeaponAncient = 66.666666;
			}
		}
	}
	LoadData();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled", GetName());
		WriteCall(0x834022, CItemEnchant::EnchantItem);
		WriteCall(0x83412A, CItemEnchant::EnchantItem);

		WriteCall(0x83431E, CItemEnchant::EnchantAttributeItemWeapon);
		WriteCall(0x696672, CItemEnchant::EnchantAttributeItemWeaponDice);
		WriteJump(0x69678D, (UINT64)AttributeEnchantValueWeaponHook, 7);
		WriteJump(0x8342B9, (UINT64)AttributeEnchantLevelWeaponCheckHook, 2);

		WriteCall(0x834404, CItemEnchant::EnchantAttributeItemArmor);
		WriteCall(0x6960B1, CItemEnchant::EnchantAttributeItemArmorDice);
		WriteJump(0x8343A4, (UINT64)AttributeEnchantLevelArmorCheckHook, 2);
		WriteJump(0x696147, (UINT64)AttributeEnchantValueArmorHook, 5);
		WriteCall(0x8E5ADD, CItemEnchant::SendEnchantSucceededPacket, 37);
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
	}
}

void CItemEnchant::SendEnchantSucceededPacket(User *pUser)
{
	guard;

	if(pUser->pEUD->isCrystalEnchantFail)
	{
		pUser->pUserSocket->Send("cddQ", 0x87, 3, pUser->pEUD->isCrystalEnchantFail, (UINT64)1);
		pUser->pEUD->isCrystalEnchantFail = 0;
	}else
	{
		pUser->pUserSocket->Send("cddQ", 0x87, 0, 0, 0);
	}

	unguard;
}

void CItemEnchant::LoadData()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//ItemEnchantData.txt", lpData);
	if(lpData && loaded > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		wstring line;
		int lineCount = 0;
		while(getline(file, line))
		{
			lineCount++;
			if(line.size() > 4)
			{
				if( line[0] == L'/' || line[0] == L';' )
					continue;
				if( line.find(L"global_begin") == 0 )
				{
					enabled = ParseOptionInt(line, L"enabled", 0);
					continue;
				}
				if( line.find(L"setting_begin") == 0 )
				{
					wstring wGrade = ParseOptionString(line, L"grade", L"none");
					UINT grade = 0;
					if(wGrade == L"d")
						grade = CRYSTAL_D;
					else if(wGrade == L"c")
						grade = CRYSTAL_C;
					else if(wGrade == L"b")
						grade = CRYSTAL_B;
					else if(wGrade == L"a")
						grade = CRYSTAL_A;
					else if(wGrade == L"s")
						grade = CRYSTAL_S;
					else if(wGrade == L"s80")
						grade = CRYSTAL_S80;
					else if(wGrade == L"s84")
						grade = CRYSTAL_S84;
					else
					{
						g_Log.Add(CLog::Error, "[%s] Invalid item grade[%S] line[%d]!", __FUNCTION__, wGrade.c_str(), lineCount);
						continue;
					}

					//setting_begin grade=d	max_armor_level=20	max_weapon_level=30	max_magic_weapon_level=30 max_safe_weapon_level=10	max_safe_magic_weapon_level=10	max_safe_armor_level=10	setting_end
					g_ItemEnchantConfig[grade].maxArmorLevel = ParseOptionInt(line, L"max_armor_level", 20);
					g_ItemEnchantConfig[grade].maxWeaponLevel =  ParseOptionInt(line, L"max_weapon_level", 30);
					g_ItemEnchantConfig[grade].maxMagicWeaponLevel =  ParseOptionInt(line, L"max_magic_weapon_level", 30);
					g_ItemEnchantConfig[grade].maxSafeArmorLevel =  ParseOptionInt(line, L"max_safe_armor_level", 10);
					g_ItemEnchantConfig[grade].maxSafeWeaponLevel =  ParseOptionInt(line, L"max_safe_weapon_level", 10);
					g_ItemEnchantConfig[grade].maxSafeMagicWeaponLevel =  ParseOptionInt(line, L"max_safe_magic_weapon_level", 10);

					continue;
				}
				if( line.find(L"rate_begin") == 0 )
				{
					//rate_begin grade=d	level=3	armor=66	armor_blessed=66	armor_ancient=66	weapon=66	magic_weapon=40	weapon_blessed=66	weapon_ancient=66	magic_weapon_blessed=66	magic_weapon_ancient=66	rate_end
					wstring wGrade = ParseOptionString(line, L"grade", L"none");
					UINT grade = 0;
					if(wGrade == L"d")
						grade = CRYSTAL_D;
					else if(wGrade == L"c")
						grade = CRYSTAL_C;
					else if(wGrade == L"b")
						grade = CRYSTAL_B;
					else if(wGrade == L"a")
						grade = CRYSTAL_A;
					else if(wGrade == L"s")
						grade = CRYSTAL_S;
					else if(wGrade == L"s80")
						grade = CRYSTAL_S80;
					else if(wGrade == L"s84")
						grade = CRYSTAL_S84;
					else
					{
						g_Log.Add(CLog::Error, "[%s] Invalid item grade[%S] line[%d]!", __FUNCTION__, wGrade.c_str(), lineCount);
						continue;
					}

					UINT level = ParseOptionInt(line, L"level", -1);
					if(level < 30)
					{
						g_ItemEnchantRate[grade][level].armor = (double)ParseOptionInt(line, L"armor", 66);
						g_ItemEnchantRate[grade][level].armorBlessed = (double)ParseOptionInt(line, L"armor_blessed", 66);
						g_ItemEnchantRate[grade][level].armorAncient = (double)ParseOptionInt(line, L"armor_ancient", 66);

						g_ItemEnchantRate[grade][level].weapon = (double)ParseOptionInt(line, L"weapon", 66);
						g_ItemEnchantRate[grade][level].weaponBlessed = (double)ParseOptionInt(line, L"weapon_blessed", 66);
						g_ItemEnchantRate[grade][level].weaponAncient = (double)ParseOptionInt(line, L"weapon_ancient", 66);

						g_ItemEnchantRate[grade][level].magicWeapon = (double)ParseOptionInt(line, L"magic_weapon", 66);
						g_ItemEnchantRate[grade][level].magicWeaponBlessed = (double)ParseOptionInt(line, L"magic_weapon_blessed", 66);
						g_ItemEnchantRate[grade][level].magicWeaponAncient = (double)ParseOptionInt(line, L"magic_weapon_ancient", 66);
					}else
					{
						g_Log.Add(CLog::Error, "[%s] Invalid item level[%d](max 29) line[%d]!", __FUNCTION__, level, lineCount);
						continue;
					}
				}
			}
		}
		delete [] lpData;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//ItemEnchantData.txt !", GetName());
	}
}

void CItemEnchant::LoadINI()
{
	const TCHAR* sectionName = _T("ItemEnchantSystem");
	enabled = GetPrivateProfileInt(sectionName, _T("Enabled"), 0, g_ConfigFile);
	g_MaxArmorEnchant = GetPrivateProfileInt(sectionName, _T("MaxArmorEnchant"), 20, g_ConfigFile);
	g_MaxNormalWeaponEnchant = GetPrivateProfileInt(sectionName, _T("MaxNormalWeaponEnchant"), 30, g_ConfigFile);
	g_MaxMagicWeaponEnchant = GetPrivateProfileInt(sectionName, _T("maxMagicWeaponEnchant"), 30, g_ConfigFile);

	for(int n=0;n<30;n++)
	{
		if(n > 2)
		{
			{
				wstringstream skey;
				skey << L"MagicWeaponEnchantRate" << n ;
				g_MagicWeaponEnchantRate[n] = GetPrivateProfileDouble(sectionName, skey.str().c_str(), 40.0, g_ConfigFile);
			}
			{
				wstringstream skey;
				skey << L"NormalWeaponEnchantRate" << n ;
				g_NormalWeaponEnchantRate[n] = GetPrivateProfileDouble(sectionName, skey.str().c_str(), 40.0, g_ConfigFile);
			}
			{
				wstringstream skey;
				skey << L"ArmorEnchantRate" << n ;
				g_ArmorEnchantRate[n] = GetPrivateProfileDouble(sectionName, skey.str().c_str(), 40.0, g_ConfigFile);
			}
		}else
		{
			g_MagicWeaponEnchantRate[n] = 100;
			g_NormalWeaponEnchantRate[n] = 100;
			g_ArmorEnchantRate[n] = 100;
		}
	}
}

double CItemEnchant::GetRateArmor(UINT crystalType, UINT level, UINT scrollEtcType)
{
	if(crystalType < CRYSTAL_TYPE_COUNT)
	{
		if(level >= 30)
			level = 29;

		if(scrollEtcType == ETCITEM_SCRL_ENCHANT_AM)
			return g_ItemEnchantRate[crystalType][level].armor;
		else if(scrollEtcType == ETCITEM_BLESS_SCRL_ENCHANT_AM)
			return g_ItemEnchantRate[crystalType][level].armorBlessed;
		else if(scrollEtcType == ETCITEM_ANCIENT_CRYSTAL_ENCHANT_AM)
			return g_ItemEnchantRate[crystalType][level].armorAncient;
	}

	return 66.666666;
}

double CItemEnchant::GetRateWeapon(UINT crystalType, UINT level, UINT scrollEtcType)
{
	if(crystalType < CRYSTAL_TYPE_COUNT)
	{
		if(level >= 30)
			level = 29;

		if(scrollEtcType == ETCITEM_SCRL_ENCHANT_WP)
			return g_ItemEnchantRate[crystalType][level].weapon;
		else if(scrollEtcType == ETCITEM_BLESS_SCRL_ENCHANT_WP)
			return g_ItemEnchantRate[crystalType][level].weaponBlessed;
		else if(scrollEtcType == ETCITEM_ANCIENT_CRYSTAL_ENCHANT_WP)
			return g_ItemEnchantRate[crystalType][level].weaponAncient;
	}

	return 66.666666;
}

double CItemEnchant::GetRateMagicWeapon(UINT crystalType, UINT level, UINT scrollEtcType)
{
	if(crystalType < CRYSTAL_TYPE_COUNT)
	{
		if(level >= 30)
			level = 29;

		if(scrollEtcType == ETCITEM_SCRL_ENCHANT_WP)
			return g_ItemEnchantRate[crystalType][level].magicWeapon;
		else if(scrollEtcType == ETCITEM_BLESS_SCRL_ENCHANT_WP)
			return g_ItemEnchantRate[crystalType][level].magicWeaponBlessed;
		else if(scrollEtcType == ETCITEM_ANCIENT_CRYSTAL_ENCHANT_WP)
			return g_ItemEnchantRate[crystalType][level].magicWeaponAncient;
	}

	return 66.666666;
}

bool CItemEnchant::EnchantItem(CItem *pScroll, CItem *pItem, User *pUser, UINT changeType, double param)
{
	guard;
	if(pItem)
	{
		UINT scrollType = pScroll->pII->etcItemType;
		if( ETCITEM_SCRL_SAFE_ENCHANT_WP == scrollType ||
			ETCITEM_SCRL_SAFE_ENCHANT_AM == scrollType ||
			ETCITEM_ANCIENT_CRYSTAL_ENCHANT_WP == scrollType ||
			ETCITEM_ANCIENT_CRYSTAL_ENCHANT_AM == scrollType ||
			ETCITEM_CRYSTAL_ENCHANT_WP == scrollType ||
			ETCITEM_CRYSTAL_ENCHANT_AM == scrollType ||
			ETCITEM_BLESS_SCRL_ENCHANT_WP == scrollType ||
			ETCITEM_BLESS_SCRL_ENCHANT_AM == scrollType ||
			ETCITEM_SCRL_ENCHANT_WP == scrollType ||
			ETCITEM_SCRL_ENCHANT_AM == scrollType )
		{
			UINT enchantLevel = pItem->pSID->enchantLevel;
			UINT slotType = pItem->pSID->slotType;
			double bonus = 0.0;
			CItem *pBonusItem = pUser->GetPendingUseETCItem(PUI_ENCHANT_BONUS);
			if(pBonusItem)
			{
				bonus = pUser->enchantRateBonus;
			}

			bonus += 100.0;
			bonus += param;
			bonus /= 100.0;

			double chance = 40.0;
			UINT crystalType = pItem->pII->crystalType;
			if( crystalType > 0  && crystalType < CRYSTAL_TYPE_COUNT)
			{
				if(pItem->IsWeapon())
				{
					if(pItem->pII->magicWeapon)
					{
						if(enchantLevel < g_ItemEnchantConfig[crystalType].maxMagicWeaponLevel)
						{
							if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_WP)
							{
								if(enchantLevel < g_ItemEnchantConfig[crystalType].maxSafeMagicWeaponLevel)
								{
									g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
									unguard;
									return true;
								}							
								else
								{
									pUser->EnchantCancel();
									pUser->pUserSocket->SendSystemMessage(L"You cannot enchant this item with safe enchant scroll anymore.");
									unguard;
									return false;
								}
							}
							if(enchantLevel < 3)
							{
								g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
								unguard;
								return true;
							}else
							{
								chance = GetRateMagicWeapon(crystalType, enchantLevel, pScroll->pII->etcItemType);
								chance *= bonus;

								double dice = RandDoubleEx(100.0);

							//	g_Log.Add(CLog::Blue, "[%s] dice[%f] chance[%f] WM", __FUNCTION__, dice, chance);

								if(dice <= chance)
								{
									g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
									unguard;
									return true;
								}else
								{
									UINT etcItemType = pScroll->pII->etcItemType;
									if(etcItemType == ETCITEM_BLESS_SCRL_ENCHANT_WP)
									{
										g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, 0, pUser, (INT32)chance);
										unguard;
										return true;
									}else if(etcItemType == ETCITEM_ANCIENT_CRYSTAL_ENCHANT_WP)
									{
										g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, enchantLevel, pUser, (INT32)chance);
										pUser->pEUD->isCrystalEnchantFail = pItem->pSID->itemId;
										unguard;
										return true;
									}else
									{
										g_DB.RequestEnchantItemFail(pScroll, pBonusItem, pItem, pUser, (INT32)chance);
										unguard;
										return false;
									}
								}
							}
						}else
						{
							pUser->EnchantCancel();
							pUser->pUserSocket->SendSystemMessage(L"This item already got max enchant level.");
							unguard;
							return false;
						}
					}else
					{
						if(enchantLevel < g_ItemEnchantConfig[crystalType].maxWeaponLevel)
						{
							if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_WP)
							{
								if(enchantLevel < g_ItemEnchantConfig[crystalType].maxSafeWeaponLevel)
								{
									g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
									unguard;
									return true;
								}							
								else
								{
									pUser->EnchantCancel();
									pUser->pUserSocket->SendSystemMessage(L"You cannot enchant this item with safe enchant scroll anymore.");
									unguard;
									return false;
								}
							}
							if(enchantLevel < 3)
							{
								g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
								unguard;
								return true;
							}else
							{
								chance = GetRateWeapon(crystalType, enchantLevel, pScroll->pII->etcItemType);

								chance *= bonus;

								double dice = RandDoubleEx(100.0);
						//		g_Log.Add(CLog::Blue, "[%s] dice[%f] chance[%f] W", __FUNCTION__, dice, chance);
								if(dice <= chance)
								{
									g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
									unguard;
									return true;
								}else
								{
									UINT etcItemType = pScroll->pII->etcItemType;
									if(etcItemType == ETCITEM_BLESS_SCRL_ENCHANT_WP)
									{
										g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, 0, pUser, (INT32)chance);
										unguard;
										return true;
									}else if(etcItemType == ETCITEM_ANCIENT_CRYSTAL_ENCHANT_WP)
									{
										g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, enchantLevel, pUser, (INT32)chance);
										pUser->pEUD->isCrystalEnchantFail = pItem->pSID->itemId;
										unguard;
										return true;
									}else
									{
										g_DB.RequestEnchantItemFail(pScroll, pBonusItem, pItem, pUser, (INT32)chance);
										unguard;
										return false;
									}
								}
							}
						}else
						{
							pUser->EnchantCancel();
							pUser->pUserSocket->SendSystemMessage(L"This item already got max enchant level.");
							unguard;
							return false;
						}
					}
				}else
				{
					if(enchantLevel < g_ItemEnchantConfig[crystalType].maxArmorLevel)
					{
						if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_AM)
						{
							if(enchantLevel < g_ItemEnchantConfig[crystalType].maxSafeArmorLevel)
							{
								g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
								unguard;
								return true;
							}							
							else
							{
								pUser->EnchantCancel();
								pUser->pUserSocket->SendSystemMessage(L"You cannot enchant this item with safe enchant scroll anymore.");
								unguard;
								return false;
							}
						}

						if(enchantLevel < 3 || ( slotType == SlotOnePiece && enchantLevel < 4))
						{
							g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
							unguard;
							return true;
						}else
						{
							if(slotType == SlotOnePiece)
							{
								chance = GetRateArmor(crystalType, (enchantLevel - 1), pScroll->pII->etcItemType);
							}else
							{
								chance = GetRateArmor(crystalType, enchantLevel, pScroll->pII->etcItemType);
							}

							chance *= bonus;

							double dice = RandDoubleEx(100.0);

						//	g_Log.Add(CLog::Blue, "[%s] dice[%f] chance[%f] A", __FUNCTION__, dice, chance);
							if(dice <= chance)
							{
								g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, (enchantLevel+1), pUser, (INT32)chance);
								unguard;
								return true;
							}else
							{
								UINT etcItemType = pScroll->pII->etcItemType;
								if(etcItemType == ETCITEM_BLESS_SCRL_ENCHANT_AM)
								{
									g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, 0, pUser, (INT32)chance);
									unguard;
									return true;
								}else if(etcItemType == ETCITEM_ANCIENT_CRYSTAL_ENCHANT_AM)
								{
									g_DB.RequestEnchantItem(pScroll, pBonusItem, pItem, enchantLevel, pUser, (INT32)chance);
									pUser->pEUD->isCrystalEnchantFail = pItem->pSID->itemId;
									unguard;
									return true;
								}else
								{
									g_DB.RequestEnchantItemFail(pScroll, pBonusItem, pItem, pUser, (INT32)chance);
									unguard;
									return false;
								}
							}
						}
					}else
					{
						pUser->EnchantCancel();
						pUser->pUserSocket->SendSystemMessage(L"This item already got max enchant level.");
						unguard;
						return false;
					}
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid grade[%d] item User[%S]", __FUNCTION__, crystalType, pUser->pSD->wName);
				pUser->EnchantCancel();
				unguard;
				return false;
			}
		}else
		{
			typedef bool (*f)(CItem*, CItem*, User*, UINT, double);
			bool ret = f(0x6931DC)(pScroll, pItem, pUser, changeType, param);

			unguard;
			return ret;
		}
	}
	unguard;
	return false;
}

bool CItemEnchant::CanEnchant(User *pUser, CItem *pItem, CItem *pScroll)
{
	if(pUser && pItem && pScroll)
	{
		UINT crystalType = pItem->pII->crystalType;
		UINT enchantLevel = pItem->pSID->enchantLevel;
		UINT scrollType = pScroll->pII->etcItemType;
		
		if(pUser->pEUD->isItemDataLock)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
			return false;
		}
		if(pItem->IsWeapon())
		{
			if(pItem->pII->magicWeapon)
			{
				if(scrollType== ETCITEM_SCRL_SAFE_ENCHANT_AM || scrollType == ETCITEM_SCRL_SAFE_ENCHANT_WP)
				{
					if(enchantLevel >= g_ItemEnchantConfig[crystalType].maxSafeMagicWeaponLevel)
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot enchant this item with safe enchant scroll anymore.");
						return false;
					}
				}else
				{
					if(enchantLevel >= g_ItemEnchantConfig[crystalType].maxMagicWeaponLevel)
					{
						pUser->pUserSocket->SendSystemMessage(L"This item already got max enchant level.");
						return false;
					}
				}
			}else
			{
				if(scrollType== ETCITEM_SCRL_SAFE_ENCHANT_AM || scrollType == ETCITEM_SCRL_SAFE_ENCHANT_WP)
				{
					if(enchantLevel >= g_ItemEnchantConfig[crystalType].maxSafeWeaponLevel)
					{
						pUser->pUserSocket->SendSystemMessage(L"You cannot enchant this item with safe enchant scroll anymore.");
						return false;
					}
				}else
				{
					if(enchantLevel >= g_ItemEnchantConfig[crystalType].maxWeaponLevel)
					{
						pUser->pUserSocket->SendSystemMessage(L"This item already got max enchant level.");
						return false;
					}
				}
			}
		}else
		{
			if(scrollType== ETCITEM_SCRL_SAFE_ENCHANT_AM || scrollType == ETCITEM_SCRL_SAFE_ENCHANT_WP)
			{
				if(enchantLevel >= g_ItemEnchantConfig[crystalType].maxSafeArmorLevel)
				{
					pUser->pUserSocket->SendSystemMessage(L"You cannot enchant this item with safe enchant scroll anymore.");
					return false;
				}
			}else
			{
				if(enchantLevel >= g_ItemEnchantConfig[crystalType].maxArmorLevel)
				{
					pUser->pUserSocket->SendSystemMessage(L"This item already got max enchant level.");
					return false;
				}
			}
		}

		
	}
	return true;
}

void CItemEnchant::SetSafeAttribute(UINT value)
{
	UINT index = GetThreadIndex();
	if(index < 32)
	{
		g_SafeAttributeValue[index] = value;
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Thread index overflow[%d] !", __FUNCTION__, index);
	}
}

UINT CItemEnchant::GetSafeAttribute()
{
	UINT index = GetThreadIndex();
	if(index < 32)
	{
		return g_SafeAttributeValue[index];
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Thread index overflow[%d] !", __FUNCTION__, index);
	}
	return 0;
}

bool CItemEnchant::EnchantAttributeItemWeapon(CItem *pScroll, CItem* pItem, User *pUser, UINT attributeTypeEnum, INT32 levelLimit)
{
	typedef bool (*f)(CItem*, CItem*, User*, UINT, INT32);
	
	if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_ARMOR_ATTR)
	{
		pUser->SetPendingUseETCItem(0, PUI_ATTRIBUTE);
		pUser->isEnchantingAttribute = false;
		pUser->pUserSocket->SendSystemMessage(L"You have to select an armor without attribute to continue!");
		return false;
	}

	if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_WEAPON_ATTR)
	{
		BaseAttribute attr;
		pItem->GetTotalAttribute(&attr);
		if(attr.attackAttributeValue == 0)
		{
			SetSafeAttribute(levelLimit);
			levelLimit = 9;
		}else
		{
			pUser->SetPendingUseETCItem(0, PUI_ATTRIBUTE);
			pUser->isEnchantingAttribute = false;
			pUser->pUserSocket->SendSystemMessage(L"You have to select a weapon without attribute to continue!");
			return false;
		}
	}
	bool ret = f(0x6965B4L)(pScroll, pItem, pUser, attributeTypeEnum, levelLimit);

	if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_WEAPON_ATTR)
	{
		SetSafeAttribute(0);
	}

	return ret;
}

double CItemEnchant::EnchantAttributeItemWeaponDice()
{
	if(GetSafeAttribute() > 0)
	{
		return 0.0;
	}
	typedef double (*f)();
	return f(0x94D9E4L)();
}

WORD GetEnchantAttributeValueWrapper()
{
	return CItemEnchant::GetSafeAttribute();
}

bool CItemEnchant::EnchantAttributeItemArmor(CItem *pScroll, CItem* pItem, User *pUser, UINT attributeTypeEnum, INT32 levelLimit)
{
	typedef bool (*f)(CItem*, CItem*, User*, UINT, INT32);
	
	if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_WEAPON_ATTR)
	{
		pUser->SetPendingUseETCItem(0, PUI_ATTRIBUTE);
		pUser->isEnchantingAttribute = false;
		pUser->pUserSocket->SendSystemMessage(L"You have to select a weapon without attribute to continue!");
		return false;
	}

	if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_ARMOR_ATTR)
	{
		BaseAttribute attr;
		pItem->GetTotalAttribute(&attr);

		for(int n=0;n<6;n++)
		{
			if(attr.defAttribute[n] > 0)
			{
				pUser->SetPendingUseETCItem(0, PUI_ATTRIBUTE);
				pUser->isEnchantingAttribute = false;
				pUser->pUserSocket->SendSystemMessage(L"You have to select an armor without attribute to continue!");
				return false;
			}
		}

		SetSafeAttribute(levelLimit);
		levelLimit = 9;
	}
	bool ret = f(0x695FF8L)(pScroll, pItem, pUser, attributeTypeEnum, levelLimit);

	if(pScroll->pII->etcItemType == ETCITEM_SCRL_SAFE_ENCHANT_ARMOR_ATTR)
	{
		SetSafeAttribute(0);
	}

	return ret;
}

double CItemEnchant::EnchantAttributeItemArmorDice()
{
	if(GetSafeAttribute() > 0)
	{
		return 0.0;
	}
	typedef double (*f)();
	return f(0x94D9E4L)();
}