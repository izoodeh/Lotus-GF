#pragma once
#pragma pack( push, 1 )

//size: 0xF8
class CSharedSSQInfo
{
public:
	/* 00 */ UINT _unkn00;
	/* 04 */ UINT status; /* 0 - none, 1 - event, 2 - accounting, 3 - seal_effect*/
	/* 08 */ LPVOID _unkn08[30];
};


#pragma pack( pop )