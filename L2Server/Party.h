#pragma once

class CSharedPartyData;

enum ItemRoutingType
{
	IR_FINDERS_KEEPERS = 0,
	IR_RANDOM = 1,
	IR_RANDOM_INCLUDING_SPOIL = 2,
	IR_BY_TURN = 3,
	IR_BY_TURN_INCLUDING_SPOIL = 4
};

//vt: BBD518h size: 90h
class CParty : public CObject
{
public:
	/* 40 */ CYieldLock *pLock;
	/* 48 */ CSharedPartyData* pSD;
	/* 50 */ LPVOID _unkn50;
	/* 58 */ UINT _unkn58;
	/* 5C */ UINT memberId[9];
	/* 80 */ UINT indexByOrder;
	/* 84 */ UINT memberCount;
	/* 88 */ ItemRoutingType itemRoutingType;
	/* 8C */ INT32 _unkn8c;

	UserVector GetAllMembers();
	void GetAllMembersEx(UserVector& members, bool useLock = true);
	void GetAllMembersInRange(UserVector& members, FVector& pos, double range);
	void Dismiss(bool param = 0);
	void Withdraw(User* pUser, bool param = false);
};