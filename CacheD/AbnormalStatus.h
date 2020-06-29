#pragma once
#include "Socket.h"

class CAbnormalStatus
{
public:
	static bool RequestSaveAbnormalStatus(CSocket* pSocket, const unsigned char* packet);
	static bool RequestLoadAbnormalStatus(CSocket* pSocket, const unsigned char* packet);
};