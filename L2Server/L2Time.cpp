#include "StdAfx.h"
#include "L2Time.h"
#include "HookManager.h"
#include "Protector.h"
#include "EventName.h"
#include "FactionSystem.h"
#include "ItemDecay.h"
#include "Championship.h"
#include "TvT.h"
#include "PvPSystem.h"
#include "AutoAnnounce.h"
#include "VoteReward.h"

CL2Time g_L2Time;

CL2Time::CL2Time()
{
	lpInstance = (CL2Time*)0x913ABE0;
}

CL2Time::~CL2Time()
{
}

void CL2Time::Init()
{

	WriteCall(0x6B7564, CL2Time::TimerExpired);
}

int CL2Time::GetTime()
{
	typedef int (*f)(CL2Time*);
	return f(0x6B4BE0L)(lpInstance);
}

/* Tick every 1000 ms */
void CL2Time::TimerExpired(LPVOID lpInstance)
{
	typedef void (*f)(LPVOID);
	f(0x6B73B0L)(lpInstance);

	g_Protector.TimerExpired();
	g_EventName.TimerExpired();
	g_FactionSystem.TimerExpired();
	g_ItemDecay.TimerExpired();
	g_Championship.TimerExpired();
	g_TvT.TimerExpired();
	g_PvPSystem.TimerExpired();
	g_AutoAnnounce.TimerExpired();
	g_VoteReward.TimerExpired();
}