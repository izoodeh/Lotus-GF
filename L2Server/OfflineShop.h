#pragma once
#include "UserSocketReleaseVerifier.h"


extern "C"
{
	void OfflineShopCheck();
};

struct OfflineShopData
{
	UINT serverId;
	UINT databaseId;
	User *pUser;
	CUserSocketReleaseVerifier *pUSRV;
	FVector pos;
	time_t createTime;
};

typedef map<CUserSocket*, OfflineShopData> OfflineShopMap;

class COfflineShop : public CFeature
{
	BOOL enabled;
	INT32 requiredItemId;
	UINT requiredItemCount;
	time_t shopDuration;
	UINT limitPerIPnMAC;
	UINT limitMsgId;
	UINT notEnoughRequiredItemMsgId;
	UINT notMeetingRequirementsMsgId;
	UINT activateMsgId;
	UINT deactivateMsgId;
	UINT disabledMsgId;
	INT32 m_IdleRewardId;
	INT32 m_IdleRewardCount;
	UINT m_IdleRewardTimeout;

	OfflineShopMap mData;
	void LoadINI();
	UINT GetIPnMACCount(UINT ip, INT64 mac);
public:
	COfflineShop();
	~COfflineShop();
	void Init();
	void TimerExpired(User *pUser);
	bool CloseShop(User *pUser);
	bool RequestActivate(User *pUser);
	static void ReleaseSocket(CUserSocketReleaseVerifier *pUSRV, UINT timestamp, UINT id);
	static void OnKick(CUserSocket* pSocket);
	static void StatusWnd(LPVOID lpInstance, INT32 line, const WCHAR* format, INT32 value);
};

extern COfflineShop g_OfflineShop;