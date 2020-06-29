#pragma once
#include "StaticObject.h"

class CEventMatch : public CStaticObject
{
public:
	/* 50 */ LPVOID _unkn50;
	/* 58 */ LPVOID _unkin58;
	/* 60 */ LPVOID _unkn60;
	/* 68 */ UINT matchRule;
	/* 6C */ UINT _unkn6C;
	/* 70 */ LPVOID _unkn70[15];
	/* E8 */ UINT fanceState; /* 0 - none, 1 - column, 2 - wall */
	/* EC */ UINT fanceHeight;
	/* F0 */ UINT fanceWidth;
};