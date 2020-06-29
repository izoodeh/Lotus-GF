#include "stdafx.h"
#include "Packet.h"

COutPacket::COutPacket() : m_len(0)
{
}

COutPacket::~COutPacket()
{
}

void COutPacket::WriteC(BYTE value)
{
	(*reinterpret_cast<PBYTE>(&m_buff[m_len])) = value;
	m_len += sizeof(BYTE);
}

void COutPacket::WriteH(WORD value)
{
	(*reinterpret_cast<PWORD>(&m_buff[m_len])) = value;
	m_len += sizeof(WORD);
}

void COutPacket::WriteD(DWORD value)
{
	(*reinterpret_cast<PDWORD>(&m_buff[m_len])) = value;
	m_len += sizeof(DWORD);
}

void COutPacket::WriteQ(QWORD value)
{
	(*reinterpret_cast<PQWORD>(&m_buff[m_len])) = value;
	m_len += sizeof(QWORD);
}

CInPacket::CInPacket(SIZE_T length, PCHAR lpBuff) : m_pBuff(lpBuff), m_index(0), m_length(length)
{

}

CInPacket::~CInPacket()
{
}

CHAR CInPacket::ReadC()
{
	if((m_index + sizeof(CHAR)) <= m_length)
	{
		CHAR ret = m_pBuff[m_index];
		m_index++;
		return ret;
	}
	return 0;
}

WORD CInPacket::ReadH()
{
	if((m_index + sizeof(WORD)) <= m_length)
	{
		WORD ret = (*reinterpret_cast<PWORD>(&m_pBuff[m_index]));
		m_index+= sizeof(WORD);
		return ret;
	}
	return 0;
}

DWORD CInPacket::ReadD()
{
	if((m_index + sizeof(DWORD)) <= m_length)
	{
		DWORD ret = (*reinterpret_cast<PDWORD>(&m_pBuff[m_index]));
		m_index+= sizeof(DWORD);
		return ret;
	}
	return 0;
}

QWORD CInPacket::ReadQ()
{
	if((m_index + sizeof(WORD)) <= m_length)
	{
		QWORD ret = (*reinterpret_cast<PQWORD>(&m_pBuff[m_index]));
		m_index+= sizeof(QWORD);
		return ret;
	}
	return 0;
}
