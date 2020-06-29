#pragma once

class CCreature;
class User;
class CNpc;
class CSummon;
class CPet;
class CBoss;
class CZZoldagu;
class CParty;
class CPledge;
class CWorld;
class CSocket;
class CUserSocket;
class CSkillInfo;
class CItem;
class CPCTrap;
class CEventMatch;

static const DWORD VT_USER_SOCKET = 0xC746B8;
static const DWORD VT_CCREATURE = 0xAC87E8;
static const DWORD VT_CNPC = 0xB93748;
static const DWORD VT_CCOLLECTION = 0xAB8EC8;
static const DWORD VT_CDOPPELGANGER = 0xB1F278;
static const DWORD VT_CSUMMON = 0xBCB0F8;
static const DWORD VT_CPET = 0xBCC828;
static const DWORD VT_CTELEPORTER = 0xC317D8;
static const DWORD VT_CTRAP = 0xC38B48;
static const DWORD VT_CPCTRAP = 0xC396C8;
static const DWORD VT_CWORLD_TRAP = 0xC3B568;
static const DWORD VT_CBOSS = 0xA72FB8;
static const DWORD VT_CZZOLDAGU = 0xC8C148;
static const DWORD VT_CMERCHANT = 0xB7CB18;
static const DWORD VT_USER = 0xC53BB8;
static const DWORD VT_CITEM = 0xB61DB8;
static const DWORD VT_CACCESSARY = 0xB64898;
static const DWORD VT_CASSET = 0xB64DA8;
static const DWORD VT_CARMOR = 0xB652B8;
static const DWORD VT_CETCITEM = 0xB657D8;
static const DWORD VT_CQUESTITEM = 0xB65CF8;
static const DWORD VT_CWEAPON = 0xB66208;

#define VALIDATE_TARGET_TIMER 10100
#define VALIDATE_AUTO_HP_TIMER 10101
#define VALIDATE_AUTO_MP_TIMER 10102
#define VALIDATE_AUTO_CP_TIMER 10103
#define ITEM_DECAY_TIMER 10107
#define DEACTIVATE_SPAWN_PROTECTION_TIMER 10108


//size 10h    vTable: BA8EC8h
class MemoryObject
{
public:
/* 08 */	int refCount;
/* 0C */	int inTimer;
/* 10 */	int identity;
/* 14 */	int deleteCount; //?

/* 00 */	virtual ~MemoryObject() = 0;
/* 08 */	virtual MemoryObject* AddRef(const char* file, int line, int nWorkType) = 0;
/* 10 */ 	virtual void Release(const char* file, int line, int nWorkType, bool flag) = 0;
/* 18 */	virtual void _NullSub18() = 0;

	bool Valid(DWORD vTable);
};

//vTable: B5BFC8h
class CBaseObject : public MemoryObject
{
public:
	bool AddTimer(INT64 delay, INT32 id);
};

//Size 0x20, VT: 0xB6C4D8
class CShutDownMessage : public CBaseObject
{
public:
	/* 18 */ INT32 shutdownTime;
	/* 1C */ INT32 _unkn1C;
};