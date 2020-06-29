#pragma once

extern "C"
{
	void PacketDecryptHook();
	void PacketDecryptWrapper(LPBYTE packet, CUserSocket *pSocket, UINT size);
	void SendKeyPacketHook();
	void SendKeyPacketWrapper(CUserSocket* pSocket, const char* format, BYTE opCode, BYTE param1, UINT64 key, DWORD param2, DWORD param3, BYTE param4, DWORD param5);
};

class PacketCryption
{
public:
	static void Init();
};