#ifndef CALLSTACK_H
#define CALLSTACK_H


extern UINT g_CallStackIndex[32];
extern PCWSTR g_CallStackName[32][1000];

class CCallTrack
{
	UINT m_ThreadIndex;
	UINT m_StackIndex;
public:
	CCallTrack(const WCHAR* wName) : m_ThreadIndex(GetThreadIndex()), m_StackIndex(UINT_MAX)
	{
		if(m_ThreadIndex < 32)
		{
			m_StackIndex = g_CallStackIndex[m_ThreadIndex]++;
			if(m_StackIndex < 1000)
			{
				g_CallStackName[m_ThreadIndex][m_StackIndex] = wName;
			}
		}
	}
	static bool IsFunctionCalled(PCWSTR nameOffset);
	static void CallStackThread(UINT threadId);
	~CCallTrack()
	{
		if(m_StackIndex < 1000)
		{
			UINT temp = --g_CallStackIndex[m_ThreadIndex];
			if(temp != m_StackIndex)
			{
				g_Log.Add(CLog::Error, "[%s] Invalid stack index[%d] vs [%d] last called function[%S]", __FUNCTION__, m_StackIndex, temp, g_CallStackName[m_ThreadIndex][temp]);
				g_CallStackIndex[m_ThreadIndex] = m_StackIndex;
			}
		}
	}
};

#define CTL CCallTrack ct(__FUNCTIONW__)

__forceinline void AddToCallStack(const WCHAR* funcName)
{
#ifdef USE_CALL_TRACK
	UINT index = GetThreadIndex();
	if(index < 32)
	{
		if(g_CallStackIndex[index] < 1000)
		{
			g_CallStackName[index][g_CallStackIndex[index]] = funcName;
			g_CallStackIndex[index]++;
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Index overflow! [%d] thread[%d] in function[%S]", "AddToCallStack", g_CallStackIndex[index], index, funcName);
			if(g_CallStackName[index][g_CallStackIndex[index]])
			{
				g_Log.Add(CLog::Error, "[%s] Previus caller[%S]", "AddToCallStack", g_CallStackName[index][g_CallStackIndex[index]]);
			}
		}
	}
#endif
}

#define guard AddToCallStack (__FUNCTIONW__)

__forceinline void DelFromCallStack()
{
#ifdef USE_CALL_TRACK
	UINT index = GetThreadIndex();
	if(index < 32)
	{
		g_CallStackIndex[index]--;
	}
#endif
}

#define unguard DelFromCallStack()

class CCallStack
{
	BOOL m_enabled;
public:
	CCallStack();
	~CCallStack();
	void Init();
	void GenerateLog(const WCHAR* wInfo = 0);
	static void GenerateLinError(LPVOID lpInstance);
};

extern CCallStack g_CallStack;

#endif