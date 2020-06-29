#pragma once


enum GlobalDominionState
{
	GlobalDominionFinishedSiege = 0,
	GlobalDominionCastleSiege = 1,
	GlobalDominionRegistration = 2,
	GlobalDominionStandBy = 3,
	GlobalDominionSiege = 4
};

enum DomionionState
{
	DominionNone = 0,
	DominionSiegeDeclared = 1,
	DominionExtended = 2
};

enum DominionSiegeState
{
	DominionSiegeFinished = 0,
	DominionSiegeRegistration = 1,
	DominionSiegeStandBy = 2,
	DominionSiegeFight = 5,
	DominionSiegeDone = 6
};

enum TerritoryType
{
	TerritoryNone = 0,
	TerritoryDispelAll = 1,
	TerritoryPvPReward = 2,
	TerritoryBlockItem = 3,
	TerritoryBattleZone = 4,
	TerritoryDispelAbnormal = 5,
	TerritoryCount = 6
};

namespace Network
{
	namespace OutgoingPacket
	{
		enum CustomPacket
		{
			BaseId = 0x8D,
			AntiBot = 0x01,
			HardwareId = 0x02
		};
	};
	namespace IncommingPacket
	{
		enum CustomPacket
		{
			BaseId = 0xA,
			AntiBot = 0x01,
			HardwareId = 0x02
		};
	};
};

enum DuelType
{
	DuelSolo = 0,
	DuelParty = 1
};

enum TeamType
{
	TeamNone = 0,
	TeamBlue = 1,
	TeamRed = 2
};

enum EtcSkillAcquireType
{
	EtcSkillAcquireFishing = 1,
	EtcSkillAcquirePledge = 2,
	EtcSkillAcquireSubPledge = 3,
	EtcSkillAcquireTransform = 4,
	EtcSkillAcquireSubJob = 5,
	EtcSkillAcquireCollect = 6
};

enum SSQ_STATUS
{
	SSQ_NONE = 0,
	SSQ_EVENT = 1,
	SSQ_ACCOUNTING = 2,
	SSQ_SEAL_EFFECT = 3
};

enum CRYSTAL_TYPE
{
	CRYSTAL_NONE = 0,
	CRYSTAL_D = 1,
	CRYSTAL_C = 2,
	CRYSTAL_B = 3,
	CRYSTAL_A = 4,
	CRYSTAL_S = 5,
	CRYSTAL_S80 = 6,
	CRYSTAL_S84 = 7,
	CRYSTAL_EVENT = 8,
	CRYSTAL_FREE = 9,
	CRYSTAL_TYPE_COUNT = 10
};

enum SlotType
{
	SlotNone = 0,
	SlotUnderwear = 1,
	SlotRightEar = 2,
	SlotLeftEar = 4,
	SlotNeck = 8,
	SlotRightFinger = 16,
	SlotLeftFinger = 32,
	SlotHead = 64,
	SlotRightHand = 128,
	SlotLeftHand = 256,
	SlotGloves = 512,
	SlotChest = 1024,
	SlotLegs = 2048,
	SlotFeet = 4096,
	SlotBack = 8192,
	SlotRightLeftHand = 16384,
	SlotLeftRightHand = 16384,
	SlotOnePiece = 32768,
	SlotHair = 65536,
	SlotAllDress = 131072,
	SlotHairEx = 262144,
	SlotHairAll = 524288,
	SlotRightBracelet = 1048576,
	SlotLeftBracelet = 2097152,
	SlotDeco1 = 4194304,
	SlotDeco2 = 8388608,
	SlotDeco3 = 16777216,
	SlotDeco4 = 33554432,
	SlotDeco5 = 67108864,
	SlotDeco6 = 134217728,
	SlotWaist = 268435456
};

enum UPDATE_STAT
{
	STAT_HP = 9,
	STAT_MP = 11,
	STAT_HPnCP = 26,
	STAT_CP = 33
};

enum CONSUME_TYPE
{
	CONSUME_NORMAL = 0,
	CONSUME_CHARGE = 1,
	CONSUME_STACKABLE = 2,
	CONSUME_ASSET = 3
};

enum ItemType
{
	ItemWeapon = 0,
	ItemArmor = 1,
	ItemAccessary = 2,
	ItemQuest = 3,
	ItemAsset = 4,
	ItemETC = 5
};

enum OPERATE_TYPE
{
	OPERATE_A1 = 0,
	OPERATE_A2 = 1,
	OPERATE_A3 = 2,
	OPERATE_P = 3,
	OPERATE_T = 4,
	OPERATE_CA1 = 5,
	OPERATE_CA2 = 6,
	OPERATE_CA5 = 7,
	OPERATE_DA1 = 8,
	OPERATE_DA2 = 9
};

enum BASIC_PROPERTY
{
	BP_STR = 1,
	BP_DEX = 2,
	BP_CON = 3,
	BP_INT = 4,
	BP_MEN = 5,
	BP_WIT = 6,
	BP_NONE = 33
};

enum PendingUseItemType
{
	PUI_SCROLL = 0,
	PUI_MPCC = 1,
	PUI_ATTRIBUTE = 2,
	PUI_ENCHANT_BONUS = 3
};

enum RestartPoint
{
	RP_VILLAGE = 0,
	RP_CLANHALL = 1,
	RP_CASTLE = 2,
	RP_FORTRESS = 3,
	RP_SIEGEHQ = 4,
	RP_FESTIVAL = 5
};

enum SocialActionType
{
	SAT_GREET = 2,
	SAT_VICTORY = 3,
	SAT_ADVANCE = 4,
	SAT_NO = 5,
	SAT_YES = 6,
	SAT_BOW = 7,
	SAT_UNAWARE = 8,
	SAT_WAITINGA = 9,
	SAT_LAUGH = 10,
	SAT_APPLAUS = 11,
	SAT_DANCE = 12,
	SAT_SAD = 13,
	SAT_LEVEL_UP = 15,
	SAT_HERO = 16,
	SAT_CURSED_WEAPON_LEVEL_UP = 17
};

enum GameRuleType
{
	GR_NONE = 3,
	GR_CLASS = 2,
	GR_CLASS_FREE = 1,
	GR_TEAM = 0
};

enum SayType
{
	SAY_ALL = 0,
	SAY_SHOUT = 1, //!
	SAY_TELL = 2,
	SAY_PARTY = 3, //#
	SAY_CLAN = 4,  //@
	SAY_GM = 5,
	SAY_PETITION_PLAYER = 6, // used for petition
	SAY_PETITION_GM = 7, //* used for petition
	SAY_TRADE = 8, //+
	SAY_ALLIANCE = 9, //$
	SAY_ANNOUNCEMENT = 10,
	SAY_PARTYROOM_ALL = 16, //(Red)
	SAY_PARTYROOM_COMMANDER = 15, //(Yellow)
	SAY_HERO_VOICE = 17
};

enum AttackType
{
	AT_NONE = 0,
	AT_SWORD = 1,
	AT_BLUNT = 2,
	AT_DAGGER = 3,
	AT_POLE = 4,
	AT_FIST = 5,
	AT_BOW = 6,
	AT_ETC = 7,
	AT_DUAL = 8,
	AT_DUALFIST = 9,
	AT_FISHINGROD = 10,
	AT_RAPIER = 11,
	AT_CROSSBOW = 12,
	AT_ANCIENTSWORD = 13
};

enum WeaponType
{
	WeaponNone = 0,
	WeaponSword = 1,
	WeaponBlunt = 2,
	WeaponDagger = 3,
	WeaponPole = 4,
	WeaponFist = 5,
	WeaponBow = 6,
	WeaponEtc = 7,
	WeaponDual = 8,
	WeaponDualFist = 9,
	WeaponFishingRod = 10,
	WeaponRapier = 11,
	WeaponCrossbow = 12,
	WeaponAncientSword = 13,
	WeaponFlag = 14,
	WeaponDualDagger = 15
};

enum AbnormalStatusSlotType
{
	AB_NONE = -1,
	AB_AB_HAWK_EYE = 0,
	AB_ALL_ATTACK_DOWN = 1,
	AB_ALL_ATTACK_UP = 2,
	AB_ALL_SPEED_DOWN = 3,
	AB_ALL_SPEED_UP = 4,
	AB_ANTARAS_DEBUFF = 5,
	AB_ARMOR_EARTH = 6,
	AB_ARMOR_FIRE = 7,
	AB_ARMOR_HOLY = 8,
	AB_ARMOR_UNHOLY = 9,
	AB_ARMOR_WATER = 10,
	AB_ARMOR_WIND = 11,
	AB_ATTACK_SPEED_UP_BOW = 12,
	AB_ATTACK_TIME_DOWN = 13,
	AB_ATTACK_TIME_UP = 14,
	AB_AVOID_DOWN = 15,
	AB_AVOID_UP = 16,
	AB_AVOID_UP_SPECIAL = 17,
	AB_BERSERKER = 18,
	AB_BIG_BODY = 19,
	AB_BIG_HEAD = 20,
	AB_BLEEDING = 21,
	AB_BOW_RANGE_UP = 22,
	AB_BUFF_QUEEN_OF_CAT = 23,
	AB_BUFF_UNICORN_SERAPHIM = 24,
	AB_CANCEL_PROB_DOWN = 25,
	AB_CASTING_TIME_DOWN = 26,
	AB_CASTING_TIME_UP = 27,
	AB_CHEAP_MAGIC = 28,
	AB_CRITICAL_DMG_DOWN = 29,
	AB_CRITICAL_DMG_UP = 30,
	AB_CRITICAL_PROB_DOWN = 31,
	AB_CRITICAL_PROB_UP = 32,
	AB_DANCE_OF_AQUA_GUARD = 33,
	AB_DANCE_OF_CONCENTRATION = 34,
	AB_DANCE_OF_EARTH_GUARD = 35,
	AB_DANCE_OF_FIRE = 36,
	AB_DANCE_OF_FURY = 37,
	AB_DANCE_OF_INSPIRATION = 38,
	AB_DANCE_OF_LIGHT = 39,
	AB_DANCE_OF_MYSTIC = 40,
	AB_DANCE_OF_PROTECTION = 41,
	AB_DANCE_OF_SHADOW = 42,
	AB_DANCE_OF_SIREN = 43,
	AB_DANCE_OF_VAMPIRE = 44,
	AB_DANCE_OF_WARRIOR = 45,
	AB_DEBUFF_NIGHTSHADE = 46,
	AB_DEBUFF_SHIELD = 47,
	AB_DECREASE_WEIGHT_PENALTY = 48,
	AB_DERANGEMENT = 49,
	AB_DETECT_WEAKNESS = 50,
	AB_DMG_SHIELD = 51,
	AB_DOT_ATTR = 52,
	AB_DOT_MP = 53,
	AB_DRAGON_BREATH = 54,
	AB_DUELIST_SPIRIT = 55,
	AB_FATAL_POISON = 56,
	AB_FISHING_MASTERY_DOWN = 57,
	AB_FLY_AWAY = 58,
	AB_FOCUS_DAGGER = 59,
	AB_HEAL_EFFECT_DOWN = 60,
	AB_HEAL_EFFECT_UP = 61,
	AB_HERO_BUFF = 62,
	AB_HERO_DEBUFF = 63,
	AB_HIT_DOWN = 64,
	AB_HIT_UP = 65,
	AB_HOLY_ATTACK = 66,
	AB_HP_RECOVER = 67,
	AB_HP_REGEN_DOWN = 68,
	AB_HP_REGEN_UP = 69,
	AB_LIFE_FORCE_ORC = 70,
	AB_LIFE_FORCE_OTHERS = 71,
	AB_MAGIC_CRITICAL_UP = 72,
	AB_MAJESTY = 73,
	AB_MAX_BREATH_UP = 74,
	AB_MAX_HP_DOWN = 75,
	AB_MAX_HP_UP = 76,
	AB_MAX_MP_UP = 77,
	AB_MA_DOWN = 78,
	AB_MA_UP = 79,
	AB_MA_UP_HERB = 80,
	AB_MD_DOWN = 81,
	AB_MD_UP = 82,
	AB_MD_UP_ATTR = 83,
	AB_MIGHT_MORTAL = 84,
	AB_MP_COST_DOWN = 85,
	AB_MP_COST_UP = 86,
	AB_MP_RECOVER = 87,
	AB_MP_REGEN_UP = 88,
	AB_MULTI_BUFF = 89,
	AB_MULTI_DEBUFF = 90,
	AB_PARALYZE = 91,
	AB_PA_DOWN = 92,
	AB_PA_PD_UP = 93,
	AB_PA_UP = 94,
	AB_PA_UP_HERB = 95,
	AB_PA_UP_SPECIAL = 96,
	AB_PD_DOWN = 97,
	AB_PD_UP = 98,
	AB_PD_UP_BOW = 99,
	AB_PD_UP_SPECIAL = 100,
	AB_PINCH = 101,
	AB_POISON = 102,
	AB_POLEARM_ATTACK = 103,
	AB_POSSESSION = 104,
	AB_PRESERVE_ABNORMAL = 105,
	AB_PUBLIC_SLOT = 106,
	AB_RAGE_MIGHT = 107,
	AB_REDUCE_DROP_PENALTY = 108,
	AB_REFLECT_ABNORMAL = 109,
	AB_RESIST_BLEEDING = 110,
	AB_RESIST_DEBUFF_DISPEL = 111,
	AB_RESIST_DERANGEMENT = 112,
	AB_RESIST_HOLY_UNHOLY = 113,
	AB_RESIST_POISON = 114,
	AB_RESIST_SHOCK = 115,
	AB_RESIST_SPIRITLESS = 116,
	AB_REUSE_DELAY_DOWN = 117,
	AB_REUSE_DELAY_UP = 118,
	AB_ROOT_PHYSICALLY = 119,
	AB_ROOT_MAGICALLY = 120,
	AB_SHIELD_DEFENCE_UP = 121,
	AB_SHIELD_PROB_UP = 122,
	AB_SILENCE = 123,
	AB_SILENCE_ALL = 124,
	AB_SILENCE_PHYSICAL = 125,
	AB_SLEEP = 126,
	AB_SNIPE = 127,
	AB_SONG_OF_CHAMPION = 128,
	AB_SONG_OF_EARTH = 129,
	AB_SONG_OF_FLAME_GUARD = 130,
	AB_SONG_OF_HUNTER = 131,
	AB_SONG_OF_INVOCATION = 132,
	AB_SONG_OF_LIFE = 133,
	AB_SONG_OF_MEDITATION = 134,
	AB_SONG_OF_RENEWAL = 135,
	AB_SONG_OF_STORM_GUARD = 136,
	AB_SONG_OF_VENGEANCE = 137,
	AB_SONG_OF_VITALITY = 138,
	AB_SONG_OF_WARDING = 139,
	AB_SONG_OF_WATER = 140,
	AB_SONG_OF_WIND = 141,
	AB_SPA_DISEASE_A = 142,
	AB_SPA_DISEASE_B = 143,
	AB_SPA_DISEASE_C = 144,
	AB_SPA_DISEASE_D = 145,
	AB_SPEED_DOWN = 146,
	AB_SPEED_UP = 147,
	AB_SPEED_UP_SPECIAL = 148,
	AB_SSQ_TOWN_BLESSING = 149,
	AB_SSQ_TOWN_CURSE = 150,
	AB_STEALTH = 151,
	AB_STUN = 152,
	AB_THRILL_FIGHT = 153,
	AB_TOUCH_OF_DEATH = 154,
	AB_TOUCH_OF_LIFE = 155,
	AB_TURN_FLEE = 156,
	AB_TURN_PASSIVE = 157,
	AB_TURN_STONE = 158,
	AB_ULTIMATE_BUFF = 159,
	AB_ULTIMATE_DEBUFF = 160,
	AB_VALAKAS_ITEM = 161,
	AB_VAMPIRIC_ATTACK = 162,
	AB_WATCHER_GAZE = 163,
	AB_RESURRECTION_SPECIAL = 164,
	AB_COUNTER_SKILL = 165,
	AB_AVOID_SKILL = 166,
	AB_CP_UP = 167,
	AB_CP_DOWN = 168,
	AB_CP_REGEN_UP = 169,
	AB_CP_REGEN_DOWN = 170,
	AB_INVINCIBILITY = 171,
	AB_ABNORMAL_INVINCIBILITY = 172,
	AB_PHYSICAL_STANCE = 173,
	AB_MAGICAL_STANCE = 174,
	AB_COMBINATION = 175,
	AB_ANESTHESIA = 176,
	AB_CRITICAL_POISON = 177,
	AB_SEIZURE_PENALTY = 178,
	AB_ABNORMAL_ITEM = 179,
	AB_SEIZURE_A = 180,
	AB_SEIZURE_B = 181,
	AB_SEIZURE_C = 182,
	AB_FORCE_MEDITATION = 183,
	AB_MIRAGE = 184,
	AB_POTION_OF_GENESIS = 185,
	AB_PVP_DMG_UP = 186,
	AB_PVP_DMG_DOWN = 187,
	AB_IRON_SHIELD = 188,
	AB_TRANSFER_DAMAGE = 189,
	AB_SONG_OF_ELEMENTAL = 190,
	AB_DANCE_OF_ALIGNMENT = 191,
	AB_ARCHER_SPECIAL = 192,
	AB_SPOIL_BOMB = 193,
	AB_FIRE_DOT = 194,
	AB_WATER_DOT = 195,
	AB_WIND_DOT = 196,
	AB_EARTH_DOT = 197,
	AB_HEAL_POWER_UP = 198,
	AB_RECHARGE_UP = 199,
	AB_NORMAL_ATTACK_BLOCK = 200,
	AB_DISARM = 201,
	AB_DEATH_MARK = 202,
	AB_KAMAEL_SPECIAL = 203,
	AB_TRANSFORM = 204,
	AB_DARK_SEED = 205,
	AB_REAL_TARGET = 206,
	AB_FREEZING = 207,
	AB_TIME_CHECK = 208,
	AB_MA_MD_UP = 209,
	AB_DEATH_CLACK = 210,
	AB_HOT_GROUND = 211,
	AB_EVIL_BLOOD = 212,
	AB_ALL_REGEN_UP = 213,
	AB_ALL_REGEN_DOWN = 214,
	AB_IRON_SHIELD_I = 215,
	AB_ARCHER_SPECIAL_I = 216,
	AB_T_CRT_RATE_UP = 217,
	AB_T_CRT_RATE_DOWN = 218,
	AB_T_CRT_DMG_UP = 219,
	AB_T_CRT_DMG_DOWN = 220,
	AB_INSTINCT = 221,
	AB_OBLIVION = 222,
	AB_WEAK_CONSTITUTION = 223,
	AB_THIN_SKIN = 224,
	AB_ENERVATION = 225,
	AB_SPITE = 226,
	AB_MENTAL_IMPOVERISH = 227,
	AB_ATTRIBUTE_POTION = 228,
	AB_TALISMAN = 229,
	AB_MULTI_DEBUFF_FIRE = 230,
	AB_MULTI_DEBUFF_WATER = 231,
	AB_MULTI_DEBUFF_WIND = 232,
	AB_MULTI_DEBUFF_EARTH = 233,
	AB_MULTI_DEBUFF_HOLY = 234,
	AB_MULTI_DEBUFF_UNHOLY = 235,
	AB_LIFE_FORCE_KAMAEL = 236,
	AB_MA_UP_SPECIAL = 237,
	AB_PK_PROTECT = 238,
	AB_MAXIMUM_ABILITY = 239,
	AB_TARGET_LOCK = 240,
	AB_PROTECTION = 241,
	AB_WILL = 242,
	AB_SEED_OF_KNIGHT = 243,
	AB_EXPOSE_WEAK_POINT = 244,
	AB_FORCE_OF_DESTRUCTION = 245,
	AB_ELEMENTAL_ARMOR = 246,
	AB_SUMMON_CONDITION = 247,
	AB_IMPROVE_PA_PD_UP = 248,
	AB_IMPROVE_MA_MD_UP = 249,
	AB_IMPROVE_HP_MP_UP = 250,
	AB_IMPROVE_CRT_RATE_DMG_UP = 251,
	AB_IMPROVE_SHIELD_RATE_DEFENCE_UP = 252,
	AB_IMPROVE_SPEED_AVOID_UP = 253,
	AB_LIMIT = 254,
	AB_MULTI_DEBUFF_SOUL = 255,
	AB_CURSE_LIFE_FLOW = 256,
	AB_BETRAYAL_MARK = 257,
	AB_TRANSFORM_HANGOVER = 258,
	AB_TRANSFORM_SCRIFICE = 259,
	AB_SONG_OF_WINDSTORM = 260,
	AB_DANCE_OF_BLADESTORM = 261,
	AB_IMPROVE_VAMPIRIC_HASTE = 262,
	AB_WEAPON_MASTERY = 263,
	AB_APELLA = 264,
	AB_TRANSFORM_SCRIFICE_P = 265,
	AB_SUB_TRIGGER_HASTE = 266,
	AB_SUB_TRIGGER_DEFENCE = 267,
	AB_SUB_TRIGGER_CRT_RATE_UP = 268,
	AB_SUB_TRIGGER_SPIRIT = 269,
	AB_MIRAGE_TRAP = 270,
	AB_DEATH_PENALTY = 271,
	AB_ENTRY_FOR_GAME = 272,
	AB_BLOOD_CONSTRACT = 273,
	AB_DWARF_BUFF = 274,
	AB_EVASION_BUFF = 275,
	AB_BLESS_THE_BLOOD = 277,
	AB_PVP_WEAPON_BUFF = 278,
	AB_PVP_WEAPON_DEBUFF = 279,
	AB_SPEED_OF_CRITICAL = 280,
	AB_VP_UP = 281,
	AB_BOT_PENALTY = 282,
	AB_HIDE = 283,
	AB_DD_RESIST = 284,
	AB_SONG_OF_PURIFICATION = 285,
	AB_DANCE_OF_BERSERKER = 286,
	AB_REFLECT_MAGIC_DD = 287,
	AB_FINAL_SECRET = 288,
	AB_STIGMA_OF_SILEN = 289,
	AB_SEED_DEBUFF_A = 290,
	AB_SEED_DEBUFF_B = 291,
	AB_SEED_DEBUFF_C = 292,
	AB_SEED_DEBUFF_D = 293,
	AB_SEED_DEBUFF_E = 294,
	AB_SEED_DEBUFF_F = 295,
	AB_SEED_DEBUFF_G = 296,
	AB_SEED_DEBUFF_H = 297,
	AB_SEED_DEBUFF_I = 298,
	AB_SEED_BUFF_A = 299,
	AB_AGATHION_BUFF = 300,
	AB_COUNTER_CRITICAL = 301,
	AB_COUNTER_CRITICAL_TRIGGER = 302,
	AB_ATTACK_TIME_DOWN_SPECIAL = 303,
	AB_BLOCK_SPEED_UP = 304,
	AB_BLOCK_SHIELD_UP = 305,
	AB_DEATHWORM = 306,
	AB_MULTI_DEBUFF_A = 307,
	AB_MULTI_DEBUFF_B = 308,
	AB_MULTI_DEBUFF_C = 309,
	AB_MULTI_DEBUFF_D = 310,
	AB_MULTI_DEBUFF_E = 311,
	AB_MULTI_DEBUFF_F = 312,
	AB_MULTI_DEBUFF_G = 313,
	AB_STIGMA_A = 314,
	AB_MULTI_BUFF_A = 315,
	AB_VAMPIRIC_ATTACK_SPECIAL = 316,
	AB_BLOCK_RESURRECTION = 317,
	AB_IMPROVE_HIT_DEFENCE_CRT_RATE_UP = 318,
	AB_IMPROVE_MAGIC_SPEED_CRT_RATE_UP = 319,
	AB_EVENT_GAWI = 320,
	AB_EVENT_BAWI = 321,
	AB_EVENT_BO = 322,
	AB_EVENT_WIN = 323,
	AB_EVENT_TERRITORY = 324,
	AB_EVENT_SANTA_REWARD = 325,
	AB_BR_EVENT_BUF1 = 326,
	AB_BR_EVENT_BUF2 = 327,
	AB_BR_EVENT_BUF3 = 328,
	AB_BR_EVENT_BUF4 = 329,
	AB_BR_EVENT_BUF5 = 330,
	AB_BR_EVENT_BUF6 = 331,
	AB_BR_EVENT_BUF7 = 332,
	AB_BR_EVENT_BUF8 = 333,
	AB_BR_EVENT_BUF9 = 334,
	AB_BR_EVENT_BUF10 = 335,
	AB_MAX = 336
};

enum AttributeType
{
	ATTR_NONE = -2,
	ATTR_FIRE = 0,
	ATTR_WATER = 1,
	ATTR_WIND = 2,
	ATTR_EARTH = 3,
	ATTR_HOLY = 4,
	ATTR_UNHOLY = 5	
};

enum TraitType
{
	TRAIT_NONE = 0,
	TRAIT_SWORD = 1,
	TRAIT_BLUNT = 2,
	TRAIT_DAGGER = 3,
	TRAIT_POLE = 4,
	TRAIT_FIST = 5,
	TRAIT_BOW = 6,
	TRAIT_ETC = 7,
	TRAIT_POISON = 9,
	TRAIT_HOLD = 10,
	TRAIT_BLEED = 11,
	TRAIT_SLEEP = 12,
	TRAIT_SHOCK = 13,
	TRAIT_DERANGEMENT = 14,
	TRAIT_BUG_WEAKNESS = 15,
	TRAIT_ANIMAL_WEAKNESS = 16,
	TRAIT_PLANT_WEAKNESS = 17,
	TRAIT_BEAST_WEAKNESS = 18,
	TRAIT_DRAGON_WEAKNESS = 19,
	TRAIT_PARALYZE = 20,
	TRAIT_DUAL = 21,
	TRAIT_DUALFIST = 22,
	TRAIT_BOSS = 23,
	TRAIT_GIANT_WEAKNESS = 24,
	TRAIT_CONSTRUCT_WEAKNESS = 25,
	TRAIT_DEATH = 26,
	TRAIT_VALAKAS = 27,
	TRAIT_ANESTHESIA = 28,
	TRAIT_CRITICAL_POISON = 29,
	TRAIT_ROOT_PHYSICALLY = 30,
	TRAIT_ROOT_MAGICALLY = 31,
	TRAIT_RAPIER = 32,
	TRAIT_CROSSBOW = 33,
	TRAIT_ANCIENTSWORD = 34,
	TRAIT_TURN_STORN = 35,
	TRAIT_GUST = 36,
	TRAIT_PHYSICAL_BLOCKADE = 37,
	TRAIT_TARGET = 38,
	TRAIT_PHYSICAL_WEAKNESS = 39,
	TRAIT_MAGICAL_WEAKNESS = 40,
	TRAIT_UNKN = 41,
	TRAIT_WATER = 42,
	TRAIT_FIRE = 43,
	TRAIT_WIND = 44,
	TRAIT_EARTH = 45,
	TRAIT_HOLY = 46,
	TRAIT_UNHOLY = 47,
	TRAIT_COUNT = 48
};

enum DebuffType
{
	DT_BUFF = 0,
	DT_DEBUFF = 1,
	DT_ALL = 2,
	DT_STORE_BUFF = 3
};

enum ETCITEM_TYPE
{
	ETCITEM_NONE = 0x0,
	ETCITEM_SCROLL = 0x1,
	ETCITEM_ARROW = 0x2,
	ETCITEM_POTION = 0x3,
	ETCITEM_SPELLBOOK = 0x4,
	ETCITEM_RECIPE = 0x5,
	ETCITEM_MATERIAL = 0x6,
	ETCITEM_PET_COLLAR = 0x7,
	ETCITEM_CASTLE_GUARD = 0x8,
	ETCITEM_DYE = 0x9,
	ETCITEM_SEED = 0x0A,
	ETCITEM_SEED2 = 0x0B,
	ETCITEM_HARVEST = 0x0C,
	ETCITEM_LOTTO = 0x0D,
	ETCITEM_RACE_TICKET = 0x0E,
	ETCITEM_TICKET_OF_LORD = 0x0F,
	ETCITEM_LURE = 0x10,
	ETCITEM_CROP = 0x11,
	ETCITEM_MATURECROP = 0x12,
	ETCITEM_SCRL_ENCHANT_WP = 0x13,
	ETCITEM_SCRL_ENCHANT_AM = 0x14,
	ETCITEM_BLESS_SCRL_ENCHANT_WP = 0x15,
	ETCITEM_BLESS_SCRL_ENCHANT_AM = 0x16,
	ETCITEM_COUPON = 0x17,
	ETCITEM_ELIXIR = 0x18,
	ETCITEM_SCRL_ENCHANT_ATTR = 0x19,
	ETCITEM_BOLT = 0x1A,
	ETCITEM_SCRL_INC_ENCHANT_PROP_WP = 0x1B,
	ETCITEM_SCRL_INC_ENCHANT_PROP_AM = 0x1C,
	ETCITEM_CRYSTAL_ENCHANT_AM = 0x1D,
	ETCITEM_CRYSTAL_ENCHANT_WP = 0x1E,
	ETCITEM_ANCIENT_CRYSTAL_ENCHANT_AM = 0x1F,
	ETCITEM_ANCIENT_CRYSTAL_ENCHANT_WP = 0x20,
	ETCITEM_RUNE = 0x21,
	ETCITEM_RUNE_SELECT = 0x22,
	ETCITEM_SCRL_SAFE_ENCHANT_AM = 0x23,
	ETCITEM_SCRL_SAFE_ENCHANT_WP = 0x24,
	ETCITEM_SCRL_SAFE_ENCHANT_WEAPON_ATTR = 0x25,
	ETCITEM_SCRL_SAFE_ENCHANT_ARMOR_ATTR = 0x26
};


enum CategoryType
{
	fighter_group_category = 0,
	mage_group_category = 1,
	wizard_group_category = 2,
	cleric_group_category = 3,
	attacker_group_category = 4,
	tanker_group_category = 5,
	first_class_group_category = 6,
	second_class_group_category = 7,
	third_class_group_category = 8,
	fourth_class_group_category = 9,
	bounty_hunter_group_category = 10,
	warsmith_group_category = 11,
	summon_npc_group_category = 12,
	knight_group_category = 13,
	white_magic_group_category = 14,
	heal_group_category = 15,
	assist_magic_group_category = 16,
	warrior_group_category = 17,
	human_2nd_group_category = 18,
	elf_2nd_group_category = 19,
	delf_2nd_group_category = 20,
	orc_2nd_group_category = 21,
	dwarf_2nd_group_category = 22,
	strider_category = 23,
	strider_group_category = 24,
	red_strider_group_category = 25,
	wolf_group_category = 26,
	grown_up_wolf_group_category = 27,
	hatchling_group_category = 28,
	baby_pet_group_category = 29,
	upgrade_baby_pet_group_category = 30,
	wyvern_group_category = 31,
	all_wolf_group_category = 32,
	wolf_category = 33,
	sin_eater_group_category = 34,
	pet_group_category = 35,
	item_equip_pet_group_category = 36,
	subjob_group_dagger_category = 37,
	subjob_group_bow_category = 38,
	subjob_group_knight_category = 39,
	subjob_group_summoner_category = 40,
	subjob_group_half_healer_category = 41,
	subjob_group_dance_category = 42,
	subjob_group_wizard_category = 43,
	human_fall_class_category = 44,
	human_wall_class_category = 45,
	human_mall_class_category = 46,
	human_call_class_category = 47,
	elf_fall_class_category = 48,
	elf_mall_class_category = 49,
	elf_wall_class_category = 50,
	elf_call_class_category = 51,
	delf_fall_class_category = 52,
	delf_mall_class_category = 53,
	delf_wall_class_category = 54,
	delf_call_class_category = 55,
	orc_fall_class_category = 56,
	orc_mall_class_category = 57,
	dwarf_all_class_category = 58,
	dwarf_bounty_class_category = 59,
	dwarf_smith_class_category = 60,
	kamael_all_class_category = 61,
	kamael_first_class_group_category = 62,
	kamael_second_class_group_category = 63,
	kamael_third_class_group_category = 64,
	kamael_fourth_class_group_category = 65,
	beginner_fighter_category = 66,
	beginner_mage_category = 67,
	kamael_male_main_occupation_category = 68,
	kamael_female_main_occupation_category = 69,
	archer_group_category = 70,
	shield_master_category = 71,
	bard_category = 72,
	force_master_category = 73,
	weapon_master_category = 74,
	bow_master_category = 75,
	dagger_master_category = 76,
	heal_master_category = 77,
	wizard_master_category = 78,
	buff_master_category = 79,
	summon_master_category = 80,
	warrior_cloack_category = 81,
	rogue_cloack_category = 82,
	mage_cloack_category = 83,
	shield_master2_3_category = 84,
	bard2_3_category = 85,
	force_master2_3_category = 86,
	weapon_master2_3_category = 87,
	bow_master2_3_category = 88,
	dagger_master2_3_category = 89,
	heal_master2_3_category = 90,
	wizard_master2_3_category = 91,
	buff_master2_3_category = 92,
	summon_master2_3_category = 93,
	attribute_group_summoner_category = 94,
	sub_group_warrior_category = 95,
	sub_group_rogue_category = 96,
	sub_group_knight_category = 97,
	sub_group_summoner_category = 98,
	sub_group_wizard_category = 99,
	sub_group_healer_category = 100,
	sub_group_enchanter_category = 101,
	sub_group_hec_category = 102,
	sub_group_hew_category = 103,
	sub_group_hef_category = 104,
	sub_group_orc_category = 105,
	sub_group_ware_category = 106,
	sub_group_black_category = 107,
	sub_group_de_category = 108,
	sub_group_kamael_category = 109,
	light_tanker_group_category = 110,
	dark_tanker_group_category = 111,
	melee_attacker_category = 112
};