#pragma once

struct UserItemReuse
{
	CSLock m_Lock;
	map<INT32, UINT> m_ItemReuse[4];
	UINT GetReuse(INT32 itemId, UINT subjobIndex);
	void SetReuse(INT32 itemId, UINT reuse, UINT subjobIndex);
};

class CItemReuseManager
{
	BOOL m_Enabled;
	CSLock m_Lock;
	map<UINT, UserItemReuse*> m_Data;
public:
	CItemReuseManager();
	~CItemReuseManager();
	inline BOOL IsEnabled() { return m_Enabled; };
	void Init();
	void OnEnterWorld(User *pUser);
	void SetReuse(User *pUser, INT32 itemId, UINT reuse);
	UINT GetReuse(User *pUser, INT32 itemId);
};

extern CItemReuseManager g_ItemReuseManager;