#pragma once

class COutPacket
{
	CHAR m_buff[16384];
	SIZE_T m_len;
public:
	COutPacket();
	~COutPacket();
	void WriteC(BYTE value);
	void WriteH(WORD value);
	void WriteD(DWORD value);
	void WriteQ(QWORD value);
	inline SIZE_T GetLen() { return m_len; };
	inline CHAR* GetBuff() { return m_buff; };
};

class CInPacket
{
	PCHAR m_pBuff;
	SIZE_T m_index;
	SIZE_T m_length;
public:
	CInPacket(SIZE_T length, PCHAR lpBuff);
	~CInPacket();
	CHAR ReadC();
	WORD ReadH();
	DWORD ReadD();
	QWORD ReadQ();
};