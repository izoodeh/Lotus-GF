#pragma once

struct EventNameData
{
	wstring name;
	INT32 startDay;
	INT32 startHour;
	INT32 startMinute;
	INT32 eventDieNpcClassId;
	BOOL isOn;
};

class CEventName : public CFeature
{
	BOOL enabled;
	vector<EventNameData> vData;
	void LoadData();
	void TurnOn(EventNameData& data);
	void TurnOff(EventNameData& data);
public:
	CEventName();
	~CEventName();
	void OnNpcDie(CNpc *pNpc);
	void Init();
	void TimerExpired();
};

extern CEventName g_EventName;