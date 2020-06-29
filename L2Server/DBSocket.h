#pragma once
#include "IOSocket.h"

//D0h size
class CDBSocket : public CIOSocketEx
{
public:
	void SendV(const char* format, va_list va);
};