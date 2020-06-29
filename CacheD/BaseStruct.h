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

union IP
{
	struct
	{
		UINT8 p[4];
	} Part;
	struct
	{
		UINT address;
	} Full;

	IP(const WCHAR* sIP)
	{
		wstringstream str(sIP);
		WCHAR c;
		int value;
		for(unsigned n = 0;n < 4 ;n++)
		{
			str >> value >> c;
			Part.p[n] = value;

		}
	}
	IP(wstring sIP)
	{
		wstringstream str(sIP);
		WCHAR c;
		int value;
		for(unsigned n = 0;n < 4 ;n++)
		{
			str >> value >> c;
			Part.p[n] = value;

		}
	}
	IP(const char* sIP)
	{
		stringstream str(sIP);
		char c;
		int value;
		for(unsigned n = 0;n < 4 ;n++)
		{
			str >> value >> c;
			Part.p[n] = value;

		}
	}
	IP(string sIP)
	{
		stringstream str(sIP);
		char c;
		int value;
		for(unsigned n = 0;n < 4 ;n++)
		{
			str >> value >> c;
			Part.p[n] = value;

		}
	}
	IP() { Full.address = 0; };
	wstring ToWString()
	{
		wstringstream str;
		str << (UINT)Part.p[0] << L"." << (UINT)Part.p[1] << L"." << (UINT)Part.p[2] << L"." << (UINT)Part.p[3];
		return str.str();
	}
};


#pragma pack( pop )