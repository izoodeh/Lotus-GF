#pragma once

class CNetwork
{
public:
	CNetwork();
	virtual ~CNetwork();
	virtual void Init();
};

extern CNetwork g_Network;