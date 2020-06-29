#pragma once

class CDonateSystem : public CFeature
{
	BOOL enabled;
	INT32 rewardId;
	BOOL donateCmdAsConvert;
	void LoadINI();
	void ConvertPoints(User *pUser);
public:
	CDonateSystem();
	~CDonateSystem();
	BOOL IsEnabled();
	void Init();
	bool HandleCmd(User *pUser, const WCHAR* wParam);
};

extern CDonateSystem g_DonateSystem;