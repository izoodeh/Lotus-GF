#include "stdafx.h"
#include "xwstring.h"

//-----------------------------------------------------------------
xwstring::xwstring(wchar_t* str)
{
	typedef xwstring* (*t)(xwstring* p,wchar_t* str);
	t constr = (t)0x40DFF0;
	constr(this, str);
}

void xwstring::copy(xwstring* src, __int64 pos, __int64 size)
{
	typedef void (*t)(xwstring* dst, xwstring* src,__int64 pos, __int64 size);
	t f = (t)0x40BDF0;
	f(this, src, pos, size);
}
