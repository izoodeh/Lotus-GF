#pragma once
#pragma pack( push, 1 )

//size DF8h
class CSharedCreatureData
{
public:
	/* 000 */ LPVOID _unkn000;
	/* 008 */ FVector pos;
	/* 020 */ WORD heading;
	/* 022 */ WORD _align22;
	/* 024 */ INT32 object_id;
	/* 028 */ INT32 sharedIndex;
	/* 02C */ INT32 _unkn2C;
	/* 030 */ LPVOID _unkn030;
	/* 038 */ LPVOID _unkn038;
	/* 040 */ LPVOID _unkn040;
	/* 048 */ LPVOID _unkn048;
	/* 050 */ BOOL active;
	/* 054 */ UINT _unkn54;
	/* 058 */ LPVOID _unkn058;
	/* 060 */ INT32 _unkn060;
	/* 064 */ BOOL is_pc;
	/* 068 */ BOOL alive;
	/* 06C */ INT32 age;
	/* 070 */ INT32 _unkn070;
	/* 074 */ INT32 private_id;
	/* 078 */ INT32 subjob_id;
	/* 07C */ INT32 nobless_type;
	/* 080 */ INT32 hero_type;
	/* 084 */ INT32 _align84;
	/* 088 */ INT32 subjob0_class;
	/* 08C */ INT32 subjob1_class;
	/* 090 */ INT32 subjob2_class;
	/* 094 */ INT32 subjob3_class;
	/* 098 */ INT32 race;
	/* 09C */ INT32 occupation;	//current class
	/* 0A0 */ INT32 sex;
	/* 0A4 */ WCHAR wName[25];
	/* 0D6 */ bool undying;
	/* 0D7 */ bool can_be_attacked;
	/* 0D8 */ bool invisible;
	/* 0D9 */ bool _unkn0D9;
	/* 0DA */ INT16 _unkn0DA;
	/* 0DC */ INT32 _unknDC;
	/* 0E0 */ double collision_radius;
	/* 0E8 */ double collision_height;
	/* 0F0 */ INT32 hair_style;
	/* 0F4 */ INT32 hair_color;
	/* 0F8 */ INT32 face_style;
	/* 0FC */ INT32 _unkn0FC;

	/* 100 */ INT64 exp;
	/* 108 */ INT32 sp;
	/* 10C */ INT32 lastItemSharedIndex;
	/* 110 */ INT32 equipment[29];
	/* 184 */ INT32 _unkn184;
	/* 188 */ LPVOID _unkn188;
	/* 190 */ LPVOID _unkn190;
	/* 198 */ LPVOID _unkn198;
	/* 1A0 */ LPVOID _unkn1A0;
	/* 1A8 */ INT32 _unkn1A8;
	/* 1AC */ INT32 baseINT;
	/* 1B0 */ INT32 baseSTR;
	/* 1B4 */ INT32 baseCON;
	/* 1B8 */ INT32 baseMEN;
	/* 1BC */ INT32 baseDEX;
	/* 1C0 */ INT32 baseWIT;
	/* 1C4 */ INT32 _unkn1C4;
	/* 1C8 */ double hp;
	/* 1D0 */ double cp;
	/* 1D8 */ double mp;
	/* 1E0 */ INT32 is_not_moving;	// 0 when moving 1 when stops
	/* 1E4 */ INT32 move_type;	
	/* 1E8 */ INT32 stop_mode;
	/* 1EC */ INT32 fly_type;
	/* 1F0 */ LPVOID _unkn1F0;
	/* 1F8 */ INT32 target_index;
	/* 1FC */ INT32 target_id;
	
	/* 200 */ bool is_guilty;
	/* 201 */ bool is_freezed;
	/* 202 */ INT16 _align202;
	/* 204 */ INT32 pvp_count;
	/* 208 */ INT32 pk_count;
	/* 20C */ INT32 pk_pardon;
	/* 210 */ INT32 karma;
	/* 214 */ INT16 _unkn214;
	/* 216 */ INT16 sociality_give_count;
	/* 218 */ INT32 sociality_count;
	/* 21C */ UINT pvpFlagTimestamp;
	/* 220 */ INT32 memo[26][4];
	/* 3C0 */ INT32 nr_memo[20][4];

	/* 500 */ BYTE _unkn500[212];
	/* 5D4 */ BYTE oneTimeQuestMap[32];
	/* 5F4 */ BYTE _unkn5F4[12];

	/* 600 */ LPVOID _unkn600[10];
	/* 650 */ INT32 _unkn650;
	/* 654 */ UINT quest_last_reward_time;
	/* 658 */ INT32 _unkn658;
	/* 65C */ INT32 _unkn65C;
	/* 660 */ INT32 pledge_id;
	/* 664 */ INT32 pledge_index;
	/* 668 */ BOOL is_pledge_master;
	/* 66C */ INT32 pledge_type;
	/* 670 */ LPVOID _unkn670;
	/* 678 */ INT32 grade_id;
	/* 67C */ INT32 _unkn67C;
	/* 680 */ INT32 alliance_id;
	/* 684 */ INT32 _unkn684;
	/* 688 */ INT32 _unkn688;
	/* 68C */ INT32 party_id;
	/* 698 */ LPVOID _unkn690;
	/* 690 */ LPVOID _unkn698;
	/* 6A0 */ INT32 residence_id;
	/* 6A4 */ INT32 castle_id;	//used by npcs - mercenary
	/* 6A8 */ INT32 _unkn6A8;
	/* 6AC */ UINT storeMode;
	/* 6B0 */ LPVOID _unkn6B0[10];	//0x6EC - consumed_body - npc will despawn 0x6F0 - resurrectedExp;

	/* 700 */ LPVOID _unkn700[7];
	/* 738 */ INT32 death_penalty_level;
	/* 73C */ BOOL is_newbie;
	/* 740 */ INT32 dbid;
	/* 744 */ INT32 base_attack_type;	//?
	/* 748 */ INT32 base_can_penetrate;	//?
	/* 74C */ INT32 base_attack_range;	//?
	/* 750 */ INT32 base_damage_range[4];	//?
	/* 760 */ INT32 base_physical_attack;	//18
	/* 764 */ INT32 base_physical_defence;	//7 field_type
	/* 768 */ INT32 base_attack_speed; //21
	/* 76C */ INT32 base_critical;	//19
	/* 770 */ INT32 base_unkn770; //accuracy	??
	/* 774 */ INT32 base_unkn774;	//??
	/* 778 */ INT32 base_unkn778;
	/* 77C */ INT32 base_unkn77C;
	/* 780 */ INT32 base_unkn780;
	/* 784 */ INT32 base_unkn784;
	/* 788 */ INT32 base_unkn788;
	/* 78C */ INT32 base_hitAvoid;
	/* 790 */ LPVOID _unkn790[2];
	/* 7A0 */ INT32 _unkn7A0;
	/* 7A4 */ INT32 level;
	/* 7A8 */ INT32 max_slots;
	/* 7AC */ INT32 current_slots;
	/* 7B0 */ INT32 current_weight;
	/* 7B4 */ INT32 _unkn7B4;
	/* 7B8 */ double max_load;
	/* 7C0 */ double org_hp;
	/* 7C8 */ double org_mp;
	/* 7D0 */ double org_cp;
	/* 7D8 */ double max_hp;
	/* 7E0 */ double max_mp;
	/* 7E8 */ double max_cp;
	/* 7F0 */ double org_hp_regen;
	/* 7F8 */ double org_mp_regen;

	/* 800 */ double org_cp_regen;
	/* 808 */ double hp_regen;
	/* 810 */ double mp_regen;
	/* 818 */ double cp_regen;
	/* 820 */ double org_speed;
	/* 828 */ double speed;
	/* 830 */ double p_hit;
	/* 838 */ double avoid_rate;
	/* 840 */ double unkn840; //something related to attack speed or reuse
	/* 848 */ INT32 equiped_weapon_class_id;
	/* 84C */ UINT attack_type;
	/* 850 */ INT32 _unkn850;
	/* 854 */ UINT armorType;
	/* 858 */ UINT attackRange;
	/* 85C */ UINT _unkn85C;
	/* 860 */ LPVOID _unkn858[3];
	/* 878 */ double physical_attack;
	/* 880 */ double physical_defend;
	/* 888 */ double magic_attack;
	/* 890 */ double magic_defend;
	/* 898 */ double attack_speed;
	/* 8A0 */ double physical_critical_rate;
	/* 8A8 */ double hit_time_factor;
	/* 8B0 */ double time_factor;
	/* 8B8 */ double physical_use_speed; //for casting skills
	/* 8C0 */ double magic_use_speed;
	/* 8C8 */ INT32 builder_level;
	/* 8CC */ INT32 _unkn8CC;	// not used oO
	/* 8D0 */ LPVOID _unkn8D0; // not used oO
	/* 8D8 */ INT32 last_blow_weapon_class_id;
	/* 8DC */ INT32 ssq_round;
	/* 8E0 */ INT32 ssq_part;
	/* 8E4 */ INT32 _unkn8E4;
	/* 8E8 */ INT32 seal_selection_no;
	/* 8EC */ INT32 _unkn8EC;
	/* 8F0 */ LPVOID _unkn8F0[2];

	/* 900 */ LPVOID _unkn900[5];
	/* 928 */ INT32 _unkn928;
	/* 92C */ INT32 ssq_dawn_round;
	/* 930 */ BOOL in_peacezone;
	/* 934 */ BOOL in_battlefield;
	/* 938 */ INT32 pc_cafe_points;
	/* 93C */ bool can_use_pc_cafe_point;
	/* 93D */ bool is_pc_cafe_user;	//premium user
	/* 93E */ INT16 _unkn93E; //align
	/* 940 */ INT32 multipvp_points; //have to be rechecked
	/* 944 */ INT32 _unkn944;
	/* 948 */ double overhit_bonus;
	/* 950 */ INT32 _unkn950;
	/* 954 */ bool is_in_combat_mode;
	/* 955 */ bool _unkn955;
	/* 956 */ INT16 _unkn956;
	/* 958 */ LPVOID _unkn958[2];
	/* 968 */ WCHAR wAI[64];
	/* 9E8 */ LPVOID _unkn9E8[3];

	/* A00 */ LPVOID _unknA00[13];
	/* A68 */ INT32 _unknA68;
	/* A6C */ INT32 summon_type;
	/* A70 */ INT32 summon_exp_consume_ratio;
	/* A74 */ INT32 summoner_id;
	/* A78 */ INT32 boss_id;
	/* A7C */ INT32 npc_class_id;
	/* A80 */ INT32 weight_point;
	/* A84 */ INT32 respawn_time;
	/* A88 */ LPVOID _unknA88[3];
	/* AA0 */ INT32 _unknAA0;
	/* AA4 */ INT32 clan_help_range;
	/* AA8 */ LPVOID _unknAA8;
	/* AB0 */ INT32 p_state;
	/* AB4 */ INT32 _unknAB4;
	/* AB8 */ LPVOID _unknAB8[2];
	/* AC8 */ CSharedCreatureData *pMasterSD;
	/* AD0 */ INT32 action;
	/* AD4 */ INT32 _unknAD4;
	/* AD8 */ LPVOID _unknAD8;
	/* AE0 */ bool is_spoiled;
	/* AE1 */ bool _unknAE1;
	/* AE2 */ INT16 _unknAE2;
	/* AE4 */ INT32 _unknAE4;
	/* AE8 */ LPVOID _unknAE8[2];
	/* AF8 */ UINT _unknAF8;
	/* AFC */ UINT notTargetable;	//for CNPC

	/* B00 */ INT32 flag;
	/* B04 */ INT32 yongma_type;	//strider/wyvern/wolf
	/* B08 */ INT32 yongma_class;
	/* B0C */ INT32 strider_level;
	/* B10 */ INT32 meal;
	/* B14 */ INT32 _unknB14;
	/* B18 */ INT32 pet_dbid;
	/* B1C */ INT32 _unknB1C;
	/* B20 */ INT32 max_magic_level;
	/* B24 */ INT32 durationItemEquipped;
	/* B28 */ INT32 _unknB28;
	/* B2C */ INT32 transformID;
	/* B30 */ INT32 instant_zone_id;
	/* B34 */ INT32 instant_zone_type_id;
	/* B38 */ INT32 instant_zone_type_id_in_use;
	/* B3C */ INT32 _unknB3C;
	/* B40 */ LPVOID _unknB40[8];
	/* B80 */ bool _unknB80;
	/* B81 */ bool isTargetable;
	/* B82 */ bool showNameTag;
	/* B83 */ bool isVisible;
	/* B84 */ INT32 _unknB94;
	/* B88 */ LPVOID _unknB88[4];
	/* BA8 */ INT32 _unknBA8;
	/* BAC */ int abnormalTable[46][3];	//AbnormalStatusSlotType/level/unkn

	/* DD4 */ INT32 db_value;
	/* DD8 */ INT32 param1;
	/* DDC */ INT32 param2;
	/* DE0 */ INT32 param3;
	/* DE4 */ INT32 flying;
	/* DE8 */ BYTE _unknDE8;
	/* DE9 */ BYTE language;
	/* DEA */ BYTE _alignDEA[6];
	/* DF0 */ LPVOID _unknDF0;

	void DeleteAbnormalStatus(AbnormalStatusSlotType slotType);
	bool HaveMemo(int id);
	bool SetMemo(int id);
	int GetMemoState(int id, BOOL exState = 0);
	bool SetMemoState(int id, int slot, int state);
	bool SetFlagJurnal(int id, int flag);
	bool HaveNRMemo(int id);
	void SetNRMemo(int id);
};

#pragma pack( pop )