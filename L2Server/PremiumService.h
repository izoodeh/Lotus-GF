#pragma once
#include "PremiumServiceSocket.h"

enum PointType
{
	PointDonate = 0,
	PointVote = 1
};

typedef bool (*PremiumCallback)(CPremiumServiceSocket* pSocket, const unsigned char* packet);

class CPremiumService
{
	/* 00 */ LPVOID lpVT;
	/* 08 */ HANDLE hThread;
	/* 10 */ bool connected;
	/* 11 */ bool verified;
	/* 12 */ bool _unkn12;
	/* 13 */ bool _unkn13;
	/* 14 */ UINT _unkn14;
	/* 18 */ CPremiumServiceSocket *pSocket;

	static bool Bind(UINT id, PremiumCallback func);
public:
	static void Init();
	static int SendBuyItemRequest(CPremiumService* lpInstance, int userId, int productId, int count);
	static bool ReplyBuyItem(CPremiumServiceSocket* pSocket, const unsigned char* packet);
	static void RequestGetGamePoint(CPremiumService* lpInstance, UINT userId, UINT type);
	static bool ReplyGetGamePoint(CPremiumServiceSocket* pSocket, const unsigned char* packet);
	static void RequestSubGamePoint(CPremiumService* lpInstance, UINT userId, INT64 points);
	static bool ReplySubGamePoint(CPremiumServiceSocket* pSocket, const unsigned char* packet);
	static void RequestAddGamePoint(CPremiumService* lpInstance, UINT userId, INT64 points);
	static bool ReplyAddGamePoint(CPremiumServiceSocket* pSocket, const unsigned char* packet);
	static bool ReplyNetPing(CPremiumServiceSocket* pSocket, const unsigned char* packet);
};