#include "StdAfx.h"
#include "ObsceneFilter.h"

CObsceneFilter g_ObsceneFilter;

CObsceneFilter::CObsceneFilter()
{
	SetName(L"Obscene Filter");
}

CObsceneFilter::~CObsceneFilter()
{
}

void CObsceneFilter::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());
	LoadData();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled, loaded %d word(s)", GetName(), lWords.size());
	}else
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is disabled", GetName());
	}
}

void CObsceneFilter::LoadData()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//ObsceneFilter.txt", lpData);
	if(lpData && loaded > 2)
	{
		wstringstream file((PWCHAR)&lpData[2]);
		wstring line;
		while(getline(file, line))
		{
			if(line.size() > 4)
			{
				if( line[0] == L'/' || line[0] == L';' )
					continue;

				if( line.find(L"setting_begin") == 0 )
				{
					//setting_begin	enabled=1 show_animation=1	to_low_level_msg=2344	to_high_level_msg=2345	not_enough_adena_msg=2346	setting_end
					enabled = ParseOptionInt(line, L"enabled", 0);
					messageId = ParseOptionInt(line, L"messageId", 0);
					continue;
				}
				if( line.find(L"word_begin") == 0 )
				{
					//skill_begin	skill_id=1204	skill_level=2	price=0	min_level=1	max_level=90	custom_time=3600	skill_end
					ObsceneWordData data;
					memset(&data, 0, sizeof(ObsceneWordData));

					data.actionType = ParseOptionInt(line, L"action_type", 0);
					data.chatBanTime = ParseOptionInt(line, L"chat_ban_time", 0);
					data.kick = ParseOptionInt(line, L"kick", 0);
					data.partialSearch = ParseOptionInt(line, L"partial_search", 0);
					data.sendWarning = ParseOptionInt(line, L"send_warning", 0);
					wstring word = ParseOptionString(line, L"word");
					if(word.size() > 0)
					{
						transform(word.begin(), word.end(), word.begin(), towlower);
						data.word = word;
						lWords.push_back(data);
					}					
				}
			}
		}
		delete [] lpData;
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//ObsceneFilter.txt !", GetName());
	}
}

void CObsceneFilter::CheckString(User *pUser, WCHAR* wStr, bool &drop, bool &kick)
{
	if(enabled)
	{
		wstring wMessageL(wStr);
		WCHAR wTemp[2048];
		memset(wTemp, 0, sizeof(wTemp));
		size_t len = wcslen(wStr);
		for( size_t n = 0; n < len ; n++ )
		{
			wTemp[n] = towlower(wStr[n]);
		}	

		
		for(list<ObsceneWordData>::iterator Iter = lWords.begin(); Iter!=lWords.end();Iter++)
		{
			WCHAR* wFound = wcsstr(wTemp, Iter->word.c_str());
			while(wFound)
			{
				if(Iter->partialSearch)
				{
					if(Iter->sendWarning && messageId)
					{
						pUser->pUserSocket->SendSystemMessage(messageId);
					}
					if(Iter->chatBanTime > 0)
					{
						pUser->SetStopSay((Iter->chatBanTime * 1000), true);
					}
					if(Iter->kick)
					{
						kick = true;
						return;
					}
					if(Iter->actionType == 0)
					{
						drop = true;
						return;
					}
					if(Iter->actionType == 1)
					{
						for(int n=0;n<Iter->word.size();n++)
						{
							size_t pos = wFound - wTemp;
							wStr[pos+n] = L'*';
							wTemp[pos+n] = L'*';
						}
					}
				}else
				{
					WCHAR c1 = L' ';
					WCHAR c2 = L' ';
					if(wFound != wTemp)
					{
						c1 = wFound[-1];
					}
					
					c2 = wFound[Iter->word.size()];

					wstring separators( L" \n\t\r_-.,`'~<>!@#$%^&*()=+" );
					
					if(separators.find(c1) != wstring::npos && separators.find(c2) != wstring::npos)
					{
						if(Iter->sendWarning && messageId)
						{
							pUser->pUserSocket->SendSystemMessage(messageId);
						}
						if(Iter->chatBanTime > 0)
						{
							pUser->SetStopSay((Iter->chatBanTime * 1000), true);
						}
						if(Iter->kick)
						{
							kick = true;
							return;
						}
						if(Iter->actionType == 0)
						{
							drop = true;
							return;
						}
						if(Iter->actionType == 1)
						{
							for(int n=0; n < Iter->word.size(); n++)
							{
								size_t pos = wFound - wTemp;
								wStr[pos+n] = L'*';
								wTemp[pos+n] = L'*';
							}
						}
					}

					wFound = &wFound[1];
				}

				wFound = wcsstr(wFound, Iter->word.c_str());
			}
		}
	}
}
