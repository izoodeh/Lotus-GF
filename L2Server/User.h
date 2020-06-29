#pragma once
#include "Creature.h"
#include "SchemeBuffList.h"
#include "OlympiadUser.h"
#include "ExtUserData.h"

struct HTMLCmdData
{
	INT64 noIdeaYet;
	INT64 noIdeaYet2;
};

//vTable: C53BB8h size: 3A10
class User : public CCreature
{
public:
	/* 1CB0 */ LPVOID _unkn1CB0[10];	//1cb0 - UINT brAve flag
	/* 1D00 */ LPVOID _unkn1D00[32];
	/* 1E00 */ LPVOID _unkn1E00[32];
	/* 1F00 */ LPVOID _unkn1F00[32];
	/* 2000 */ LPVOID _unkn2000[32];
	/* 2100 */ LPVOID _unkn2100[32];
	/* 2200 */ LPVOID _unkn2200[27];
	
	/* 22D8 */ bool _align22D8;
	/* 22D9 */ bool soulshotInUse;
	/* 22DA */ bool spiritShotInUse;
	/* 22DB */ bool summonSoulshotInUse;
	/* 22DC */ bool summonSpiritshotInUse;

	/* 22DD */ bool _align22DD[3];
	/* 22E0 */ LPVOID _unkn22E0[4];

	/* 2300 */ LPVOID _unkn2300[13];	//23E4 - DWORD chatBanTimestamp;
	/* 2368 */ BYTE checkBotFlag;
	/* 2369 */ BYTE _unkn2369[7];
	/* 2370 */ LPVOID _unkn2370[9];
	/* 23B8 */ UINT _unkn23B8;
	/* 23BC */ bool isEnchantingItem;
	/* 23BD */ bool isEnchantingAttribute;
	/* 23BE */ BYTE _align23BD[2];
	/* 23C0 */ LPVOID _unkn23C0[2];
	/* 23D0 */ UINT pledgeServerId;
	/* 23D4 */ UINT _unkn23D4;
	/* 23D8 */ LPVOID _unkn23D8;
	/* 23E0 */ DWORD _unkn23E0;
	/* 23E4 */ DWORD stopSayTimeoutTick;
	/* 23E8 */ LPVOID _unkn23E8[3];
	
	/* 2400 */ LPVOID _unkn2400[32];
	/* 2500 */ LPVOID _unkn2500[32];
	/* 2600 */ LPVOID _unkn2600[32];
	/* 2700 */ LPVOID _unkn2700[32];
	/* 2800 */ LPVOID _unkn2800[32];
	/* 2900 */ LPVOID _unkn2900[32];
	/* 2A00 */ LPVOID _unkn2A00[32];
	/* 2B00 */ LPVOID _unkn2B00[32];
	/* 2C00 */ LPVOID _unkn2C00[32];
	/* 2D00 */ LPVOID _unkn2D00[32];
	/* 2E00 */ LPVOID _unkn2E00[32];
	/* 2F00 */ LPVOID _unkn2F00[32];
	/* 3000 */ LPVOID _unkn3000[32];
	/* 3100 */ LPVOID _unkn3100[32];
	/* 3200 */ LPVOID _unkn3200[12];
	/* 3260 */ INT32 _unkn3260;
	/* 3264 */ CYieldLock expectedHtmlReplyLock;
	/* 3270 */ bool expectingHtmlReply;
	/* 3271 */ bool _align3271[7];
	/* 3278 */ vector<HTMLCmdData> vExpectedHtmlReply;
	/* 3298 */ COlympiadUser olympiadUser;
	/* 32E8 */ LPVOID _unkn32E8[3];
	/* 3300 */ LPVOID _unkn3300[11];
	/* 3358 */ CUserSocket *pUserSocket;
	/* 3360 */ LPVOID _unkn3360[20];	//3360 - tradeId;
	/* 3400 */ LPVOID _unkn3400[32];
	/* 3500 */ LPVOID _unkn3500[27];
	
	/* 35D8 */ bool gmWhisper;
	/* 35D9 */ bool _align35d9[7];
	
	/* 35E0 */ LPVOID _unkn35E0[4];

	/* 3600 */ LPVOID _unkn3600[32];
	/* 3700 */ LPVOID _unkn3700[32];
	/* 3800 */ LPVOID _unkn3800[32];
	/* 3900 */ LPVOID _unkn3900[4];
	/* 3920 */ double enchantRateBonus;
	/* 3928 */ LPVOID _unkn3928[27];
	/* 3A00 */ LPVOID _unkn3A00;
	/* 3A08 */ INT64 donatePoints;
	/* 3A10 */ CExtUserData *pEUD;

	void ActionFailed();
	bool BasicAction(int id, bool ctrl, bool shift, UINT stopType);
	void BlockTarget(UINT serverId);
	bool IsBlockedTarget(UINT serverId);
	bool IsNowTrade();
	bool ValidUser();
	bool CanUseItem(CItem *pItem);
	bool CheckMyPledgeDeclareWar(User *pOther);
	bool DeleteItemInInventory(int classId, INT64 count, AtomicJob *pJob = 0);
	void DeleteItemInInventory(int classId);
	void DespawnPet(bool param = true, bool param2 = false, UINT despawnJob = 0);
	void EnchantCancel();
	void EndBuyProduct();
	CAlliance* GetAlliance();
	CParty* GetParty();
	CItem* GetPendingUseETCItem( PendingUseItemType type );
	UINT GetPartyMemberCount(FVector& pos, double range = 1500.0, bool diffIp = true, bool diffMac = true);	//pos - base point, range - max distance
	CPledge* GetPledge();
	CSummon* GetSummonOrPet();
	CTrade* GetTrade();
	static User* GetUser(LPUINT lpTargetId);
	void Kick();
	void StoreAbnormal(AbnormalData& ad, time_t expireTime);
	bool RestoreAbnormal(StoredAbnormalData& sad);
	void StoredAbnormalTimerExpired();
	void ResetStoredAbnormalTimes();
	void DeleteStoredAbnormals();
	bool StoreRandomBuffs(CCreature *pOther, CSkillInfo *pSI, UINT chance, UINT count);

	bool IsPartyMaster();
	void PlaySound(const WCHAR* wName);
	void ProcessMyParty(bool param = false);
	bool RideForEvent(INT32 npcClassId = 1012621, INT32 timeInSec = 600, INT32 param = 0);
	bool RideWyvern(INT32 npcClassId = 1012621);
	void SendAbnormalStatusInfo();
	void SendDeleteSkillToDB(int skillId);
	void SendUserInfo(CUserSocket* pGMSocket = 0);
	void SendCharInfo(CUserSocket* pSocket = 0, bool param = false);
	void SetKarma(int value);
	void SendQuestList(CUserSocket* pSocket = 0);
	void SetPendingUseETCItem(CItem *pItem, PendingUseItemType type);
	void SetSpelledInfoChagned();
	void SetStopSay(UINT timeInMs, bool saveInDB = false);
	void ShowHTML(const WCHAR* fileName, const WCHAR* wContent, int objectType = 0);
	void ShowQuestMark(int id);
	void SocialAction(int actionType, int param2 = 0 , bool param3 = false);
	bool TeleportToLocation(int x, int y, int z, UINT paramEx = 0x10000);
	void TradeCancel();
	SlotType UnequipItem(CItem *pItem);
	bool UseItem(CItem *pItem, UINT param1 = 0, bool param2 = 0);
	void QuitPrivateStore();
	void SetLevel(int level);
	void SetPvPFlag();
	void SendHome(bool withKarma);
	void SetGuilty(bool value);
	void SetUserInfoChanged();
	void SetCharInfoChanged();
	void ValidateAcquiredSkills();
};