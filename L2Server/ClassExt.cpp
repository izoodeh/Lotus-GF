#include "StdAfx.h"
#include "ClassExt.h"
#include "HookManager.h"

CClassExt g_ClassExt;

BYTE g_NpcBitFlag[NPC_CLASS_COUNT][0x4E4];

CClassExt::CClassExt(void) : extendNpcClassCount(FALSE)
{
}

CClassExt::~CClassExt(void)
{
}

void CClassExt::Init()
{
	ExtendNpcClassCount();
	ExtendUserSocket();
	ExtendUser();
}

void CClassExt::ExtendUser()
{
	UINT newSize = sizeof(User);

	WriteDWORD(0x5E1869, newSize);
	WriteDWORD(0x5AEF0E, newSize);
	
	if(offsetof(User, User::pEUD) != 0x3A10)
	{
		ShowMessage(L"Extender", L"Invalid User size!");
	}
	extendUserSocket = TRUE;
}

void CClassExt::ExtendUserSocket()
{
	UINT newSize = sizeof(CUserSocket);

	WriteDWORD(0x93CEC1, newSize);
	WriteDWORD(0x91BFEE, newSize);
	
	if(offsetof(CUserSocket, CUserSocket::packetTimestamp) != 0xFA0)
	{
		ShowMessage(L"Extender", L"Invalid CUserSocket size!");
	}
	extendUserSocket = TRUE;
}

void CClassExt::ExtendNpcClassCount()
{
	//.text:0000000000629E52                 cmp     dword ptr [rbx+9Ch], 270Fh
	WriteDWORD(0x629E58, (NPC_CLASS_COUNT - 1));
	//.text:0000000000643F3C                 cmp     dword ptr [rbx+40h], 270Fh
	WriteDWORD(0x643F3F, (NPC_CLASS_COUNT - 1));
	//.text:00000000006AE32E                 cmp     r11d, 270Fh
	WriteDWORD(0x6AE331, (NPC_CLASS_COUNT - 1));
	//.text:00000000007A76D6                 cmp     dword ptr [rdi+3Ch], 270Fh
	WriteDWORD(0x7A76D9, (NPC_CLASS_COUNT - 1));
	//.text:00000000007A9AA0                 cmp     dword ptr [rbx+44h], 270Fh
	WriteDWORD(0x7A9AA3, (NPC_CLASS_COUNT - 1));
	//.text:0000000000A268B2                 mov     edi, 270Fh
	WriteDWORD(0xA268B3, (NPC_CLASS_COUNT - 1));
	//.text:0000000000443D6A                 cmp     eax, 2710h
	WriteDWORD(0x443D6B, NPC_CLASS_COUNT);
	//.text:00000000005354A4                 cmp     edx, 2710h
	WriteDWORD(0x5354A6, NPC_CLASS_COUNT);
	//.text:00000000006AE4B9                 mov     [rsp+78h+var_48], 2710h
	WriteDWORD(0x6AE4BD, NPC_CLASS_COUNT);
	//.text:000000000071E051                 cmp     eax, 2710h
	WriteDWORD(0x71E052, NPC_CLASS_COUNT);
	//.text:000000000077C2B5                 cmp     ecx, 2710h
	WriteDWORD(0x77C2B7, NPC_CLASS_COUNT);
	//.text:000000000077CD43                 cmp     ebp, 2710h
	WriteDWORD(0x77CD45, NPC_CLASS_COUNT);
	//.text:000000000077DC3F                 cmp     r9d, 2710h
	WriteDWORD(0x77DC42, NPC_CLASS_COUNT);
	//.text:000000000077DD44                 cmp     eax, 2710h
	WriteDWORD(0x77DD45, NPC_CLASS_COUNT);
	//.text:00000000007C7EF7                 cmp     ebp, 2710h
	WriteDWORD(0x7C7EF9, NPC_CLASS_COUNT);
	//.text:00000000007C95D4                 cmp     eax, 2710h
	WriteDWORD(0x7C95D5, NPC_CLASS_COUNT);
	//.text:0000000000860E47                 cmp     ebp, 2710h
	WriteDWORD(0x860E49, NPC_CLASS_COUNT);
	//.text:00000000008619F0                 cmp     eax, 2710h
	WriteDWORD(0x8619F1, NPC_CLASS_COUNT);
	//.text:0000000000763123                 mov     ecx, 2710h
	WriteDWORD(0x763124, NPC_CLASS_COUNT);

	//.text:00000000005381AA                 lea     rax, unk_A18DC70
	INT64 relative = (INT64)&g_NpcBitFlag[0] - 0x5381B1;
	WriteDWORD(0x5381AD, (DWORD)relative);
	//.text:0000000000721A4C                 lea     rdx, unk_A18DC70
	relative = (INT64)&g_NpcBitFlag[0] - 0x721A53;
	WriteDWORD(0x721A4F, (DWORD)relative);
	//.text:0000000000722EC8                 lea     rax, unk_A18DC70
	relative = (INT64)&g_NpcBitFlag[0] - 0x722ECF;
	WriteDWORD(0x722ECB, (DWORD)relative);
	//.text:0000000000722FE1                 lea     rax, unk_A18DC70
	relative = (INT64)&g_NpcBitFlag[0] - 0x722FE8;
	WriteDWORD(0x722FE4, (DWORD)relative);
	//.text:000000000096EEC5                 lea     rax, unk_A18DC70
	relative = (INT64)&g_NpcBitFlag[0] - 0x96EECC;
	WriteDWORD(0x96EEC8, (DWORD)relative);
	//.text:00000000009703D8                 lea     rax, unk_A18DC70
	relative = (INT64)&g_NpcBitFlag[0] - 0x9703DF;
	WriteDWORD(0x9703DB, (DWORD)relative);
	//.text:0000000000A268AB                 lea     rbx, unk_A18DC70
	relative = (INT64)&g_NpcBitFlag[0] - 0xA268B2;
	WriteDWORD(0xA268AE, (DWORD)relative);

	extendNpcClassCount = TRUE;
}