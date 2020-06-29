#pragma once

class CIPAddress
{
	WORD p[4];
public:
	CIPAddress();
	CIPAddress(const CIPAddress& other);
	CIPAddress(string ip);
	CIPAddress(wstring ip);
	CIPAddress(UINT ip);
	CIPAddress(sockaddr_in addr);
	CIPAddress(in_addr addr);
	string ToString();
	wstring ToWString();
	bool operator == (const CIPAddress &other) const;
	bool operator != (const CIPAddress &other) const;
	bool operator < (const CIPAddress &other) const;

};