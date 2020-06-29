#pragma once

class CAuthStat : public CFeature
{
	BOOL enabled;
	UINT baseUserCount;
	double userCountMultipler;
	void LoadINI();
public:
	CAuthStat();
	~CAuthStat();
	void Init();
	static bool AuthUserNum(LPVOID lpInstance, int userCount, int userLimit);
};

extern CAuthStat g_AuthStat;