#include "stdafx.h"
#include "EventName.h"

CEventName g_EventName;

CEventName::CEventName() : enabled(FALSE)
{
	SetName(L"EventName");
}

CEventName::~CEventName()
{
}

void CEventName::Init()
{
	LoadData();
	if(enabled)
	{
		g_Log.Add(CLog::Blue, "[%S] Feature is enabled. Loaded [%d] eventname(s).", GetName(), vData.size());
	}
}

void CEventName::LoadData()
{
	LPBYTE lpData = 0;
	DWORD loaded = Utils::ReadFileBinary("..//Script//EventName.txt", lpData);
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
					
					continue;
				}
				if( line.find(L"event_begin") == 0 )
				{
					//event_begin	name=[test_event]	start_day=monday	start_time=20:00	end_day=monday	end_time=22:00	event_end
					EventNameData data;

					data.name = ParseOptionString(line, L"name", L"NotSet");
					wstring day = ParseOptionString(line, L"start_day", L"NotSet");
					if(day == L"sunday")
						data.startDay = 0;
					else if(day == L"monday")
						data.startDay = 1;
					else if(day == L"tuesday")
						data.startDay = 2;
					else if(day == L"wednesday")
						data.startDay = 3;
					else if(day == L"thursday")
						data.startDay = 4;
					else if(day == L"friday")
						data.startDay = 5;
					else if(day == L"saturday")
						data.startDay = 6;
					else
						data.startDay = INT_MAX;

					wstring startTime = ParseOptionString(line, L"start_time", L"00:00");
					startTime = ReplaceChar(startTime, L":", L" ");
					{
						wstringstream sstr;
						sstr << startTime;
						sstr >> data.startHour >> data.startMinute;
					}

					data.eventDieNpcClassId = ParseOptionInt(line, L"event_die_npc_class_id", 0);

					
					data.isOn = FALSE;
					vData.push_back(data);
				}
			}
		}
	}else
	{
		g_Log.Add(CLog::Error, "[%S] Cannot find ..//Script//EventName.txt !", GetName());
	}
}

void CEventName::TurnOn(EventNameData& data)
{
	if(data.isOn == FALSE)
	{
		g_Log.Add(CLog::Black, "[%S] Turning on [%S]", GetName(), data.name.c_str());
		data.isOn = TRUE;
		typedef void(*f)(const WCHAR*);
		f(0x6BEC34L)(data.name.c_str());
	}
}

void CEventName::TurnOff(EventNameData& data)
{
	if(data.isOn)
	{
		g_Log.Add(CLog::Black, "[%S] Turning off [%S]", GetName(), data.name.c_str());
		data.isOn = FALSE;
		typedef void(*f)(const WCHAR*);
		f(0x6BEC58L)(data.name.c_str());
	}
}

void CEventName::TimerExpired()
{
	if(enabled)
	{
		tm ti; time_t rawtime;
		time ( &rawtime );
		localtime_s(&ti, &rawtime );
		Lock();
		for(vector<EventNameData>::iterator Iter = vData.begin();Iter!=vData.end();Iter++)
		{
			if(!Iter->isOn)
			{
				//check start time
				if(Iter->startDay == ti.tm_wday || Iter->startDay == INT_MAX)
				{
					if(Iter->startHour == ti.tm_hour)
					{
						if(Iter->startMinute == ti.tm_min)
						{
							TurnOn(*Iter);
						}
					}
				}
			}
		}
		Unlock();
	}
}

void CEventName::OnNpcDie(CNpc *pNpc)
{
	if(enabled)
	{
		if(pNpc->ValidCreature())
		{
			Lock();
			for(vector<EventNameData>::iterator Iter = vData.begin();Iter!=vData.end();Iter++)
			{
				if(Iter->isOn && pNpc->pSD->npc_class_id == Iter->eventDieNpcClassId)
				{
					TurnOff(*Iter);
				}
			}
			Unlock();
		}
	}
}