#pragma once

//50h size
class CPledgeMap
{
	LPVOID _unkn00[16];	 //18h map
public:
	bool ExistKey(int key);
};