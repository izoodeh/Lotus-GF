#include "stdafx.h"
#include "AdminPacketHandler.h"
#include "DBConn.h"
#include "Query.h"

CAdminPacketHandler g_AdminPacketHandler;

CAdminPacketHandler::CAdminPacketHandler()
{
}

CAdminPacketHandler::~CAdminPacketHandler()
{
}

void CAdminPacketHandler::Init()
{
	AdminCallback *lpBase = (AdminCallback*)0x6A31E0;
	for(UINT n=0;n<256;n++)
	{
		if(n < 150)
		{
			m_Handler[n] = lpBase[n];
		}else
		{
			m_Handler[n] = AdminDummyPacket;
		}
	}

	Bind(150, AdminTestPacket);
	Bind(151, AdminSetItemLock);

	//Fix max allowed handler_id
	//.text:0000000000440928                 cmp     byte ptr [r13+0], 96h
	WriteBYTE(0x44092C, 152);
	//Replace old packet table
	//.text:00000000004087BC                 lea     rax, AdminPacketTable
	WriteDWORD(0x4087BF, static_cast<UINT>((UINT64)&m_Handler[0] - 0x4087C3));
}

void CAdminPacketHandler::Bind(UINT id, AdminCallback handler)
{
	if(id < 256)
	{
		m_Handler[id] = handler;
	}
}

//Reply format - confirmation packet (cdd) 0xOP_CODE, (1 - succeeded, 0 - failed), (error code)

bool AdminSetItemLock(CAdminSocket *pSocket, const unsigned char* packet)
{
	if(pSocket->echoPacket)
	{
		pSocket->Send("cdd", 151, 0, 30);
	}else
	{
		UINT charId = 0;
		BOOL lockState = 0;
		WCHAR wPassword[25];
		Disassemble(packet, "ddS", &charId, &lockState, sizeof(wPassword), wPassword);

		if(charId > 0)
		{
			CQuery query(L"{ Call dbo.lin_ItemLockSetPassword (?, '?', ?) }");
			query.AddParam(charId);
			query.AddParam(wPassword);
			query.AddParam(lockState);
			CDBConn* pCon = new CDBConn();
			pCon->PrepareStatement(query.Get());
			pCon->ExecutePrepared(query.Get());
			delete pCon;

			CSocket** lpL2ServerSocket = (CSocket**)0x2739628L;
			if(CSocket* pServerSocket = (*lpL2ServerSocket))
			{
				pServerSocket->Send("chdSd", 0x1E, CO_REPLY_LOAD_ITEM_LOCK, charId, wPassword, lockState);
			}
		}

		pSocket->Send("cdd", 151, 1, 0);
	}
	return false;
}

bool AdminTestPacket(CAdminSocket *pSocket, const unsigned char* packet)
{
	if(pSocket->echoPacket)
	{
		pSocket->Send("cdd", 150, 0, 30);
	}else
	{
		UINT seed = 0;
		Disassemble(packet, "d", &seed);

		g_Log.Add(CLog::Blue, L"[%s] called id[%d] seed[%d]", __FUNCTIONW__, packet[-1], seed);
		pSocket->Send("cdd", 150, 1, 0);
	}
	return false;
}

bool AdminDummyPacket(CAdminSocket *pSocket, const unsigned char* packet)
{
	g_Log.Add(CLog::Error, L"[%s] DummyPacket called id[%d]", __FUNCTIONW__, packet[-1]);
	return false;
}