#ifndef _XWSTRING_
#define _XWSTRING_ 1

#include <stdlib.h>
#include "yywclex.h"

class xwstring {
public:
	xwstring() { this->len = 0; this->maxlen = 7; this->s.name[0] = 0; this->s.ptr = NULL; };
	xwstring(wchar_t* str); // constr declared in the CParserForSkill.cpp
	~xwstring() {
		if (this->maxlen > 7) {
			//got crash when tried to call free(this->s.ptr);
		}
		this->s.ptr = 0;
		this->len = 0;
		this->maxlen = 7;
	};
	wchar_t* get(void) { return this->maxlen == 7 ? this->s.name : this->s.ptr; };
	operator wchar_t* () { return this->get(); };
    void copy(xwstring*, __int64 pos = 0, __int64 size = -1);
	xwstring* operator = (xwstring* src) { this->copy(src); };
	xwstring(xwstring* src) { this->copy(src); };
//--------------------------------------------------------------------------
	/* 0x00 */	yl::yywflexer* pLexer;
	/* 0x08 */
		union _u_str {
			wchar_t *ptr;
			wchar_t	name[8]; // or pointer to something ?
		} s;
	/* 0x18 */	__int64	len;
	/* 0x20 */	__int64	maxlen;
}; // sizeof = 0x28

#endif // _XWSTRING_
