#pragma once
#include "Socket.h"
#include "SystemMessage.h"

struct PacketCS
{
	UINT key;
	UINT previous;
	UINT current;
	UINT calculated;
	void Init()
	{
		key = 0;
		previous = 0;
		current = 0;
		calculated = 0;
	}
};

struct PingEx
{
	UINT tickId;
	UINT tickId2;
	UINT tickId3;
	UINT tickId4;
	UINT localTick;
	UINT localCheckTick;
	time_t pingTimeout;
	void SetTickId(UINT tick)
	{
		tickId4 = tickId3;
		tickId3 = tickId2;
		tickId2 = tickId;
		tickId = tick;
	}
	bool ValidTick(UINT tick)
	{
		if(tick == tickId || tick == tickId2 || tick == tickId3 || tick == tickId4)
		{
			return true;
		}
		return false;
	}
	void Init()
	{
		tickId = 0;
		localTick = 0;
		localCheckTick = 0;
		pingTimeout = 0;
	}
};

//vTable: C746B8h size: FA0h
class CUserSocket :	public CSocket
{
public:
	/* 100 */ LPVOID _unkn100[32];
	/* 200 */ LPVOID _unkn200[32];
	/* 300 */ LPVOID _unkn300[32];
	/* 400 */ LPVOID _unkn400[32];
	/* 500 */ LPVOID _unkn500[5];
	/* 528 */ User *pUser;
	/* 530 */ LPVOID _unkn530;
	/* 538 */ INT32 _unkn538;
	/* 53C */ _SYSTEMTIME relaxSystemTime;
	/* 54C */ INT32 _unkn54C;
	/* 550 */ LPVOID _unkn550[4];
	/* 570 */ UINT closeState; // 4 when closing
	/* 574 */ UINT _unkn574;
	/* 578 */ LPVOID _unkn578[17];
	/* 600 */ LPVOID _unkn600[32];
	/* 700 */ LPVOID _unkn700[32];
	/* 800 */ LPVOID _unkn800[32];
	/* 900 */ LPVOID _unkn900[32];
	/* A00 */ LPVOID _unknA00[16];
	/* A80 */ __time32_t relaxTime;
	/* A84 */ INT32 _unknA84;
	/* A88 */ UINT _unknA88;
	/* A8C */ UINT lastUsedPacketId;
	/* A90 */ LPVOID _unknA90[2];
	/* AA0 */ WCHAR accountNameW[15];
	/* ABE */ CHAR accountNameA[15];
	/* ACD */ bool _alignACD[3];
	/* AD0 */ UINT accountId;
	/* AD4 */ INT32 _unknAd4;
	/* AD8 */ LPVOID _unknAD8[5];
	/* B00 */ LPVOID _unknB00[32];
	/* C00 */ LPVOID _unknC00[32];
	/* D00 */ LPVOID _unknD00[32];
	/* E00 */ LPVOID _unknE00[32];
	/* F00 */ LPVOID _unknF00[13];
	/* F68 */ INT64 macAddress;
	/* F70 */ LPVOID _unknF70[6];
	/* FA0 */ DWORD packetTimestamp[256];
	/*13A0 */ BYTE inKeyEx[4];
	/*13A4 */ INT32 protocolVersion;
	/*13A8 */ UINT dropGetTimestamp;
	/*13B0 */ BOOL loadedHardwareId;
	/*13B4 */ BOOL requestedHardwareId;
	/*13B8 */ HWID hardwareId;
	/*xxxx */ UINT shortHWID;
	/*xxxx */ PacketCS packetCS;
	/*xxxx */ PingEx pingEx;
	/*xxxx */ UINT packetCounter;


	void SendSystemMessage(UINT msgId);
	void SendSystemMessage(const WCHAR* wMsg);
	void SendSystemMessage(WCHAR* wName, WCHAR* wMessage);
	void SendSystemMessage(const WCHAR* wName, const WCHAR* wMessage);
	void SendSystemMessage(CSystemMessage& msg);
	void SendSystemMessage_d(UINT msgId, INT32 param);
	void SendSystemMessage_Q(UINT msgId, INT64 param);
	void SendSystemMessage_S(UINT msgId, const WCHAR* wParam);
	void SendSystemMessage_S_d(UINT msgId, const WCHAR* wParam, INT32 iParam);
	void SendSystemMessage_Announce(const WCHAR* wMessage);
	void GetMacAddressString(wstring& wMac);
	void ForceClose();
	bool ValidUserSocket();
};
