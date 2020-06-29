#pragma once

class COlympiadUser
{
public:
	/* 00 */ UINT status;	/* 4 - registered , 3 - observer,  1 and 2 - fighting , 0 none*/
	/* 04 */ UINT _unkn04;
	/* 08 */ User *pUser;
	/* 10 */ FVector returnPos;
	/* 28 */ FVector startPos;
	/* 40 */ GameRuleType gameRule;	//3 by default
	/* 44 */ UINT fieldId;
	/* 48 */ UINT _unkn48;
	/* 4C */ bool alive;
	/* 4D */ bool _align4D[3];

	bool IsInside();
	void OnLeaveWorld();
};