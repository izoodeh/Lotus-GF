#include "stdafx.h"
#include "CliExt.h"
#include "DB.h"
#include "HookManager.h"
#include "PacketCryption.h"

CCliExt g_CliExt;
CLogger g_CliExtLog(L"cliext");

void CCliExt::Init()
{
	LoadINI();
	if(m_Enabled)
	{
		g_Log.Add(CLog::Blue, "[%s] Feature is enabled - version [%d]. ", __FUNCTION__, m_Version);
		WriteNOP(0x9352CF, 3);
		WriteNOP(0x9353B2, 4);
		WriteNOP(0x935485, 3);
		WriteNOP(0x93555C, 4);
		WriteNOP(0x935624, 4);

		WriteNOP(0x8C55E3, 5);	//Bot Checking msg
		WriteNOP(0x8C561E, 5);	//This account and character is using abnormal client bla bla 

		for(UINT n=0;n<256;n++)
		{
			m_CustomHandler[n] = CPHDummyPacket;
		}
		Bind(1, CPHPingExPacket);
		Bind(2, CPHHWIDPacket);
		g_DB.RequestLoadBannedHardwareId();
	}
	PacketCryption::Init();
}

void CCliExt::LoadINI()
{
	const TCHAR* section = _T("CliExt");
	m_Enabled = GetPrivateProfileInt(section, _T("Enabled"), 0, g_ConfigFile);
	m_Version = GetPrivateProfileInt(section, _T("Version"), 0, g_ConfigFile);
	m_BoxLimit = GetPrivateProfileInt(section, _T("BoxLimit"), 0, g_ConfigFile);

	m_BoxLimitHTML[0] = 0;
	GetPrivateProfileString(section, _T("ReachedBoxLimitHtml"), 0, m_BoxLimitHTML, 0x4000, g_ConfigFile);
	m_BannedHWIDHTML[0] = 0;
	GetPrivateProfileString(section, _T("BannedHwidHtml"), 0, m_BannedHWIDHTML, 0x4000, g_ConfigFile);
}

void CCliExt::Bind(UINT id, CPHandler func)
{
	if(id < 256)
	{
		m_CustomHandler[id] = func;
	}
}

void CCliExt::OnBanDBLoad(LPBYTE lpHwid, wstring account, wstring gmName, UINT timeout)
{
	guard;

	m_BannedLock.Enter();

	bool exist = false;
	//check if alrady exist to avoid doubles
	for(UINT n=0;n<m_Banned.size();n++)
	{
		if(!memcmp(lpHwid, m_Banned[n].hash, 32))
		{
			//update timeout
			m_Banned[n].timeout = timeout;
			exist = true;
			break;
		}
	}
	if(!exist)
	{
		if(timeout > time(0))
		{
			BannedHardwareId bhi;
			memcpy(&bhi.hash[0], lpHwid, 32);
			bhi.GMName = gmName;
			bhi.accountName = account;
			bhi.timeout = timeout;
			m_Banned.push_back(bhi);
		}
	}

	m_BannedLock.Leave();

	unguard;
}

bool CCliExt::IsBanned(LPBYTE lpHwid)
{
	guard;
	bool ret = false;
	m_BannedLock.Enter();
	for(UINT n=0;n<m_Banned.size();n++)
	{
		if(!memcmp(lpHwid, m_Banned[n].hash, 32))
		{
			if(m_Banned[n].timeout > time(0))
			{
				ret = true;
			}
			break;
		}
	}
	m_BannedLock.Leave();
	unguard;
	return ret;
}

bool CCliExt::UnBan(LPBYTE lpHwid, wstring gmName)
{
	guard;
	bool ret = false;

	m_BannedLock.Enter();
	for(vector<BannedHardwareId>::iterator it = m_Banned.begin(); it!= m_Banned.end(); it++)
	{
		if(!memcmp(lpHwid, it->hash, 32))
		{
			m_Banned.erase(it);
			ret = true;
			break;
		}
	}
	m_BannedLock.Leave();

	if(ret)
	{
		char sHwid[33];
		memset(sHwid, 0, 33);
		memcpy(sHwid, lpHwid, 32);
		g_Log.Add(CLog::Blue, "[%s] GM[%S] HWID[%s]", __FUNCTION__, gmName.c_str(), sHwid);
		g_DB.RequestUnBanHardwareId(lpHwid);
	}
	unguard;
	return ret;
}

void CCliExt::Ban(LPBYTE lpHwid, wstring account, wstring gmName, UINT timeout)
{
	guard;
	m_BannedLock.Enter();
	bool exist = false;
	for(UINT n=0;n<m_Banned.size();n++)
	{
		if(!memcmp(lpHwid, m_Banned[n].hash, 32))
		{
			exist = true;
			m_Banned[n].timeout = timeout;
			break;
		}
	}
	if(!exist)
	{
		BannedHardwareId bhi;
		bhi.accountName = account;
		bhi.GMName = gmName;
		bhi.timeout = timeout;
		memcpy(bhi.hash, lpHwid, 32);
		m_Banned.push_back(bhi);
	}
	m_BannedLock.Leave();
	char sHwid[33];
	memset(sHwid, 0, 33);
	memcpy(sHwid, lpHwid, 32);

	g_DB.RequestBanHardwareId(lpHwid, account.c_str(), gmName.c_str(), timeout);
	g_Log.Add(CLog::Blue, "[%s] GM[%S] HWID[%s] - Account[%S] - timeout[%d]", __FUNCTION__, gmName.c_str(), sHwid, account.c_str(), timeout);
	unguard;
}

void CCliExt::HandleCheckSum(CUserSocket *pSocket, const unsigned char *packet, UINT packetSize)
{
	guard;
	if(packetSize > 4)
	{
		packetSize-=4;
		UINT csBase = (*(PUINT)&packet[packetSize]);
		if(csBase == pSocket->packetCS.previous)
		{
			pSocket->packetCS.current = csBase;
			unguard;
			return;
		}

		UINT csKey = pSocket->packetCS.key;
		if(packet[0] == 0x2B)
		{
	//		csKey = 0;
		}
		UINT checkSum = 0;
		csKey++;
		if((csKey % 2) == 0)
		{
			checkSum = 0x13243546;
		}else
		{
			checkSum = 0x98786756;
		}
		UINT sum = 0;
		for(UINT n=0;n<packetSize;n++)
		{
			if(n < 2)
			{
				sum += n*37;
			}else
			{
				sum += static_cast<UINT>(packet[n]);
			}
		}
		checkSum += sum;
		UtB cs;
		cs.full = checkSum;
		for(int n=0;n<4;n++)
		{
			cs.part[n] += 75;
			cs.part[n] *= packetSize+n+csKey;
		}

		checkSum = cs.full;
		checkSum += csKey;
		checkSum += packetSize;
		csKey += packetSize;
		pSocket->packetCS.calculated = checkSum;
		pSocket->packetCS.current = csBase;
		pSocket->packetCS.key = csKey;
		if(checkSum != csBase)
		{
			CHAR sHWID[33];
			memset(sHWID, 0, sizeof(sHWID));
			memcpy(sHWID, pSocket->hardwareId.hash, 32);
			if(true)
			{
				g_Log.Add(CLog::Error, "[%s] Invalid Checksum[%u] vs [%u] packet[%x] len[%d] sum[%d] key[%d] HWID[%s] Account[%S]!", __FUNCTION__, checkSum, csBase, packet[0], packetSize, sum, csKey, sHWID, pSocket->accountNameW);
				g_CliExtLog.Add(L"Invalid Checksum[%u] vs [%u] packet[%x] len[%d] sum[%d] key[%d] HWID[%S] Account[%s]!", checkSum, csBase, packet[0], packetSize, sum, csKey, sHWID, pSocket->accountNameW);
			}
		}
	}else
	{
		pSocket->packetCS.current = -1;
		pSocket->packetCS.calculated = 0;
	}
	unguard;
}

void CCliExt::DecryptPacket(CUserSocket *pSocket, unsigned char *packet, UINT packetSize)
{
	guard;
	
	static BYTE staticKey1[8][32] = { 0x5A, 0xF8, 0xEB, 0x8E, 0x05, 0x0C, 0xC2, 0x46, 0xB4, 0xAA, 0xCC, 0x97, 0x59, 0x5A, 0x0E, 0x24, 0x27, 0xF0, 0x58, 0xE5, 0x42, 0x1E, 0xCA, 0xF6, 0x5C, 0x2C, 0xFC, 0x37, 0xB0, 0x25, 0xE3, 0x86, 0xB6, 0x38, 0x66, 0x6B, 0xF6, 0x5B, 0xC4, 0x87, 0x2D, 0x70, 0x0A, 0xF4, 0x35, 0x2E, 0x5E, 0x48, 0xAB, 0xB7, 0xA6, 0xE6, 0x87, 0xCD, 0xB0, 0xF2, 0xB1, 0xAA, 0xAD, 0x2C, 0x0C, 0xB6, 0x4C, 0xAF, 0x1C, 0x88, 0x34, 0x1D, 0x98, 0xD9, 0x08, 0x5C, 0xFD, 0x27, 0xAB, 0x05, 0x98, 0x10, 0xA8, 0x96, 0xD4, 0x9E, 0xF2, 0xFD, 0x5D, 0x90, 0xA0, 0x4B, 0xF1, 0xA6, 0xA8, 0x16, 0x7D, 0x8F, 0x58, 0x62, 0x50, 0x8C, 0x8A, 0x3D, 0xD8, 0x6D, 0xC2, 0x49, 0x81, 0x7C, 0x66, 0xBC, 0xEB, 0x13, 0xC9, 0xB6, 0x96, 0x5A, 0xA9, 0x56, 0x36, 0x74, 0xBC, 0xC1, 0x64, 0x14, 0x9B, 0x6C, 0x27, 0xEA, 0x97, 0xEC, 0x93, 0xA9, 0x2F, 0x67, 0x4D, 0x97, 0x59, 0x87, 0x44, 0xF7, 0x1D, 0x0E, 0xC2, 0x13, 0xDD, 0x75, 0x70, 0xA9, 0x16, 0xEA, 0xFC, 0xFA, 0xC7, 0xC6, 0xA0, 0xCD, 0x42, 0xED, 0x7C, 0x94, 0x66, 0xE5, 0x29, 0xEC, 0x00, 0xB9, 0x5D, 0xAE, 0x47, 0x18, 0x21, 0x88, 0x4F, 0xC7, 0xED, 0x48, 0x9A, 0xAA, 0x52, 0x44, 0x1B, 0xDF, 0x0B, 0x04, 0x25, 0xFB, 0xAC, 0x0D, 0x1F, 0x89, 0x21, 0x5A, 0x1A, 0xA2, 0x69, 0x02, 0x34, 0xA0, 0xEF, 0x84, 0xCA, 0xB1, 0xCA, 0x96, 0x53, 0xA2, 0x61, 0x22, 0xDD, 0x9E, 0x0A, 0x0E, 0x48, 0x51, 0x02, 0x41, 0xBE, 0xF3, 0xA2, 0x73, 0x41, 0x27, 0x59, 0xDD, 0x54, 0x76, 0x93, 0x5C, 0x32, 0x96, 0xBF, 0x8D, 0xED, 0x11, 0x25, 0x07, 0x0E, 0x99, 0x71, 0x3A, 0x77, 0xBB, 0x34, 0xC5, 0xD6, 0x51, 0xE8, 0xED, 0x65, 0xC3, 0x15, 0x90, 0x8E, 0x39, 0x73, 0xC6, 0x70, 0x12 };
	static BYTE staticKey2[8][32] = { 0x4e, 0x4, 0x17, 0x10, 0x6b, 0x27, 0x6d, 0xba, 0xb, 0x67, 0xb9, 0xd0, 0xe0, 0x79, 0xcc, 0x5d, 0x21, 0xf2, 0xe1, 0x62, 0xd3, 0x4f, 0x31, 0x85, 0x6e, 0x4c, 0x60, 0x4, 0x72, 0x6b, 0x7, 0xf0, 0x51, 0x1f, 0x42, 0xce, 0xa5, 0xe1, 0x8, 0xf2, 0x28, 0xb1, 0x20, 0xcb, 0xdb, 0xd4, 0x98, 0x7a, 0xdb, 0xbe, 0xfc, 0xd8, 0x52, 0xfd, 0xad, 0x5a, 0x72, 0x29, 0xb0, 0xdd, 0x0, 0x84, 0xbc, 0x66, 0xc3, 0xfd, 0x6a, 0xcc, 0x0, 0xa5, 0x49, 0xdd, 0xac, 0xc2, 0xbf, 0xfb, 0x9d, 0xae, 0xf8, 0x2d, 0xdd, 0x95, 0x48, 0x46, 0x5c, 0x45, 0x3a, 0xf5, 0x32, 0x21,  0xcc, 0x2, 0xa2, 0x7c, 0xf8, 0xeb, 0x9f, 0x57, 0x86, 0xc4, 0x69, 0x44, 0x66, 0x2, 0x29, 0x7, 0xef, 0xf7, 0xfd, 0x1b, 0xd2, 0x68, 0x71, 0xbb, 0x9c, 0xbc, 0xd2, 0x1c, 0xd, 0x58, 0x50, 0xe9, 0xae, 0xa1, 0xe8, 0x4f, 0x61, 0x2d, 0x0, 0x76, 0xd2, 0x25, 0x37, 0x5e, 0x16, 0xd6, 0xd2, 0x0, 0xcc, 0xd, 0xbb, 0x81, 0x8d, 0x12, 0x6, 0x5, 0x97, 0x7, 0xfd, 0xce, 0x61, 0x74, 0x93, 0xd0, 0x16, 0x23, 0xbe, 0x50, 0xa1, 0xcb, 0xa3, 0xbc, 0xcc, 0x90, 0xa2, 0x6b, 0x15, 0x4b, 0x82, 0xc3, 0x35, 0xb9, 0xf6, 0x1e, 0x16, 0x2, 0x2c, 0x5d, 0x19, 0x22, 0x87, 0x80, 0x72, 0x30, 0xe6, 0x33, 0x82, 0x1f, 0x79, 0x24, 0xe6, 0xda, 0xf0, 0x9e, 0xb8, 0x5e, 0xc5, 0x36, 0xa2, 0x3e, 0x35, 0x7a, 0xcf, 0x31, 0x36, 0xfe, 0x5e, 0x88, 0x1a, 0xc2, 0x48, 0xbb, 0xfb, 0x24, 0x84, 0xe6, 0xd9, 0x5, 0xbc, 0xe4, 0x4f, 0xc4, 0xe5, 0x61, 0xed, 0x24, 0x1d, 0x16, 0xa1, 0x5d, 0x1, 0x6, 0xb, 0xe1, 0x83, 0xdb, 0xe2, 0x89, 0xd8, 0xe6, 0xa9, 0x47, 0x8d, 0x3b, 0xd0, 0x3, 0xda, 0x69, 0x9d, 0x4e, 0xcc, 0xf2, 0xdc, 0xfe, 0xd3, 0xdc };
	static BYTE staticKey3[8][32] = { 0x88, 0xc6, 0xea, 0xb, 0xfb, 0xe7, 0x37, 0x89, 0xf7, 0xa9, 0x83, 0xd1, 0x2, 0xa3, 0x28, 0x0, 0xdc, 0xca, 0xc3, 0x1d, 0x33, 0x58, 0xc9, 0x12, 0xe9, 0x20, 0x74, 0x8e, 0x95, 0x1e, 0x55, 0xbd, 0x69, 0xd, 0xad, 0xc, 0xb, 0xab, 0xbc, 0xd1, 0xc5, 0xad, 0xa, 0xc, 0x66, 0xe6, 0x9, 0x10, 0x45, 0xb1, 0x7, 0x72, 0xc3, 0x18, 0xbd, 0xda, 0xd8, 0xd8, 0x83, 0x29, 0xda, 0xb4, 0x6e, 0x97, 0x1b, 0x20, 0x5f, 0x3c, 0x94, 0xd9, 0x63, 0x9f, 0x55, 0x74, 0x31, 0xad, 0xa5, 0x52, 0x66, 0xe1, 0xc6, 0x21, 0x25, 0xd3, 0x56, 0x56, 0xb8, 0x13, 0xef, 0xc5, 0x2, 0x6a, 0x66, 0xca, 0x1c, 0x8b, 0x4, 0x2, 0x3a, 0x35, 0x18, 0x3f, 0xf6, 0xbf, 0x81, 0xa8, 0x33, 0x4f, 0x4b, 0x7d, 0xb0, 0x5d, 0xf2, 0xb0, 0x9a, 0x10, 0xc7, 0xb3, 0x11, 0xf6, 0xad, 0xa6, 0xe7, 0x96, 0xa8, 0x4c, 0xd4, 0x70, 0xd5, 0xdd, 0xfa, 0xef, 0xd3, 0x59, 0x58, 0xe5, 0x72, 0x20, 0xd1, 0xe2, 0xa8, 0xab, 0x67, 0x4f, 0x9b, 0x1e, 0x6a, 0x58, 0xc0, 0x7e, 0x23, 0xb4, 0xd8, 0x65, 0xc0, 0x5c, 0xda, 0xcd, 0x17, 0xc, 0x8d, 0x7, 0xeb, 0xd8, 0xd5, 0x42, 0x65, 0xad, 0x8e, 0xd6, 0x58, 0xcf, 0xde, 0xc1, 0xb, 0x6e, 0xdf, 0x50, 0x22, 0x46, 0xaf, 0xa8, 0x53, 0x57, 0x83, 0x10, 0x98, 0xd0, 0xfc, 0x8f, 0x6c, 0x6, 0x68, 0xf7, 0xde, 0xbd, 0xee, 0xc3, 0x5, 0x9a, 0x8c, 0xf2, 0x97, 0xd9, 0xcf, 0x56, 0x25, 0x56, 0x28, 0x58, 0xd3, 0xdc, 0xca, 0xce, 0x12, 0xd9, 0x4, 0xdf, 0x9, 0x6f, 0xd8, 0x75, 0x63, 0xef, 0xf4, 0x53, 0x29, 0xd5, 0xbc, 0x4d, 0x2e, 0x1a, 0x66, 0xbf, 0xe9, 0x38, 0x2b, 0xa1, 0x4c, 0x92, 0x1e, 0x68, 0x1e, 0x8b, 0x13, 0x33, 0xcf, 0x1b, 0xfa, 0x35, 0x2a, 0x20, 0x0, 0xd, 0x91, 0x47 };
	static BYTE staticKey4[8][32] = { 0xe6, 0xce, 0x2d, 0xd3, 0x3f, 0xa1, 0xae, 0xf3, 0x31, 0xdc, 0x8c, 0x62, 0x9b, 0xec, 0xda, 0x4a, 0x45, 0x42, 0xcc, 0x32, 0xc0, 0xfc, 0xa7, 0xaf, 0x88, 0x8a, 0x33, 0xcd, 0xea, 0x53, 0xb9, 0x9b, 0x61, 0x5c, 0x4b, 0xa, 0x30, 0x18, 0x6b, 0x8a, 0xa1, 0x32, 0xc4, 0x7a, 0xf0, 0x9f, 0x28, 0xe1, 0x18, 0xb6, 0x5c, 0x9d, 0x85, 0x9b, 0xf9, 0x59, 0x39, 0xbf, 0xfc, 0x96, 0xe8, 0xd4, 0x29, 0xb, 0xee, 0xdc, 0xeb, 0x92, 0x9f, 0xa9, 0xf9, 0x23, 0x8b, 0xa1, 0x22, 0xc5, 0xb1, 0x0, 0xa5, 0x36, 0xc7, 0x68, 0xbc, 0x11, 0xc9, 0xfb, 0x4a, 0x32, 0xfa, 0xe0, 0x97, 0x3e, 0x70, 0x4c, 0x19, 0xe5, 0x67, 0x1d, 0xfd, 0xe2, 0x6b, 0x4, 0xa0, 0x40, 0xa4, 0x49, 0x85, 0xe6, 0x39, 0x32, 0x28, 0x14, 0x2e, 0xe, 0xfc, 0x97, 0xa9, 0x4, 0x37, 0x8f, 0xa, 0x81, 0x80, 0x7a, 0xa1, 0x8d, 0x3f, 0x51, 0xa2, 0xc2, 0xb5, 0x9c, 0xf7, 0x39, 0x60, 0xd, 0xa9, 0x32, 0x19, 0xa2, 0x6f, 0xc4, 0x43, 0xed, 0x26, 0x4c, 0x7a, 0x65, 0xcd, 0xf4, 0x2c, 0x73, 0xa6, 0x1f, 0x89, 0x25, 0x29, 0xdd, 0x1a, 0x13, 0x83, 0x69, 0x8f, 0x82, 0x34, 0xc1, 0xa, 0x60, 0x60, 0x4d, 0x4b, 0xef, 0xc2, 0xaa, 0x5b, 0xe2, 0x9c, 0xaa, 0xd4, 0xcf, 0x6b, 0x81, 0x5e, 0x82, 0x19, 0x1d, 0xc2, 0x47, 0x39, 0xe1, 0xea, 0x9e, 0xf7, 0x8d, 0x56, 0x86, 0x90, 0x85, 0x32, 0xb2, 0x92, 0x71, 0xd1, 0xdf, 0x2b, 0x44, 0x98, 0xb9, 0x7b, 0x9d, 0xf5, 0x60, 0x36, 0xbe, 0xc5, 0x12, 0xb2, 0xc7, 0x87, 0xa, 0x88, 0xae, 0xbf, 0xe, 0xf2, 0x96, 0x10, 0xbb, 0xa, 0xcb, 0x39, 0x2a, 0x1c, 0x5f, 0xaf, 0xb8, 0x25, 0x53, 0xeb, 0xe0, 0xbe, 0x7f, 0x11, 0xbc, 0x6f, 0x2c, 0x27, 0x13, 0xce, 0x52, 0x7e, 0xc1, 0x5d, 0xbd, 0x6f, 0x20 };
	static BYTE staticKey5[8][32] = { 0xcc, 0x7c, 0xc, 0x9c, 0x88, 0x67, 0xc6, 0x62, 0xc4, 0x63, 0xa6, 0xbe, 0x95, 0xcc, 0x31, 0x86, 0xa5, 0x95, 0x5e, 0x23, 0x42, 0xb1, 0x2e, 0x6a, 0xfc, 0xd9, 0xa0, 0x4d, 0x91, 0x86, 0xec, 0xde, 0x2a, 0xa2, 0x6c, 0x32, 0xc4, 0x25, 0x13, 0x86, 0xbf, 0xe3, 0xe0, 0x90, 0x51, 0xe7, 0x4d, 0xf4, 0x9, 0x85, 0xde, 0xf5, 0xf4, 0x78, 0x16, 0x81, 0xb0, 0x70, 0xde, 0x55, 0xe8, 0x21, 0x90, 0xae, 0x88, 0x47, 0xd9, 0x2d, 0x3c, 0xd5, 0x7f, 0x2f, 0x68, 0xab, 0x5a, 0x7c, 0x42, 0xdc, 0x6d, 0x8e, 0x32, 0x3e, 0xa3, 0x26, 0x29, 0x0, 0xdf, 0x83, 0x4, 0x1f, 0x7, 0x15, 0xb5, 0x74, 0xb6, 0xcb, 0x68, 0x2a, 0x45, 0xf0, 0x11, 0x60, 0xaf, 0xbf, 0xcf, 0xc6, 0x29, 0x7b, 0xb7, 0xf7, 0xf4, 0x73, 0x8, 0x56, 0x25, 0x83, 0xa4, 0x3e, 0xed, 0x87, 0xe6, 0x43, 0x32, 0x7d, 0x6d, 0x62, 0x9, 0x89, 0xf, 0xb6, 0xac, 0xc6, 0xa5, 0xca, 0xc4, 0x8, 0xcf, 0xe9, 0x69, 0x72, 0x4f, 0xab, 0xcb, 0x2a, 0x32, 0xf, 0xe4, 0xca, 0x70, 0x4f, 0x29, 0x10, 0x1b, 0xeb, 0x84, 0x65, 0xca, 0xf1, 0xaf, 0xa3, 0x88, 0x6, 0x15, 0xd9, 0xad, 0x45, 0x71, 0x3a, 0x8, 0x73, 0x49, 0x2a, 0xe6, 0x91, 0x5e, 0xa2, 0x26, 0x50, 0x6a, 0xa1, 0xe1, 0x77, 0x3, 0x4, 0x23, 0xc7, 0x2c, 0x87, 0xd7, 0x4f, 0x5f, 0x40, 0xb1, 0xcc, 0x8e, 0x48, 0x20, 0x27, 0xe9, 0xf3, 0xe1, 0x68, 0xf1, 0x51, 0xab, 0x6c, 0xa9, 0x31, 0x1b, 0xa7, 0x49, 0x94, 0x97, 0x24, 0x77, 0xb5, 0x4b, 0x31, 0x58, 0x8b, 0xdf, 0xd1, 0x55, 0xe3, 0x27, 0x72, 0x2a, 0xc, 0x3f, 0xf2, 0xeb, 0x3c, 0x8e, 0x78, 0x9f, 0x86, 0xe, 0x27, 0x2b, 0x97, 0x18, 0x50, 0x1a, 0x15, 0x70, 0xe8, 0xa, 0xdf, 0xa9, 0x2a, 0x45, 0x2, 0x7a, 0xf7, 0x54, 0x84 };
	static BYTE staticKey6[8][32] = { 0x77, 0x78, 0x12, 0x67, 0xa1, 0xd0, 0x96, 0xe8, 0x73, 0x8d, 0xf7, 0x55, 0x82, 0x71, 0xf0, 0xda, 0x87, 0xeb, 0xf0, 0x77, 0x20, 0xf1, 0x1f, 0x1d, 0xe7, 0xdb, 0x74, 0x53, 0x38, 0x47, 0xaf, 0xb6, 0x4a, 0xbe, 0xe5, 0x41, 0x16, 0x59, 0x6e, 0xab, 0x9f, 0xd4, 0x15, 0xfa, 0x1c, 0x76, 0x2b, 0x7d, 0x79, 0x41, 0x63, 0x2e, 0xeb, 0xc3, 0x31, 0xdf, 0xeb, 0x10, 0x7c, 0x32, 0x32, 0xf8, 0x5b, 0x83, 0x41, 0x77, 0xbf, 0xfa, 0xa, 0x10, 0xbc, 0x1, 0x1d, 0x26, 0x40, 0x0, 0x8, 0x36, 0xf8, 0xea, 0x5d, 0xba, 0x59, 0x67, 0xe6, 0xd0, 0x31, 0xf8, 0x9b, 0x5a, 0x6, 0x36, 0xcd, 0x9f, 0x96, 0x4d, 0x37, 0x5c, 0xaa, 0xc0, 0x36, 0x2, 0x99, 0xec, 0x17, 0x2f, 0xa7, 0x12, 0x7c, 0x57, 0xbc, 0x4c, 0x1, 0x3d, 0x71, 0x94, 0x2, 0x8c, 0x9, 0xeb, 0x96, 0xf, 0x54, 0xd1, 0xf4, 0xc1, 0x15, 0x3f, 0xe3, 0x76, 0xd0, 0x46, 0xc1, 0x5, 0xbd, 0x7f, 0xa5, 0xe7, 0xa6, 0x63, 0x19, 0x3e, 0x7b, 0xcc, 0x14, 0xf1, 0x6c, 0xaf, 0x8f, 0x32, 0x43, 0x51, 0x6a, 0xcb, 0xd4, 0xfa, 0xf5, 0xab, 0x2d, 0x8d, 0xf3, 0x11, 0x8a, 0xc4, 0x36, 0xbb, 0x8e, 0xcb, 0xc9, 0x92, 0xbd, 0xb6, 0xdf, 0x1d, 0xdf, 0xa7, 0x33, 0x4a, 0x35, 0x38, 0x55, 0xc2, 0x1d, 0xba, 0x8c, 0x75, 0x1b, 0x38, 0x8e, 0x6f, 0xe2, 0x72, 0xf0, 0xc4, 0x58, 0x7, 0x85, 0x88, 0x18, 0xed, 0x6f, 0xb2, 0x93, 0x4f, 0x40, 0xed, 0x41, 0x1a, 0xdb, 0xfd, 0xdd, 0x35, 0x76, 0xa, 0x73, 0xce, 0x55, 0x34, 0xe5, 0x92, 0xda, 0xed, 0xde, 0x33, 0x53, 0x6d, 0xdb, 0x55, 0xa, 0x9e, 0xc, 0xe, 0x61, 0x1f, 0xf9, 0xfc, 0xe, 0x70, 0x9d, 0x6a, 0x7c, 0x14, 0xa3, 0x39, 0x80, 0x9a, 0xcb, 0x36, 0xd, 0x7e, 0x18, 0x9b, 0x68, 0xc7, 0x73, 0x19 };
//	g_Log.Add(CLog::Blue, "[%s] In [%x]", __FUNCTION__, packet[0]);
	UINT packetCounter = pSocket->packetCounter;
	if(m_Version == 1)
	{
		for(UINT n=0;n<packetSize;n++)
		{
			packet[n] ^= staticKey1[packetCounter%8][n%32];
		}
	}else if(m_Version == 2)
	{
		for(UINT n=0;n<packetSize;n++)
		{
			packet[n] ^= staticKey2[(n+packetSize)%8][n%32];
		}
	}else if(m_Version == 3)
	{
		for(UINT n=0;n<packetSize;n++)
		{
			packet[n] ^= staticKey3[(n+packetSize)%8][n%32];
		}
	}else if(m_Version == 4)
	{
		for(UINT n=0;n<packetSize;n++)
		{
			packet[n] ^= staticKey4[(n+packetSize)%8][n%32];
		}
	}else if(m_Version == 5)
	{
		for(UINT n=0;n<packetSize;n++)
		{
			packet[n] ^= staticKey5[(n+packetSize)%8][n%32];
		}
	}else if(m_Version == 6)
	{
		for(UINT n=0;n<packetSize;n++)
		{
			packet[n] ^= staticKey6[(n+packetSize)%8][n%32];
		}
	}
//	g_Log.Add(CLog::Blue, "[%s] In2 [%x]", __FUNCTION__, packet[0]);
	packetCounter++;
	pSocket->packetCounter = packetCounter;

	unguard;
}

void CCliExt::TimerExpired(CUserSocket *pSocket)
{
	guard;
	if(m_Enabled)
	{
		UINT currentTick = GetTickCount();
		PingEx& pingEx = pSocket->pingEx;
		if(pingEx.pingTimeout > 0)
		{
			if(pingEx.pingTimeout < time(0))
			{
				CHAR sHWID[33];
				memset(sHWID, 0, 33);
				memcpy(sHWID, pSocket->hardwareId.hash, 32);
				//ping timed out - disconnect
				if(User *pUser = pSocket->pUser->CastUser())
				{
					g_Log.Add(CLog::Error, "[CliExt] Ping timed out[%d][%d] User[%S] HWID[%s] - disconnected.", pingEx.pingTimeout, time(0), pUser->pSD->wName, sHWID);
					g_CliExtLog.Add(L"Ping timed out[%u][%u] User[%s] HWID[%S] - disconnected.", pingEx.pingTimeout, time(0), pUser->pSD->wName, sHWID);
				}else
				{
					g_Log.Add(CLog::Error, "[CliExt] Ping timed out[%d][%d] AccountId[%d] HWID[%s] - disconnected", pingEx.pingTimeout, time(0), pSocket->accountId, sHWID);
					g_CliExtLog.Add(L"Ping timed out[%u][%u] AccountId[%d] HWID[%S] - disconnected", pingEx.pingTimeout, time(0), pSocket->accountId, sHWID);
				}
				pSocket->ForceClose();
			}else if(pingEx.tickId < currentTick)
			{
				//send another ping packet but doesn't increase ping timeout
				//send PingEx packet
				pingEx.SetTickId(currentTick + 30000);

				CHAR buff[8190];
				int len = Assemble(buff, 8190, "cdd", 0x8D, 0x01, pingEx.tickId);
				pSocket->Send("b", len, buff);
			}
		}else
		{
			if(pingEx.tickId < currentTick)
			{
				//send PingEx packet
				pingEx.SetTickId(currentTick + 30000);
				pingEx.pingTimeout = time(0) + 120;
				CHAR buff[8190];
				int len = Assemble(buff, 8190, "cdd", 0x8D, 0x01, pingEx.tickId);
				pSocket->Send("b", len, buff);

				if(pSocket->hardwareId.IsNull())
				{
					CHAR buffEx[8190];
					int lenEx = Assemble(buffEx, 8190, "cd", 0x8D, 0x02);
					pSocket->Send("b", lenEx, buffEx);
				}
			}
		}
	}
	unguard;
}

bool CCliExt::HandleCustomPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;

	bool ret = false;
	UINT opCode = 0;
	packet = Disassemble(packet, "d", &opCode);
	if(opCode < 256)
	{
		ret = m_CustomHandler[opCode](pSocket, packet);
	}

	unguard;
	return ret;
}


bool CPHDummyPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	g_Log.Add(CLog::Error, "[%s] called id[%d] - ip[%d.%d.%d.%d]", __FUNCTION__, &packet[-4], pSocket->address.S_un.S_un_b.s_b1, pSocket->address.S_un.S_un_b.s_b2, pSocket->address.S_un.S_un_b.s_b3, pSocket->address.S_un.S_un_b.s_b4);
	unguard;
	return false;
}

bool CPHPingExPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = false;

	USHORT buffSize = 0;
	UINT tickId = 0, localTick = 0, localCheckTick = 0;
	UINT requestQuit = 0;
	WCHAR moduleName[255] = { 0 };
	packet = Disassemble(packet, "h", &buffSize);

	if(buffSize < 4096)
	{
		BYTE packetData[8190] = { 0 };
		for(UINT n=0;n<buffSize;n++)
		{
			packetData[n] = packet[n];
		}
		CryptXor(packetData, buffSize);

		Disassemble(packetData, "ddddS", &localTick, &tickId, &localCheckTick, &requestQuit, sizeof(moduleName), moduleName);

		PingEx& pingEx = pSocket->pingEx;

//		g_Log.Add(CLog::Blue, "[%s] Received ping[%u]", __FUNCTION__, tickId);

		if(requestQuit)
		{
			CHAR sHWID[33];
			memset(sHWID, 0, 33);
			memcpy(sHWID, pSocket->hardwareId.hash, 32);
			
			if(User *pUser = pSocket->pUser->CastUser())
			{
				g_Log.Add(CLog::Error, "[CliExt] Detected illegal action - User[%S] HWID[%s] reason[%d] module[%S]", pUser->pSD->wName, sHWID, requestQuit, moduleName);
				g_CliExtLog.Add(L"Detected illegal action - User[%s] HWID[%S] reason[%d] module[%s]", pUser->pSD->wName, sHWID, requestQuit, moduleName);
			}else
			{
				g_Log.Add(CLog::Error, "[CliExt] Detected illegal action - AccountId[%d] HWID[%s] reason[%d] module[%S]", pSocket->accountId, sHWID, requestQuit, moduleName);
				g_CliExtLog.Add(L"Detected illegal action - AccountId[%d] HWID[%S] reason[%d] module[%s]", pSocket->accountId, sHWID, requestQuit, moduleName);
			}
			ret = true;
		}else
		{
			if(pingEx.ValidTick(tickId) == false)
			{
				CHAR sHWID[33];
				memset(sHWID, 0, 33);
				memcpy(sHWID, pSocket->hardwareId.hash, 32);
				
				if(User *pUser = pSocket->pUser->CastUser())
				{
					g_Log.Add(CLog::Error, "[CliExt] Invalid PingEx tickId - User[%S] HWID[%s]", pUser->pSD->wName, sHWID);
					g_CliExtLog.Add(L"Invalid PingEx tickId[%d] vs [%d][%d][%d][%d] - User[%s] HWID[%S]", tickId, pingEx.tickId, pingEx.tickId2, pingEx.tickId3, pingEx.tickId4, pUser->pSD->wName, sHWID);
				}else
				{
					g_Log.Add(CLog::Error, "[CliExt] Invalid PingEx tickId - AccountId[%d] HWID[%s]", pSocket->accountId, sHWID);
					g_CliExtLog.Add(L"Invalid PingEx tickId - AccountId[%d] HWID[%S]", pSocket->accountId, sHWID);
				}
				ret = true;
			}

			if( localCheckTick > (localTick + 2000) || (localTick - (localCheckTick - 2000)) > 105000 )
			{
				CHAR sHWID[33];
				memset(sHWID, 0, 33);
				memcpy(sHWID, pSocket->hardwareId.hash, 32);
				
				if(User *pUser = pSocket->pUser->CastUser())
				{
					g_Log.Add(CLog::Error, "[CliExt] Invalid local ticks[%u][%u] - User[%S] HWID[%s]", localTick, localCheckTick, pUser->pSD->wName, sHWID);
					g_CliExtLog.Add(L"Invalid local ticks[%u][%u] - User[%s] HWID[%S]", localTick, localCheckTick, pUser->pSD->wName, sHWID);
				}else
				{
					g_Log.Add(CLog::Error, "[CliExt] Invalid local ticks[%u][%u] - AccountId[%d] HWID[%s]", localTick, localCheckTick, pSocket->accountId, sHWID);
					g_CliExtLog.Add(L"Invalid local ticks[%u][%u] - AccountId[%d] HWID[%S]", localTick, localCheckTick, pSocket->accountId, sHWID);
				}
				ret = true;
			}

			if(pingEx.localCheckTick > 0 && pingEx.localTick > 0)
			{
				if( ( localCheckTick - pingEx.localCheckTick ) > 120000 || (localTick - pingEx.localTick) > 120000 )
				{
					CHAR sHWID[33];
					memset(sHWID, 0, 33);
					memcpy(sHWID, pSocket->hardwareId.hash, 32);
					
					if(User *pUser = pSocket->pUser->CastUser())
					{
						g_Log.Add(CLog::Error, "[CliExt] Outdated local ticks[%u][%u] [%u][%u] - User[%S] HWID[%s]", localTick, localCheckTick, pingEx.localTick, pingEx.localCheckTick, pUser->pSD->wName, sHWID);
						g_CliExtLog.Add(L"Outdated local ticks[%u][%u] [%u][%u] - User[%s] HWID[%S]", localTick, localCheckTick, pingEx.localTick, pingEx.localCheckTick, pUser->pSD->wName, sHWID);
					}else
					{
						g_Log.Add(CLog::Error, "[CliExt] Outdated local ticks[%u][%u] [%u][%u] - AccountId[%d] HWID[%s]", localTick, localCheckTick, pingEx.localTick, pingEx.localCheckTick, pSocket->accountId, sHWID);
						g_CliExtLog.Add(L"Outdated local ticks[%u][%u] [%u][%u] - AccountId[%d] HWID[%S]", localTick, localCheckTick, pingEx.localTick, pingEx.localCheckTick, pSocket->accountId, sHWID);
					}
					ret = true;
				}
			}
		}

		pingEx.localTick = localTick;
		pingEx.localCheckTick = localCheckTick;
		pingEx.pingTimeout = 0;
	}else
	{
		g_Log.Add(CLog::Error, "[%s] Too big packet in ping reply[%d] Account[%d] HWID[%S]", __FUNCTION__, buffSize, pSocket->accountId, pSocket->hardwareId.ToString().c_str());
	}
	unguard;
	return ret;
}

bool CPHHWIDPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = false;
	
	if(pSocket->shortHWID == 0)
	{
		packet = Disassemble(packet, "d", &pSocket->shortHWID);
		memcpy(pSocket->hardwareId.hash, packet, 32);
	}
	
	unguard;
	return ret;
}
