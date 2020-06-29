#include "StdAfx.h"
#include "ChatManager.h"
#include "ObsceneFilter.h"

CChatManager g_ChatManager;

CChatManager::CChatManager() : enabled(FALSE), globalChat(FALSE)
{
	SetName(L"Chat Manager");
}

CChatManager::~CChatManager()
{

}

void CChatManager::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	memset(chatSetting, 0, sizeof(chatSetting));
	LoadINI();
}

void CChatManager::LoadINI()
{
	const WCHAR* sectionName = L"ChatManager";
	enabled = GetPrivateProfileInt(sectionName, L"Enabled", 0, g_ConfigFile);
	globalChat = GetPrivateProfileInt(sectionName, L"GlobalChat", 0, g_ConfigFile);
	sendMessage = GetPrivateProfileInt(sectionName, L"SendMessage", 0, g_ConfigFile);
	messageId = GetPrivateProfileInt(sectionName, L"MessageId", 0, g_ConfigFile);

	chatSetting[SAY_ALL].enabled = GetPrivateProfileInt(sectionName, L"AllEnabled", 1, g_ConfigFile);
	chatSetting[SAY_ALL].minLevel = GetPrivateProfileInt(sectionName, L"AllRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_ALL].delay = GetPrivateProfileInt(sectionName, L"AllDelay", 0, g_ConfigFile);
	TCHAR temp[8190];
	if(GetPrivateProfileString(sectionName, _T("AllCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_ALL].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("AllInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_ALL].invalidCharMsg = temp;
	}

	if(GetPrivateProfileString(sectionName, _T("AllReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_ALL].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_HERO_VOICE].enabled = GetPrivateProfileInt(sectionName, L"HeroEnabled", 1, g_ConfigFile);
	chatSetting[SAY_HERO_VOICE].minLevel = GetPrivateProfileInt(sectionName, L"HeroRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_HERO_VOICE].delay = GetPrivateProfileInt(sectionName, L"HeroDelay", 2000, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("HeroCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_HERO_VOICE].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("HeroInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_HERO_VOICE].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("HeroReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_HERO_VOICE].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_TRADE].enabled = GetPrivateProfileInt(sectionName, L"TradeEnabled", 1, g_ConfigFile);
	chatSetting[SAY_TRADE].minLevel = GetPrivateProfileInt(sectionName, L"TradeRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_TRADE].delay = GetPrivateProfileInt(sectionName, L"TradeDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("TradeCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_TRADE].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("TradeInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_TRADE].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("TradeReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_TRADE].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}
	chatSetting[SAY_SHOUT].enabled = GetPrivateProfileInt(sectionName, L"ShoutEnabled", 1, g_ConfigFile);
	chatSetting[SAY_SHOUT].minLevel = GetPrivateProfileInt(sectionName, L"ShoutRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_SHOUT].delay = GetPrivateProfileInt(sectionName, L"ShoutDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("ShoutCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_SHOUT].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("ShoutInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_SHOUT].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("ShoutReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_SHOUT].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}


	chatSetting[SAY_TELL].enabled = GetPrivateProfileInt(sectionName, L"TellEnabled", 1, g_ConfigFile);
	chatSetting[SAY_TELL].minLevel = GetPrivateProfileInt(sectionName, L"TellRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_TELL].delay = GetPrivateProfileInt(sectionName, L"TellDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("TellCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_TELL].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("TellInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_TELL].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("TellReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_TELL].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_CLAN].enabled = GetPrivateProfileInt(sectionName, L"ClanEnabled", 1, g_ConfigFile);
	chatSetting[SAY_CLAN].minLevel = GetPrivateProfileInt(sectionName, L"ClanRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_CLAN].delay = GetPrivateProfileInt(sectionName, L"ClanDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("ClanCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_CLAN].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("ClanInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_CLAN].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("ClanReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_CLAN].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_ALLIANCE].enabled = GetPrivateProfileInt(sectionName, L"AllianceEnabled", 1, g_ConfigFile);
	chatSetting[SAY_ALLIANCE].minLevel = GetPrivateProfileInt(sectionName, L"AllianceRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_ALLIANCE].delay = GetPrivateProfileInt(sectionName, L"AllianceDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("AllianceCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_ALLIANCE].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("AllianceInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_ALLIANCE].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("AllianceReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_ALLIANCE].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_PARTY].enabled = GetPrivateProfileInt(sectionName, L"PartyEnabled", 1, g_ConfigFile);
	chatSetting[SAY_PARTY].minLevel = GetPrivateProfileInt(sectionName, L"PartyRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_PARTY].delay = GetPrivateProfileInt(sectionName, L"PartyDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("PartyCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_PARTY].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("PartyInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_PARTY].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("PartyReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_PARTY].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_PARTYROOM_ALL].enabled = GetPrivateProfileInt(sectionName, L"PartyRoomAllEnabled", 1, g_ConfigFile);
	chatSetting[SAY_PARTYROOM_ALL].minLevel = GetPrivateProfileInt(sectionName, L"PartyRoomAllRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_PARTYROOM_ALL].delay = GetPrivateProfileInt(sectionName, L"PartyRoomAllDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("PartyRoomAllCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_PARTYROOM_ALL].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("PartyRoomAllInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_PARTYROOM_ALL].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("PartyRoomAllReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_PARTYROOM_ALL].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

	chatSetting[SAY_PARTYROOM_COMMANDER].enabled = GetPrivateProfileInt(sectionName, L"PartyRoomCommanderEnabled", 1, g_ConfigFile);
	chatSetting[SAY_PARTYROOM_COMMANDER].minLevel = GetPrivateProfileInt(sectionName, L"PartyRoomCommanderRequiredLevel", 1, g_ConfigFile);
	chatSetting[SAY_PARTYROOM_COMMANDER].delay = GetPrivateProfileInt(sectionName, L"PartyRoomCommanderDelay", 0, g_ConfigFile);
	if(GetPrivateProfileString(sectionName, _T("PartyRoomCommanderCharset"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream patternStream(Utils::ReplaceString(temp, L"-", L" ", true));
		wstring low, high;
		while(patternStream >> low >> high)
		{
			chatSetting[SAY_PARTYROOM_COMMANDER].charset.push_back(pair<WORD, WORD>(low[0], high[0]));
		}
	}
	if(GetPrivateProfileString(sectionName, _T("PartyRoomCommanderInvalidCharMessage"), 0, temp, 8190, g_ConfigFile))
	{
		chatSetting[SAY_PARTYROOM_COMMANDER].invalidCharMsg = temp;
	}
	if(GetPrivateProfileString(sectionName, _T("PartyRoomCommanderReplaceWord"), 0, temp, 8190, g_ConfigFile))
	{
		wstringstream wordStream(Utils::ReplaceString(temp, L";", L" ", true));
		wstring wordR, wordN;
		while(wordStream >> wordR >> wordN)
		{
			chatSetting[SAY_PARTYROOM_COMMANDER].wordToReplace.push_back(pair<wstring, wstring>(wordR, wordN));
		}
	}

}

bool CChatManager::Handle(CUserSocket *pSocket, const unsigned char *packet, int type)
{
	if(enabled)
	{
		User *pUser = pSocket->pUser;
		if(pUser->ValidUser())
		{
			DWORD currentTick = GetTickCount();
			switch(type)
			{
			case SAY_ALL:
			case SAY_HERO_VOICE:
			case SAY_TRADE:
			case SAY_SHOUT:
			case SAY_TELL:
			case SAY_CLAN:
			case SAY_ALLIANCE:
			case SAY_PARTY:
			case SAY_PARTYROOM_ALL:
			case SAY_PARTYROOM_COMMANDER:
				{
					if(chatSetting[type].enabled)
					{
						if(chatSetting[type].minLevel > pUser->pSD->level)
						{
							if(sendMessage)
							{
								pSocket->SendSystemMessage(L"Chat Manager", L"Level of your chatacter is too low, you cannot use this chat.");
							}
							return false;
						}
						if(chatSetting[type].delay > 0)
						{
							if( pUser->pEUD->chatTimestamp[type] > currentTick )
							{
								if(sendMessage && messageId)
								{
									pSocket->SendSystemMessage(messageId);
								}
								return false;
							}

							pUser->pEUD->chatTimestamp[type] = currentTick + chatSetting[type].delay;
						}
						if(chatSetting[type].charset.size() > 0)
						{
							//check for forbidden characters
							WCHAR wMessage[4096];
							memset(wMessage, 0, sizeof(wMessage));
							Disassemble(packet, "S", 2048, wMessage);
							vector<pair<WORD, WORD>>& charset = chatSetting[type].charset;
							size_t len = wcslen(wMessage);
							for(UINT n=0;n<len;n++)
							{
								bool valid = false;
								for(UINT m=0;m<charset.size();m++)
								{
									if(charset[m].first <= wMessage[n] && charset[m].second >= wMessage[n])
									{
										valid = true;
										break;
									}
								}
								if(!valid && wMessage[n] != L' ' && wMessage[n] != 0x08 && wMessage[n] != 0x09 && wMessage[n] != 0x1B)
								{
								//	g_Log.Add(CLog::Blue, "[%s] char[%x]", __FUNCTION__, wMessage[n]);
									pUser->pUserSocket->SendSystemMessage(chatSetting[type].invalidCharMsg.c_str());
									return false;
								}
							}
						}
						if( chatSetting[type].wordToReplace.size() > 0 )
						{
							if(type != SAY_TELL)
							{
								wstring wBase((PWCHAR)packet);
								
								vector<pair<wstring, wstring>>& words = chatSetting[type].wordToReplace;
								for(UINT n=0;n<words.size();n++)
								{
									size_t pos = wBase.find(words[n].first);
									if(pos != wstring::npos)
									{
										bool toReplace = false;
										if(pos > 0)
										{
											if(wBase[pos - 1] == L' ')
											{
												if( (pos + words[n].first.size()) < wBase.size())
												{
													if(wBase[pos+words[n].first.size()] == L' ')
													{
														toReplace = true;
													}
												}else
												{
													toReplace = true;
												}
											}
										}else
										{
											if( (pos + words[n].first.size()) < wBase.size())
											{
												if(wBase[pos+words[n].first.size()] == L' ')
												{
													toReplace = true;
												}
											}else
											{
												toReplace = true;
											}
										}
										if(toReplace)
										{
											wBase.replace(pos, words[n].first.size(), words[n].second);
											Assemble((PCHAR)packet, 4096, "Sd", wBase.c_str(), type);
										}
									}
								}
							}
						}
						if(type == SAY_SHOUT && globalChat)
						{
							if(pUser->stopSayTimeoutTick == 0)
							{
								WCHAR wMessage[4096];
								memset(wMessage, 0, sizeof(wMessage));
								Disassemble(packet, "S", 2048, wMessage);
								BroadcastToAllUser("cddSS", 0x4A, pUser->serverId, type, pUser->pSD->wName, wMessage);
							}else
							{
								//147	1	a,Chatting is currently prohibited.\0	4	79	9B	B0	FF	a,ItemSound3.sys_chat_prohibition\0	a,147\0	0	0	0	0	0	a,	a,popup\0
								pUser->pUserSocket->SendSystemMessage(147);
							}
							return false;
						}
					}else
					{
						if(sendMessage && messageId)
						{
							pSocket->SendSystemMessage(messageId);
						}
						return false;
					}
					break;
				}
			default:
				{
					break;
				}
			}
			return UserCallback(0x9343E0L)(pSocket, packet);
		}
		return true;
	}
	return UserCallback(0x9343E0L)(pSocket, packet);
}