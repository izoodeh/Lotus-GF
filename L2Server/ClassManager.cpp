#include "stdafx.h"
#include "ClassManager.h"
#include "CategoryDataDB.h"
#include "UserAction.h"
#include "DB.h"

CClassManager g_ClassManager;

CClassManager::CClassManager() : m_enabled(FALSE), m_enabledFirstClass(FALSE), m_enabledSecondClass(FALSE), m_enabledThirdClass(FALSE), m_enabledSubClassQuest(FALSE), m_enabledNobless(FALSE)
{
}

CClassManager::~CClassManager()
{
}

void CClassManager::Init()
{
	/*
	Enabled=0
FirstChangeItemId=57
FirstChangeItemCount=10000
SecondChangeItemId=57
SecondChangeItemCount=100000
ThirdChangeItemId=57
ThirdChangeItemCount=1000000
*/
	TCHAR file[261];
	GetCurrentDirectory(MAX_PATH, file);
	lstrcat(file, _T("\\ServerExt.ini"));

	const TCHAR* section = _T("ClassManager");
	m_enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, file);
	m_enabledFirstClass = GetPrivateProfileInt(section, _T("EnableFirstClass"), 1, file);
	m_requiredItemId[0] = GetPrivateProfileInt(section, _T("FirstChangeItemId"), 57, file);
	m_requiredItemCount[0] = GetPrivateProfileInt(section, _T("FirstChangeItemCount"), 10000, file);
	m_enabledSecondClass = GetPrivateProfileInt(section, _T("EnableSecondClass"), 1, file);
	m_requiredItemId[1] = GetPrivateProfileInt(section, _T("SecondChangeItemId"), 57, file);
	m_requiredItemCount[1] = GetPrivateProfileInt(section, _T("SecondChangeItemCount"), 100000, file);
	m_enabledThirdClass = GetPrivateProfileInt(section, _T("EnableThirdClass"), 1, file);
	m_requiredItemId[2] = GetPrivateProfileInt(section, _T("ThirdChangeItemId"), 57, file);
	m_requiredItemCount[2] = GetPrivateProfileInt(section, _T("ThirdChangeItemCount"), 1000000, file);
	m_enabledNobless = GetPrivateProfileInt(section, _T("EnableNobless"), 0, file);
	m_noblessItemId = GetPrivateProfileInt(section, _T("NoblessItemId"), 0, file);
	m_noblessItemCount = GetPrivateProfileInt(section, _T("NoblessItemCount"), 0, file);
	m_enabledSubClassQuest = GetPrivateProfileInt(section, _T("EnableSubClassQuest"), 0, file);
	m_subClassQuestItemId = GetPrivateProfileInt(section, _T("SubClassQuestItemId"), 0, file);
	m_subClassQuestItemCount = GetPrivateProfileInt(section, _T("SubClassQuestItemCount"), 0, file);
	m_allowInspectorOnMain = GetPrivateProfileInt(section, _T("AllowInspectorOnMain"), 0, file);
	m_disableInspector = GetPrivateProfileInt(section, _T("DisableInspector"), 0, file);

	if(m_enabled)
	{
		g_Log.Add(CLog::Blue, "[Class Manager] Feature is enabled. First[%d] Second[%d] Third[%d].", m_enabledFirstClass, m_enabledSecondClass, m_enabledThirdClass);
	}
	

	m_class_name[0] = L"Fighter";
	m_class_name[1] = L"Warrior";
	m_class_name[2] = L"Gladiator";
	m_class_name[3] = L"Warlord";
	m_class_name[4] = L"Knight";
	m_class_name[5] = L"Paladin";
	m_class_name[6] = L"Dark Avenger";
	m_class_name[7] = L"Rogue";
	m_class_name[8] = L"Treasure Hunter";
	m_class_name[9] = L"Hawkeye";
	m_class_name[10] = L"Mage";
	m_class_name[11] = L"Wizard";
	m_class_name[12] = L"Sorcerer";
	m_class_name[13] = L"Necromancer";
	m_class_name[14] = L"Warlock";
	m_class_name[15] = L"Cleric";
	m_class_name[16] = L"Bishop";
	m_class_name[17] = L"Prophet";
	m_class_name[18] = L"Elven Fighter";
	m_class_name[19] = L"Elven Knight";
	m_class_name[20] = L"Temple Knight";
	m_class_name[21] = L"Swordsinger";
	m_class_name[22] = L"Elven Scout";
	m_class_name[23] = L"Plains Walker";
	m_class_name[24] = L"Silver Ranger";
	m_class_name[25] = L"Elven Mage";
	m_class_name[26] = L"Elven Wizard";
	m_class_name[27] = L"Spellsinger";
	m_class_name[28] = L"Elemental Summoner";
	m_class_name[29] = L"Oracle";
	m_class_name[30] = L"Elder";
	m_class_name[31] = L"Dark Fighter";
	m_class_name[32] = L"Palus Knight";
	m_class_name[33] = L"Shillien Knight";
	m_class_name[34] = L"Bladedancer";
	m_class_name[35] = L"Assassin";
	m_class_name[36] = L"Abyss Walker";
	m_class_name[37] = L"Phantom Ranger";
	m_class_name[38] = L"Dark Mage";
	m_class_name[39] = L"Dark Wizard";
	m_class_name[40] = L"Spellhowler";
	m_class_name[41] = L"Phantom Summoner";
	m_class_name[42] = L"Shillien Oracle";
	m_class_name[43] = L"Shillien Elder";
	m_class_name[44] = L"Orc Fighter";
	m_class_name[45] = L"Orc Raider";
	m_class_name[46] = L"Destroyer";
	m_class_name[47] = L"Orc Monk";
	m_class_name[48] = L"Tyrant";
	m_class_name[49] = L"Orc Mage";
	m_class_name[50] = L"Orc Shaman";
	m_class_name[51] = L"Overlord";
	m_class_name[52] = L"Warcryer";
	m_class_name[53] = L"Dwarven Fighter";
	m_class_name[54] = L"Scavenger";
	m_class_name[55] = L"Bounty Hunter";
	m_class_name[56] = L"Artisan";
	m_class_name[57] = L"Warsmith";

	m_class_name[88] = L"Duelist";
	m_class_name[89] = L"Dreadnought";
	m_class_name[90] = L"Phoenix Knight";
	m_class_name[91] = L"Hell Knight";
	m_class_name[92] = L"Sagittarius";
	m_class_name[93] = L"Adventurer";
	m_class_name[94] = L"Archmage";
	m_class_name[95] = L"Soultaker";
	m_class_name[96] = L"Arcana Lord";
	m_class_name[97] = L"Cardinal";
	m_class_name[98] = L"Hierophant";
	m_class_name[99] = L"Evas Templar";
	m_class_name[100] = L"Sword Muse";
	m_class_name[101] = L"Wind Rider";
	m_class_name[102] = L"Moonlight Sentinel";
	m_class_name[103] = L"Mystic Muse";
	m_class_name[104] = L"Elemental Master";
	m_class_name[105] = L"Evas Saint";
	m_class_name[106] = L"Shillien Templar";
	m_class_name[107] = L"Spectral Dancer";
	m_class_name[108] = L"Ghost Hunter";
	m_class_name[109] = L"Ghost Sentinel";
	m_class_name[110] = L"Storm Screamer";
	m_class_name[111] = L"Spectral Master";
	m_class_name[112] = L"Shillien Saint";
	m_class_name[113] = L"Titan";
	m_class_name[114] = L"Grand Khavatari";
	m_class_name[115] = L"Dominator";
	m_class_name[116] = L"Doomcryer";
	m_class_name[117] = L"Fortune Seeker";
	m_class_name[118] = L"Maestro";

	m_class_name[123] = L"Kamael Male Soldier";
	m_class_name[124] = L"Kamael Female Soldier";
	m_class_name[125] = L"Trooper";
	m_class_name[126] = L"Warder";
	m_class_name[127] = L"Berserker";
	m_class_name[128] = L"Male Soul Breaker";
	m_class_name[129] = L"Female Soul Breaker";
	m_class_name[130] = L"Arbalester";
	m_class_name[131] = L"Doombringer";
	m_class_name[132] = L"Male Soul Hound";
	m_class_name[133] = L"Female Soul Hound";
	m_class_name[134] = L"Trickster";
	m_class_name[135] = L"Inspector";
	m_class_name[136] = L"Judicator";
}

wstring CClassManager::GetClassName(INT32 classId)
{
	map<INT32, wstring>::iterator Iter = m_class_name.find(classId);
	if(Iter!=m_class_name.end())
	{
		return Iter->second;
	}
	return L"MissingName";
}

void CClassManager::GetAvailable(int baseClassId, std::list<int> &lAvailable)
{
	if(baseClassId == 0)
	{
		lAvailable.push_back(1);
		lAvailable.push_back(4);
		lAvailable.push_back(7);
		return;
	}
	if(baseClassId == 1)
	{
		lAvailable.push_back(2);
		lAvailable.push_back(3);
		return;
	}	
	if(baseClassId == 2)
	{
		lAvailable.push_back(88);
		return;
	}
	if(baseClassId == 3)
	{
		lAvailable.push_back(89);
		return;
	}
	if(baseClassId == 4)
	{
		lAvailable.push_back(5);
		lAvailable.push_back(6);
		return;
	}
	if(baseClassId == 5)
	{
		lAvailable.push_back(90);
		return;
	}
	if(baseClassId == 6)
	{
		lAvailable.push_back(91);
		return;
	}
	if(baseClassId == 7)
	{
		lAvailable.push_back(8);
		lAvailable.push_back(9);
		return;
	}
	if(baseClassId == 8)
	{
		lAvailable.push_back(93);
		return;
	}
	if(baseClassId == 9)
	{
		lAvailable.push_back(92);
		return;
	}
	if(baseClassId == 10)
	{
		lAvailable.push_back(11);
		lAvailable.push_back(15);
		return;
	}
	if(baseClassId == 11)
	{
		lAvailable.push_back(12);
		lAvailable.push_back(13);
		lAvailable.push_back(14);
		return;
	}
	if(baseClassId == 12)
	{
		lAvailable.push_back(94);
		return;
	}
	if(baseClassId == 13)
	{
		lAvailable.push_back(95);
		return;
	}
	if(baseClassId == 14)
	{
		lAvailable.push_back(96);
		return;
	}
	if(baseClassId == 15)
	{
		lAvailable.push_back(16);
		lAvailable.push_back(17);
		return;
	}
	if(baseClassId == 16)
	{
		lAvailable.push_back(97);
		return;
	}
	if(baseClassId == 17)
	{
		lAvailable.push_back(98);
		return;
	}
	if(baseClassId == 18)
	{
		lAvailable.push_back(19);
		lAvailable.push_back(22);
		return;
	}
	if(baseClassId == 19)
	{
		lAvailable.push_back(20);
		lAvailable.push_back(21);
		return;
	}
	if(baseClassId == 20)
	{
		lAvailable.push_back(99);
		return;
	}
	if(baseClassId == 21)
	{
		lAvailable.push_back(100);
		return;
	}
	if(baseClassId == 22)
	{
		lAvailable.push_back(23);
		lAvailable.push_back(24);
		return;
	}
	if(baseClassId == 23)
	{
		lAvailable.push_back(101);
		return;
	}
	if(baseClassId == 24)
	{
		lAvailable.push_back(102);
		return;
	}
	if(baseClassId == 25)
	{
		lAvailable.push_back(26);
		lAvailable.push_back(29);
		return;
	}
	if(baseClassId == 26)
	{
		lAvailable.push_back(27);
		lAvailable.push_back(28);
		return;
	}
	if(baseClassId == 27)
	{
		lAvailable.push_back(103);
		return;
	}
	if(baseClassId == 28)
	{
		lAvailable.push_back(104);
		return;
	}
	if(baseClassId == 29)
	{
		lAvailable.push_back(30);
		return;
	}
	if(baseClassId == 30)
	{
		lAvailable.push_back(105);
		return;
	}
	if(baseClassId == 31)
	{
		lAvailable.push_back(32);
		lAvailable.push_back(35);
		return;
	}
	if(baseClassId == 32)
	{
		lAvailable.push_back(33);
		lAvailable.push_back(34);
		return;
	}
	if(baseClassId == 33)
	{
		lAvailable.push_back(106);
		return;
	}
	if(baseClassId == 34)
	{
		lAvailable.push_back(107);
		return;
	}
	if(baseClassId == 35)
	{
		lAvailable.push_back(36);
		lAvailable.push_back(37);
		return;
	}
	if(baseClassId == 36)
	{
		lAvailable.push_back(108);
		return;
	}
	if(baseClassId == 37)
	{
		lAvailable.push_back(109);
		return;
	}
	if(baseClassId == 38)
	{
		lAvailable.push_back(39);
		lAvailable.push_back(42);
		return;
	}
	if(baseClassId == 39)
	{
		lAvailable.push_back(40);
		lAvailable.push_back(41);
		return;
	}
	if(baseClassId == 40)
	{
		lAvailable.push_back(110);
		return;
	}
	if(baseClassId == 41)
	{
		lAvailable.push_back(111);
		return;
	}
	if(baseClassId == 42)
	{
		lAvailable.push_back(43);
		return;
	}
	if(baseClassId == 43)
	{
		lAvailable.push_back(112);
		return;
	}
	if(baseClassId == 44)
	{
		lAvailable.push_back(45);
		lAvailable.push_back(47);
		return;
	}
	if(baseClassId == 45)
	{
		lAvailable.push_back(46);
		return;
	}
	if(baseClassId == 46)
	{
		lAvailable.push_back(113);
		return;
	}
	if(baseClassId == 47)
	{
		lAvailable.push_back(48);
		return;
	}
	if(baseClassId == 48)
	{
		lAvailable.push_back(114);
		return;
	}
	if(baseClassId == 49)
	{
		lAvailable.push_back(50);
		return;
	}
	if(baseClassId == 50)
	{
		lAvailable.push_back(51);
		lAvailable.push_back(52);
		return;
	}
	if(baseClassId == 51)
	{
		lAvailable.push_back(115);
		return;
	}
	if(baseClassId == 52)
	{
		lAvailable.push_back(116);
		return;
	}
	if(baseClassId == 53)
	{
		lAvailable.push_back(54);
		lAvailable.push_back(56);
		return;
	}
	if(baseClassId == 54)
	{
		lAvailable.push_back(55);
		return;
	}
	if(baseClassId == 55)
	{
		lAvailable.push_back(117);
		return;
	}
	if(baseClassId == 56)
	{
		lAvailable.push_back(57);
		return;
	}
	if(baseClassId == 57)
	{
		lAvailable.push_back(118);
		return;
	}

	if(baseClassId == 123)
	{
		lAvailable.push_back(125);
		return;
	}

	if(baseClassId == 124)
	{
		lAvailable.push_back(126);
		return;
	}

	if(baseClassId == 125)
	{
		lAvailable.push_back(127);
		lAvailable.push_back(128);
		return;
	}
	if(baseClassId == 126)
	{
		if(!m_disableInspector)
		{
			lAvailable.push_back(135);
		}
		lAvailable.push_back(130);
		lAvailable.push_back(129);
		return;
	}

	if(baseClassId == 127)
	{
		lAvailable.push_back(131);
		return;
	}

	if(baseClassId == 128)
	{
		lAvailable.push_back(132);
		return;
	}

	if(baseClassId == 129)
	{
		lAvailable.push_back(133);
		return;
	}

	if(baseClassId == 130)
	{
		lAvailable.push_back(134);
		return;
	}

	if(baseClassId == 135)
	{
		lAvailable.push_back(136);
		return;
	}
}

bool CClassManager::CanChange(User *pUser, int newClass)
{
	if(newClass == 135 && m_allowInspectorOnMain == FALSE)
	{	//inspector class
		if(pUser->pSD->subjob_id == 0)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot change your class to Inspector while being on main class!");
			return false;
		}
	}

	if(g_CategoryDataDB.IsInCategory(newClass, fourth_class_group_category))
	{
		if(m_enabledThirdClass)
		{
			if(pUser->pSD->level > 75)
			{
				list<INT32> lClass;
				GetAvailable(pUser->pSD->occupation, lClass);
				for(list<INT32>::iterator Iter = lClass.begin();Iter!=lClass.end();Iter++)
				{
					if((*Iter) == newClass)
					{
						if(m_requiredItemId[2] > 0 && m_requiredItemCount[2] > 0)
						{
							if(pUser->inventory.HaveItemByClassId(m_requiredItemId[2], m_requiredItemCount[2]))
							{
								pUser->DeleteItemInInventory(m_requiredItemId[2], m_requiredItemCount[2]);
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"Not enough required item(s).");
								return false;
							}
						}
						return true;
					}
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"Please come back when you reach 76 or higher level.");
				return false;
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot change third class at the moment.");
			return false;
		}
	}else if(g_CategoryDataDB.IsInCategory(newClass, third_class_group_category))
	{
		if(m_enabledSecondClass)
		{
			if(pUser->pSD->level > 39)
			{
				list<INT32> lClass;
				GetAvailable(pUser->pSD->occupation, lClass);
				for(list<INT32>::iterator Iter = lClass.begin();Iter!=lClass.end();Iter++)
				{
					if((*Iter) == newClass)
					{
						if(m_requiredItemId[1] > 0 && m_requiredItemCount[1] > 0)
						{
							if(pUser->inventory.HaveItemByClassId(m_requiredItemId[1], m_requiredItemCount[1]))
							{
								pUser->DeleteItemInInventory(m_requiredItemId[1], m_requiredItemCount[1]);
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"Not enough required item(s).");
								return false;
							}
						}
						return true;
					}
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"Please come back when you reach 40 or higher level.");
				return false;
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot change second class at the moment.");
			return false;
		}
	}else if(g_CategoryDataDB.IsInCategory(newClass, second_class_group_category))
	{
		if(m_enabledFirstClass)
		{
			if(pUser->pSD->level > 19)
			{
				list<INT32> lClass;
				GetAvailable(pUser->pSD->occupation, lClass);
				for(list<INT32>::iterator Iter = lClass.begin();Iter!=lClass.end();Iter++)
				{
					if((*Iter) == newClass)
					{
						if(m_requiredItemId[0] > 0 && m_requiredItemCount[0] > 0)
						{
							//g_Log.Add(CLog::Blue, "required item[%d][%d]", m_requiredItemId[0], m_requiredItemCount[0] );

							if(pUser->inventory.HaveItemByClassId(m_requiredItemId[0], m_requiredItemCount[0]))
							{
								pUser->DeleteItemInInventory(m_requiredItemId[0], m_requiredItemCount[0]);
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"Not enough required item(s).");
								return false;
							}
						}
						return true;
					}
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"Please come back when you reach 20 or higher level.");
				return false;
			}
		}else
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot change first class at the moment.");
			return false;
		}
	}

	g_Log.Add(CLog::Error, "[%s] Invalid new_class[%d] user[%S] base_class[%d]", __FUNCTION__, newClass, pUser->pSD->wName, pUser->pSD->occupation);

	return false;
}

void CClassManager::RequestShowHtml(User *pUser)
{
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(!g_CategoryDataDB.IsInCategory(pUser->pSD->occupation, fourth_class_group_category) || !pUser->pSD->HaveNRMemo(235) || !pUser->pSD->HaveNRMemo(247))
			{
				list<INT32> lClass;
				GetAvailable(pUser->pSD->occupation, lClass);

				wstringstream htmlStream;
				htmlStream << L"<html><head></head><body><center></br></br></br>";
				for(list<INT32>::iterator Iter = lClass.begin(); Iter!= lClass.end();)
				{
					INT32 newClass = (*Iter);
					if(!m_enabledFirstClass && g_CategoryDataDB.IsInCategory(newClass, second_class_group_category))
					{
						lClass.erase(Iter++);
						continue;
					}
					if(!m_enabledSecondClass && g_CategoryDataDB.IsInCategory(newClass, third_class_group_category))
					{
						lClass.erase(Iter++);
						continue;
					}
					if(!m_enabledThirdClass && g_CategoryDataDB.IsInCategory(newClass, fourth_class_group_category))
					{
						lClass.erase(Iter++);
						continue;
					}
					Iter++;
				}


				if(m_enabledSubClassQuest)
				{
					if(!pUser->pSD->HaveNRMemo(235) && pUser->pSD->level >= 75)
					{
						htmlStream << L"<a action=\"bypass -h cm_subclass" << L"\">" << L"Subclass Quest" << L"</a></br>";
					}
				}
				if(m_enabledNobless)
				{
					if(!pUser->pSD->HaveNRMemo(247) && pUser->pSD->subjob_id != 0 && pUser->pSD->level >= 75 && pUser->pSD->nobless_type == 0)
					{
						htmlStream << L"<a action=\"bypass -h cm_nobless" << L"\">" << L"Nobless Quest" << L"</a></br>";
					}
				}
				if(lClass.size() > 0)
				{
					for(list<INT32>::iterator Iter = lClass.begin();Iter!=lClass.end(); Iter++)
					{
						htmlStream << L"<a action=\"bypass -h cm_change?class=" << (*Iter) << L"\">" << GetClassName((*Iter)) << L"</a></br>";
					}
				}else
				{
					htmlStream << L"That's all i can do for you at the moment!";
				}
				htmlStream << L"</center></body></html>";
				pUser->ShowHTML((PWCHAR)L"class_change_show.htm", (PWCHAR)htmlStream.str().c_str(), 0);
			}else
			{
				wstringstream htmlStream;
				htmlStream << L"<html><head></head><body><center></br></br></br>";
				htmlStream << L"There's nothing i can do for you!";
				htmlStream << L"</center></body></html>";
				pUser->ShowHTML(L"class_change_show.htm", htmlStream.str().c_str(), 0);

			}
		}
	}
}

void CClassManager::RequestChange(User *pUser, int newClass)
{
	if(m_enabled)
	{
		if(pUser->ValidUser())
		{
			if(CanChange(pUser, newClass))
			{
				UserAction::ChangeClass(pUser, newClass);
				pUser->SocialAction(SAT_LEVEL_UP, pUser->serverId);
				pUser->pUserSocket->SendSystemMessage(L"Congratulation! You've changed your class!");
				g_DB.Send("chddd", 0xF8, 0x22, pUser->databaseId, pUser->pSD->subjob_id, pUser->pSD->occupation);
			}
		}
	}
}

void CClassManager::RequestNobless(User *pUser)
{
	if(m_enabled && m_enabledNobless)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pSD->nobless_type == 0)
			{
				if(pUser->pSD->subjob_id != 0 && pUser->pSD->level >= 75)
				{
					//check for item requirement
					if(m_noblessItemId > 0 && m_noblessItemCount > 0)
					{
						if(pUser->inventory.HaveItemByClassId(m_noblessItemId, m_noblessItemCount))
						{
							pUser->DeleteItemInInventory(m_noblessItemId, m_noblessItemCount);
						}else
						{
							pUser->pUserSocket->SendSystemMessage(L"You don't have enough required item(s) to become a nobless!");
							return;
						}
					}


					pUser->lpYieldLock->Enter(__FILEW__, __LINE__);
					typedef void (*f)(LPBYTE, INT32, BYTE);
					f(0x443730L)(pUser->pSD->oneTimeQuestMap, 247, 1);
					g_DB.SaveQuestInfo(pUser);
					pUser->lpYieldLock->Leave(__FILEW__, __LINE__);

					g_DB.RequestSaveNoblessType(pUser, 1);
					pUser->AddItemToInventory(7694, 1);
					pUser->pUserSocket->SendSystemMessage(L"Congratulations! You've become the nobless!");
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You have to be in subclass with at least 75 level to be able to become the nobless!");
				}
			}else
			{
				pUser->pUserSocket->SendSystemMessage(L"You already have nobless status!");
			}
		}
	}
}

void CClassManager::RequestSubClassQuest(User *pUser)
{
	if(m_enabled && m_enabledSubClassQuest)
	{
		if(pUser->ValidUser())
		{
			if(pUser->pSD->race == 5)
			{
				if(!pUser->pSD->HaveNRMemo(236))
				{
					if(pUser->pSD->subjob_id == 0 && pUser->pSD->level >= 75)
					{
						//check for item requirement
						if(m_subClassQuestItemId > 0 && m_subClassQuestItemCount > 0)
						{
							if(pUser->inventory.HaveItemByClassId(m_subClassQuestItemId, m_subClassQuestItemCount))
							{
								pUser->DeleteItemInInventory(m_subClassQuestItemId, m_subClassQuestItemCount);
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"You don't have enough required item(s) to perform this action!");
								return;
							}
						}

						pUser->lpYieldLock->Enter(__FILEW__, __LINE__);
						pUser->pSD->SetNRMemo(236);
						typedef void (*f)(LPBYTE, INT32, BYTE);
						f(0x443730L)(pUser->pSD->oneTimeQuestMap, 236, 1);
						g_DB.SaveQuestInfo(pUser);
						pUser->lpYieldLock->Leave(__FILEW__, __LINE__);
						pUser->pUserSocket->SendSystemMessage(L"Congratulations! From now you can make a subclass!");
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"You have to be at least 75 level to be able to perform this action!");
					}
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You already acquired subclass quest!");
				}
			}else
			{
				if(!pUser->pSD->HaveNRMemo(235))
				{
					if(pUser->pSD->subjob_id == 0 && pUser->pSD->level >= 75)
					{
						//check for item requirement
						if(m_subClassQuestItemId > 0 && m_subClassQuestItemCount > 0)
						{
							if(pUser->inventory.HaveItemByClassId(m_subClassQuestItemId, m_subClassQuestItemCount))
							{
								pUser->DeleteItemInInventory(m_subClassQuestItemId, m_subClassQuestItemCount);
							}else
							{
								pUser->pUserSocket->SendSystemMessage(L"You don't have enough required item(s) to perform this action!");
								return;
							}
						}

						pUser->lpYieldLock->Enter(__FILEW__, __LINE__);
						pUser->pSD->SetNRMemo(235);
						typedef void (*f)(LPBYTE, INT32, BYTE);
						f(0x443730L)(pUser->pSD->oneTimeQuestMap, 235, 1);
						g_DB.SaveQuestInfo(pUser);
						pUser->lpYieldLock->Leave(__FILEW__, __LINE__);
						pUser->pUserSocket->SendSystemMessage(L"Congratulations! From now you can make a subclass!");
					}else
					{
						pUser->pUserSocket->SendSystemMessage(L"You have to be at least 75 level to be able to perform this action!");
					}
				}else
				{
					pUser->pUserSocket->SendSystemMessage(L"You already acquired subclass quest!");
				}
			}
		}
	}
}