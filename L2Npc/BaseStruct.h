#pragma once
#pragma pack( push, 1 )

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
};

struct FAngle
{
	int heading;
};

union SkillPchId
{
	int pchId;
	struct 
	{
		WORD level;
		WORD skillId;
	};
	SkillPchId() : pchId(0) {};
	SkillPchId(int id) : pchId(id) {};
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

#pragma pack( pop )