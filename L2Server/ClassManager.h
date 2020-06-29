#pragma once

class CClassManager
{
	BOOL m_enabled;
	BOOL m_enabledFirstClass;
	BOOL m_enabledSecondClass;
	BOOL m_enabledThirdClass;
	INT32 m_requiredItemId[3];
	INT32 m_requiredItemCount[3];
	BOOL m_enabledNobless;
	INT32 m_noblessItemId;
	INT32 m_noblessItemCount;
	BOOL m_enabledSubClassQuest;
	INT32 m_subClassQuestItemId;
	INT32 m_subClassQuestItemCount;
	BOOL m_allowInspectorOnMain;
	BOOL m_disableInspector;

	map<INT32, wstring> m_class_name;
public:
	CClassManager();
	~CClassManager();
	void Init();
	wstring GetClassName(INT32 classId);
	void GetAvailable(int baseClassId, list<int>& lAvailable);
	bool CanChange(User *pUser, int newClass);
	void RequestShowHtml(User *pUser);
	void RequestChange(User *pUser, int newClass);
	void RequestNobless(User *pUser);
	void RequestSubClassQuest(User *pUser);
};

extern CClassManager g_ClassManager;