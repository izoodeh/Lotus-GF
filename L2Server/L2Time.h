#pragma once

class CL2Time
{
	CL2Time* lpInstance;
public:
	CL2Time();
	~CL2Time();
	void Init();
	static void TimerExpired(LPVOID lpInstance);
	int GetTime();

};

extern CL2Time g_L2Time;