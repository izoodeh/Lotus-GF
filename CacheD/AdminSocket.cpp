#include "stdafx.h"
#include "AdminSocket.h"

void CAdminSocket::Send(const char *format, ...)
{
	va_list va;
	va_start(va, format);
	CHAR buff[0x4000];
	int len = VAssemble(buff, 0x4000, format, va);

	if(len > 0)
	{
		typedef void(*f)(CAdminSocket*, const char*, int, PCHAR);
		f(0x440BD8L)(this, "b", len, buff);
	}

	va_end(va);
}