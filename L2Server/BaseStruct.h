#pragma once
#pragma pack( push, 1 )

union UtB
{
	UINT full;
	BYTE part[4];
};

union HWID
{
	BYTE hash[32];
	UINT64 part[4];
	inline bool IsNull()
	{
		if(part[0] == 0 && part[1] == 0 && part[2] == 0 && part[3] == 0)
		{
			return true;
		}
		return false;
	}
	inline bool IsEqual(HWID& other)
	{
		if(part[0] == other.part[0] && part[1] == other.part[1] && part[2] == other.part[2] && part[3] == other.part[3])
		{
			return true;
		}
		return false;
	}
	HWID() { clear(); };
	HWID(LPBYTE lpHwid) { memcpy(hash, lpHwid, 32); };
	HWID(const unsigned char* lpHwid) { memcpy(hash, lpHwid, 32); };
	HWID(wstring value)
	{
		if(value.size() == 32)
		{
			for(UINT n=0;n<32;n++)
			{
				hash[n] = static_cast<BYTE>(value[n]);
			}
		}else
		{
			clear();
		}
	}
	void clear() { part[0] = part[1] = part[2] = part[3] = 0; };
	wstring ToString()
	{
		wstring str;
		for(UINT n=0;n<32;n++)
		{
			str.push_back((WCHAR)hash[n]);
		}
		return str;
	}

};

union HookMem
{
	struct
	{
		BYTE p[5];
	}byte;
	struct
	{
		BYTE opCode;
		DWORD addr;
	}full;
};


struct AcquiredSkillParam
{
	INT32 level;
	INT32 temporary;
};

//0xC8 size
struct SkillAcquireInfo
{
	/* 00 */ int skillId;
	/* 04 */ int level;
	/* 08 */ LPVOID _unkn08;
	/* 10 */ INT32 requiredLevel;
	/* 14 */ UINT _unkn14;
	/* 18 */ LPVOID _unkn18[9];
	/* 60 */ UINT requiredSocialClass;
	/* 64 */ UINT requiredPledgeType;	//default -1
	/* 68 */ LPVOID _unkn68[12];
	/* C8 */ //END
};

union IPnMAC
{
	struct
	{
		BYTE data[12];
	}Full;
	struct
	{
		UINT32 ip;
		UINT64 mac;
	}Part;
	bool operator == (const IPnMAC& other) const
	{
		if(this->Part.ip == other.Part.ip && this->Part.mac == other.Part.mac)
			return true;

		return false;
	}
	bool operator != (const IPnMAC& other) const
	{
		if(this->Part.ip != other.Part.ip)
			return true;
		if(this->Part.mac != other.Part.mac)
			return true;

		return false;
	}
	bool operator < (const IPnMAC& other) const
	{
		for(UINT n = 0;n<12;n++)
		{
			if(this->Full.data[n] < other.Full.data[n])
				return true;
			
			if(this->Full.data[n] > other.Full.data[n])
				return false;
		}
		return false;
	}
};

struct BaseAttribute
{
	/* 00 */ USHORT attackAttribute;
	/* 02 */ USHORT attackAttributeValue;
	/* 04 */ USHORT defAttribute[6];
	/* 10 */ INT64 _unkn;	//it's word but due to align it's INT64
	BaseAttribute() : attackAttribute(-2), attackAttributeValue(0), _unkn(0) 
	{
		defAttribute[0] = 0;
		defAttribute[1] = 0;
		defAttribute[2] = 0;
		defAttribute[3] = 0;
		defAttribute[4] = 0;
		defAttribute[5] = 0;
	}
};

struct ItemOptionKey
{
	UINT option1;
	UINT option2;
	ItemOptionKey() : option1(0), option2(0) {};
};

struct FVector
{
	double x;
	double y;
	double z;
	FVector(double x1, double y1, double z1) : x(x1), y(y1), z(z1) {};
	FVector() : x(0), y(0), z(0) {};
};

struct IVector
{
	int x;
	int y;
	int z;

	IVector() : x(0), y(0), z(0) {}
	IVector(int X, int Y, int Z): x(X), y(Y), z(Z)	{}
	IVector(double X, double Y, double Z): x((int)X), y((int)Y), z((int)Z) {}
	IVector(const FVector& other) : x((int)other.x), y((int)other.y), z((int)other.z) {}
	IVector& operator = (const FVector& other)
	{
		x = static_cast<int>(other.x);
		y = static_cast<int>(other.y);
		z = static_cast<int>(other.z);
		return *this;
	}
	bool operator == (const IVector& other)
	{
		if(other.x == x && other.y == y && other.z == z)
			return true;
		return false;
	}
	bool operator != (const IVector& other)
	{
		if(other.x != x || other.y != y || other.z != z)
			return true;
		return false;
	}
};

struct FAngle
{
	int heading;
};

union SkillPchId
{
	UINT pchId;
	struct 
	{
		WORD level;
		WORD skillId;
	};
	SkillPchId() : pchId(0) {};
	SkillPchId(UINT id) : pchId(id) {};
	SkillPchId(int id, int lvl) : skillId(id), level(lvl) {};
	bool operator == (const SkillPchId& other) const
	{
		if(pchId == other.pchId)
			return true;
		return false;
	}
	bool operator != (const SkillPchId& other) const
	{
		if(pchId != other.pchId)
			return true;
		return false;
	}
	bool operator < (const SkillPchId& other) const
	{
		if(pchId < other.pchId)
			return true;
		return false;
	}
};

struct XList
{
	LPVOID _unkn[3];
	XList()
	{
		_unkn[0] = 0;
		typedef LPVOID (*f)(LPVOID);
		_unkn[1] = f(0x45F70CL)(this);
		_unkn[2] = 0;
	}
	~XList()
	{

	}
};

struct AbnormalData
{
	/* 00 */ CSkillInfo *pSI;
	/* 08 */ __time32_t castTime;
	/* 0C */ bool selfBuff;
	/* 0D */ bool _alignD[3];
	/* 10 */ UINT casterId;
	/* 14 */ UINT party;
	/* 18 */ CSkillInfo *pSIEx;
	/* 20 */ XList lData;
	/* 38 */ UINT exTime;
	/* 3C */ UINT _unkn3C;
};

typedef vector <AbnormalData> AbnormalDataVector;

struct StoredAbnormalData
{
	/* 00 */ CSkillInfo *pSI;
	/* 08 */ __time32_t castTime;
	/* 0C */ BOOL selfBuff;
	/* 10 */ UINT casterId;
	/* 14 */ UINT party;
	/* 18 */ CSkillInfo *pSIEx;
	/* 20 */ UINT exTime;
	/* 24 */ UINT _unkn3C;
	/* 28 */ time_t expireTime;
	StoredAbnormalData(AbnormalData ad, time_t expTime)
	{
		pSI = ad.pSI;
		castTime = ad.castTime;
		selfBuff = ad.selfBuff;
		casterId = ad.casterId;
		party = ad.party;
		pSIEx = ad.pSIEx;
		exTime = ad.exTime;
		_unkn3C = ad._unkn3C;
		expireTime = expTime;
	}
};

typedef list<StoredAbnormalData> StoredAbnormalDataList;

union KillReuseData
{
	struct
	{
		UINT64 full;
	}Full;
	struct
	{
		UINT databaseId;
		UINT killTimestamp;
	}Part;
};

union KillReuseDataEx
{
	struct
	{
		UINT64 full;
	}Full;
	struct
	{
		IPnMAC uniqueId;
		UINT killTimestamp;
	}Part;
};

struct TerritoryData
{
	UINT territoryId[TerritoryCount];
	TerritoryData() { clear(); };
	void clear() { for(UINT n=0;n<TerritoryCount;n++) { territoryId[n] = 0; } };
};

struct ItemReq
{
	INT32 itemId;
	INT64 amount;
	ItemReq() : itemId(0), amount(0) {};
};

struct TeleportInfo
{
	UINT id;
	IVector loc;
	ItemReq price;
	INT32 minLevel;
	INT32 maxLevel;
	UINT castleId;
	TeleportInfo() : id(0), minLevel(0), maxLevel(90), castleId(0) {}
	void clear() { id = 0; };
};

#pragma pack( pop )