#include "StdAfx.h"
#include "IPAddress.h"

CIPAddress::CIPAddress(void)
{
	memset(p, 0, sizeof(p));
}

CIPAddress::CIPAddress(const CIPAddress& other)
{
	memcpy(p, other.p, sizeof(p));
}

CIPAddress::CIPAddress(string sIp)
{
	sIp = ReplaceChar(sIp, ".", " ");
	sIp = ReplaceChar(sIp, "*", "65535");

	stringstream str(sIp);

	str >> p[0] >> p[1] >> p[2] >> p[3];
}

CIPAddress::CIPAddress(wstring wIp)
{
	wIp = ReplaceChar(wIp, L".", L" ");
	wIp = ReplaceChar(wIp, L"*", L"65535");

	wstringstream str(wIp);

	str >> p[0] >> p[1] >> p[2] >> p[3];
}

CIPAddress::CIPAddress(sockaddr_in addr)
{
	p[0] = addr.sin_addr.S_un.S_un_b.s_b1;
	p[1] = addr.sin_addr.S_un.S_un_b.s_b2;
	p[2] = addr.sin_addr.S_un.S_un_b.s_b3;
	p[3] = addr.sin_addr.S_un.S_un_b.s_b4;
}

CIPAddress::CIPAddress(in_addr addr)
{
	p[0] = addr.S_un.S_un_b.s_b1;
	p[1] = addr.S_un.S_un_b.s_b2;
	p[2] = addr.S_un.S_un_b.s_b3;
	p[3] = addr.S_un.S_un_b.s_b4;
}

bool CIPAddress::operator==(const CIPAddress &other) const
{
	for(int n = 0; n < 4; n++)
	{
		if(p[n] != other.p[n] && p[n] != 65535 && other.p[n] != 65535)
		{
			return false;
		}
	}

	return true;
};

bool CIPAddress::operator < (const CIPAddress &other) const
{
	for(int n = 0; n < 4; n++)
	{
		if(p[n] < other.p[n])
		{
			return true;
		}else if( p[n] > other.p[n])
		{
			return false;
		}
	}

	return false;
};

bool CIPAddress::operator!=(const CIPAddress &other) const
{
	return !((*this) == other);
};

string CIPAddress::ToString()
{
	stringstream sip;
	for(int n=0;n<4;n++)
	{
		if( p[n] != 65535 )
		{
			sip << p[n];
		}else
		{
			sip << "*";
		}
		if(n!=3)
		{
			sip << ".";
		}
	}
	
	return sip.str();
}

wstring CIPAddress::ToWString()
{
	wstringstream sip;
	for(int n=0;n<4;n++)
	{
		if( p[n] != 65535 )
		{
			sip << p[n];
		}else
		{
			sip << L"*";
		}
		if(n!=3)
		{
			sip << L".";
		}
	}
	
	return sip.str();
}