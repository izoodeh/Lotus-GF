#pragma once

typedef bool (*CPHandler)(CUserSocket*, const unsigned char*);

struct BannedHardwareId
{
	BYTE hash[32];
	wstring accountName;
	wstring GMName;
	UINT timeout;
	BannedHardwareId() : timeout(0) { memset(hash, 0, sizeof(hash)); };
};


class CCliExt
{
	BOOL m_Enabled;
	UINT m_Version;
	INT32 m_BoxLimit;
	WCHAR m_BoxLimitHTML[0x4000];
	WCHAR m_BannedHWIDHTML[0x4000];
	CPHandler m_CustomHandler[256];

	CSLock m_BannedLock;
	vector<BannedHardwareId> m_Banned;
	void LoadINI();
public:
	void Init();
	void Bind(UINT id, CPHandler func);
	inline WCHAR* GetBannedHtml() { return m_BannedHWIDHTML; };
	inline WCHAR* GetBoxLimitHtml() { return m_BoxLimitHTML; };
	inline INT32 GetBoxLimit() { return m_BoxLimit; };
	inline void SetBoxLimit(int value) { m_BoxLimit = value; };
	void OnBanDBLoad(LPBYTE lpHwid, wstring account, wstring gmName, UINT timeout);
	bool IsBanned(LPBYTE lpHwid);
	bool UnBan(LPBYTE lpHWID, wstring gmName);
	void Ban(LPBYTE lpHWID, wstring account, wstring gmName, UINT timeout);
	void HandleCheckSum(CUserSocket *pSocket, const unsigned char* packet, UINT packetSize);
	void DecryptPacket(CUserSocket *pSocket, unsigned char* packet, UINT packetSize);
	bool HandleCustomPacket(CUserSocket *pSocket, const unsigned char* packet);
	void TimerExpired(CUserSocket *pSocket);
	inline BOOL IsEnabled() { return m_Enabled; };
	inline UINT GetVersion() { return m_Version; };
};

bool CPHDummyPacket(CUserSocket *pSocket, const unsigned char* pacekt);
bool CPHPingExPacket(CUserSocket *pSocket, const unsigned char* packet);
bool CPHHWIDPacket(CUserSocket *pSocket, const unsigned char* packet);

extern CCliExt g_CliExt;