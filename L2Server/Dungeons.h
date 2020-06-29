#pragma once

struct DungeonData
{
	UINT id;
	BOOL enabled;
	double range[3][2];	//[X/Y/Z][Min/Max]
	BOOL partyRequired;
	IVector entryPos;
	IVector kickOutPos;
	UINT minPartyMembers;
	UINT maxPartyMembers;
	INT32 requiredItemId;
	INT32 requiredItemCount;
	INT32 timeLimit;
	BOOL canJoinInviteParty;
};

class CDungeons : public CFeature
{
	BOOL m_enabled;
	map<UINT, DungeonData> m_dungeons;

	void LoadData();
	void KickOutUser(User *pUser, DungeonData& data);
public:
	CDungeons();
	~CDungeons();
	void Init();
	void TimerExpired(User *pUser);
	void RequestEnter(User *pUser, UINT dungeonId);
};

extern CDungeons g_DungeonSystem;