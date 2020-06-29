#include "stdafx.h"
#include "PremiumServiceSocket.h"

void CPremiumServiceSocket::Send(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	typedef void(*f)(CPremiumServiceSocket*, const char*, va_list);
	f(0x7EB1A8L)(this, format, va);
	va_end(va);
}