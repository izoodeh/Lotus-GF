#include "StdAfx.h"
#include "Socket.h"

void CSocket::SetAddress(struct in_addr addr)
{
	typedef void (*f)(CSocket*, struct in_addr);
	f(0x8589ACL)(this, addr);
}

struct in_addr CSocket::GetAddress( struct in_addr& addr )
{
	typedef struct in_addr (*f)(CSocket*,  struct in_addr& );
	return f(0x858A14L)(this, addr);
}

void CSocket::Send(const char *format, ...)
{
	va_list va;
	va_start(va, format);

	typedef void (*f)(CSocket*, const char*, va_list);
	f(0x859934L)(this, format, va);

	va_end(va);
}

void CSocket::SendV(const char* format, va_list va)
{
	typedef void (*f)(CSocket*, const char*, va_list);
	f(0x859934L)(this, format, va);
}

CUserSocket* CSocket::CastUserSocket()
{
	try
	{
		if(this)
		{
			if((*(LPDWORD)this) == 0xC746B8 )
			{
				return (CUserSocket*)this;
			}
		}
	}catch(...)
	{

	}
	return 0;
}