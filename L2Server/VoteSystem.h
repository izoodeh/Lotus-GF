#pragma once

class CVoteSystem : public CFeature
{
	BOOL enabled;
	INT32 rewardId;
	BOOL voteCmdAsConvert;
	void LoadINI();
	void ConvertPoints(User *pUser);
public:
	CVoteSystem();
	~CVoteSystem();
	BOOL IsEnabled();
	void Init();
	bool HandleCmd(User *pUser, const WCHAR* wParam);
};

extern CVoteSystem g_VoteSystem;