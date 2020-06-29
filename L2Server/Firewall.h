#pragma once
#include "IPAddress.h"

class CFirewall : public CFeature
{
	enum ACTION_TYPE
	{
		ACTION_TYPE_NOTHING = 0,
		ACTION_TYPE_LOG = 1,
		ACTION_TYPE_DROP = 2,
		ACTION_TYPE_LOG_DROP = 3,
		ACTION_TYPE_KICK = 4,
		ACTION_TYPE_LOG_KICK = 5
	};

	BOOL enabled;
	DWORD minTimestamp[256];	 /* minimum timestamp */
	INT32 actionType[256];	/* action type */

	INT32 conLimit; /* connection limit per ip*/
	INT32 m_banDuration; /* ban timestamp */
	list<CIPAddress> m_safeIpList;
	
	map<CIPAddress, time_t> m_bannedIp;	/* IP/TIMESTAMP */
	CRITICAL_SECTION m_bannedIpLock;

	HANDLE hFile;
	void LoadINI();
	void AddLog(wstring log);
public:
	CFirewall();
	void Init();
	void OnPacketRecv(CUserSocket *pSocket, BYTE opCode, bool& drop, bool& kick);
	bool CanConnect(CIPAddress ip);
	static SOCKET OnAccept(SOCKET ListenSocket, sockaddr *addr, LPINT lpAddrLen);
	~CFirewall();
};

extern CFirewall g_Firewall;