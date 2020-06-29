#pragma once
#include "UserSocket.h"

class CUserSocketReleaseVerifier : public CBaseObject
{
public:
	/* 18 */ CUserSocket* pSocket;
	/* 20 */ const WCHAR* wStr;
};