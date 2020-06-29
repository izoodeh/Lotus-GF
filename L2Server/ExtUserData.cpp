#include "StdAfx.h"
#include "ExtUserData.h"


CExtUserData::CExtUserData()
{
	/* Initialize new stuff here */
	InitializeCriticalSection(&_lock);
	memset(chatTimestamp, 0, sizeof(chatTimestamp));
	spawnProtection = FALSE;
	activateSpawnProtection = FALSE;
	offlineShop = FALSE;
	offlineShopTime = 0;
	blockBuffEx = FALSE;
	blockBuffExPump = FALSE;
	votePoints = 0;
	donatePoints = 0;
	enterWorldTime = time(0);
	blockExp = FALSE;
	blockExpPump = FALSE;
	updateDbTick = GetTickCount() + 60000;
	currentActivatedSkillId = 0;
	currentActivatedSkillLevel = 0;
	deathTick = 0;
	bilingJob.productId = 0;
	bilingJob.count = 0;
	faction.factionId = 0;
	faction.team = TeamNone;
	autoConsumeHp.itemId = 0;
	autoConsumeHp.lastUseTick = 0;
	autoConsumeHp.pInfo = 0;
	autoConsumeMp.itemId = 0;
	autoConsumeMp.lastUseTick = 0;
	autoConsumeMp.pInfo = 0;
	autoConsumeCp.itemId = 0;
	autoConsumeCp.lastUseTick = 0;
	autoConsumeCp.pInfo = 0;
	bonusExp = 0.0;
	bonusSp = 0.0;
	bonusDrop = 0.0;
	bonusSpoil = 0.0;
	kickTick = 0;
	teleportTick = GetTickCount();
	autoLoot = FALSE;
	isItemDataLock = FALSE;
	isCrystalEnchantFail = 0;

	championshipUser.Clear();

	pvpData.points = 0;
	pvpData.lastSeasonPoints = 0;
	pvpData.transferredPoints = 0;
	pvpData.topPlayer = FALSE;
	pvpData.receivedReward = FALSE;

	dungeonUser.id = 0;
	dungeonUser.enterTime = 0;
	dungeonUser.kickoutTimer = 0;
	dungeonUser.resetTimer = 0;
	dungeonUser.canJoinInviteParty = FALSE;

	builderUser.isRecordingRange = FALSE;
	dispelAllDone = FALSE;
	unequipDone = FALSE;
	inBattleZone = FALSE;
	userInfoChanged = FALSE;
	charInfoChanged = FALSE;
	pUSR = 0;

	/*           end             */
}

CExtUserData::~CExtUserData()
{
	DeleteCriticalSection(&_lock);
}

void UserSkillReuse::Set(INT32 skillId, UINT endReuse, UINT subId, UINT reuseType)
{
	guard;
	m_Lock.Enter();
	if(reuseType)
	{
		m_Reuse[4][skillId] = endReuse;
	}else
	{
		m_Reuse[subId][skillId] = endReuse;
	}
	m_Lock.Leave();
//	g_Log.Add(CLog::Blue, "[%s] Added skill[%d] sub[%d] endReuse[%d] reuseType[%d]", __FUNCTION__, skillId, subId, endReuse, reuseType);
	unguard;
}

UINT UserSkillReuse::Get(INT32 skillId, UINT subId)
{
	guard;
	UINT reuse = 0;
	m_Lock.Enter();
	map<INT32, UINT>::iterator it = m_Reuse[4].find(skillId);
	if(it!=m_Reuse[4].end())
	{
		reuse = it->second;
	}else if(subId < 4)
	{
		it = m_Reuse[subId].find(skillId);
		if(it!=m_Reuse[subId].end())
		{
			reuse = it->second;
		}
	}
	m_Lock.Leave();
	unguard;
	return reuse;
}