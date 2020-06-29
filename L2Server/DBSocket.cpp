#include "StdAfx.h"
#include "DBSocket.h"

void CDBSocket::SendV(const char *format, va_list va)
{
	typedef void(*f)(CDBSocket*, const char*, va_list);
	f(0x5D1660L)(this, format, va);
}
