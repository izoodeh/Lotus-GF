#pragma once

class CClientSocket
{
public:
	/* 00 */ UINT _unkn00[18];
	/* 48 */ BYTE inBuff[0x4000];
	void Send(int len, char * buff);
	static void Init();
	static void __thiscall Crypt(CClientSocket *pSocket, LPBYTE lpBuff, LPBYTE lpKey, INT32 size);
	static int __cdecl OnCSPacket(LPBYTE buff, int buffSize);
	static void OnKeyPacket(LPBYTE lpPacket);
};