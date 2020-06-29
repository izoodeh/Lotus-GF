#include "StdAfx.h"
#include "VitalitySystem.h"
#include "HookManager.h"

UINT g_VitalityDecrementRate = 100;

void VitalitySystem::Init()
{
	g_VitalityDecrementRate = GetPrivateProfileInt(TEXT("VitalitySystem"), TEXT("VitalityDecrementRate"), 100, g_ConfigFile);
	WriteCall(0x75E845, VitalitySystem::GetVitalityDecrementValue);
	WriteCall(0x75FB09, VitalitySystem::GetVitalityDecrementValue);
}

int VitalitySystem::GetVitalityDecrementValue(int level)
{
	typedef int (*f)(int);
	int value = f(0x9593ECL)(level);

	value *= g_VitalityDecrementRate;
	value /= 100;

	return value;
}