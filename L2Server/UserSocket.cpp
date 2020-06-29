#include "StdAfx.h"
#include "UserSocket.h"

void CUserSocket::ForceClose()
{
	typedef void(*f)(CUserSocket*);
	f(0x9297B8L)(this);
}

void CUserSocket::SendSystemMessage(UINT msgId)
{
	if(this)
	{
		typedef void(*f)(CUserSocket*, UINT);
		f(0x40D10CL)(this, msgId);
	}
}

void CUserSocket::SendSystemMessage_d(UINT msgId, INT32 param)
{
	if(this)
	{
		typedef void(*f)(CUserSocket*, UINT, INT32);
		f(0x436E18L)(this, msgId, param);
	}
}

void CUserSocket::SendSystemMessage_S(UINT msgId, const WCHAR* wParam)
{
	if(this && wParam)
	{
		typedef void(*f)(CUserSocket*, UINT, const WCHAR*);
		f(0x4867B4L)(this, msgId, wParam);
	}
}

void CUserSocket::SendSystemMessage_Announce(const WCHAR* wMessage)
{
	if(this && wMessage)
	{
		typedef void(*f)(CUserSocket*, const WCHAR*);
		f(0x5B34D0L)(this, wMessage);
	}
}

void CUserSocket::SendSystemMessage_Q(UINT msgId, INT64 param)
{
	if(this)
	{
		typedef void(*f)(CUserSocket*, UINT, INT64);
		f(0x6C09D4L)(this, msgId, param);
	}
}

void CUserSocket::SendSystemMessage_S_d(UINT msgId, const WCHAR* wParam, INT32 iParam)
{
	if(this && wParam)
	{
		typedef void(*f)(CUserSocket*, UINT, const WCHAR*, INT32);
		f(0x7C83A4L)(this, msgId, wParam, iParam);
	}
}

void CUserSocket::SendSystemMessage(WCHAR *wName, WCHAR* wMessage)
{
	if(this && wName && wMessage)
	{
		typedef void(*f)(CUserSocket*, WCHAR*, WCHAR*);
		f(0x9244F0L)(this, wName, wMessage);
	}
}

void CUserSocket::SendSystemMessage(const WCHAR* wMsg)
{
	if(this && wMsg)
	{
		Send("cdddS", 0x62, 1983, 1, 0, wMsg);
	}
}

void CUserSocket::SendSystemMessage(const WCHAR *wName,const WCHAR* wMessage)
{
	if(this && wName && wMessage)
	{
		typedef void(*f)(CUserSocket*, const WCHAR*, const WCHAR*);
		f(0x9244F0L)(this, wName, wMessage);
	}
}

void CUserSocket::SendSystemMessage(CSystemMessage& msg)
{
	if(this)
	{
		Send("b", msg.GetSize(), msg.GetBuff());
	}
}

void CUserSocket::GetMacAddressString(wstring& wMac)
{
	typedef void (*f)(CUserSocket*, wstring&);
	f(0x92E16CL)(this, wMac);
}

bool CUserSocket::ValidUserSocket()
{
	return Valid(VT_USER_SOCKET);
}