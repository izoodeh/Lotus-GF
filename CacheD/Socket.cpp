#include "StdAfx.h"
#include "Socket.h"

void CSocket::Send(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	
	char buff[0x4000];
	int len = VAssemble(buff, 0x4000, format, va);

	typedef void (*f)(CSocket*, const char*, int, char*);
	f(0x524F18L)(this, "b", len, buff);

	va_end(va);
}