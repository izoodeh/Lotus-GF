#include "StdAfx.h"
#include "AbnormalStatus.h"
#include "DBConn.h"

bool CAbnormalStatus::RequestSaveAbnormalStatus(CSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, abnormalCount = 0;
	UINT abnormalData[46][4];
	memset(abnormalData, 0, sizeof(abnormalData));
	

	packet = Disassemble(packet, "dd", &charId, &abnormalCount);
	//Format: c ddb
	//b - dddd x nBuffAmount   / SkillID, Level, Seconds, SelfCast - sx lx dx cx
	if( abnormalCount < 47)
	{
		for(UINT n=0; n<abnormalCount; n++)
		{
			packet = Disassemble(packet, "dddd", &abnormalData[n][0], &abnormalData[n][1], &abnormalData[n][2], &abnormalData[n][3]);
		}
	}

	wstringstream strUpdate;
	strUpdate << L"{ CALL dbo.lin_UpdateActiveSkill (" << charId << L", ";

	for(UINT n=0;n<46;n++)
	{
		if(n == 45)
		{
			strUpdate << abnormalData[n][0] << L", " << abnormalData[n][1] << L", " << abnormalData[n][2] << L", " << abnormalData[n][3] << L") }";
		}else
		{
			strUpdate << abnormalData[n][0] << L", " << abnormalData[n][1] << L", " << abnormalData[n][2] << L", " << abnormalData[n][3] << L", ";
		}
	}

	CDBConn *pConn = new CDBConn();
	wstring stm(strUpdate.str());
	pConn->PrepareStatement(stm.c_str());
	if(pConn->ExecutePrepared(stm.c_str()))
	{
	}else
	{
		pConn->UnBind();
		wstringstream strInsert;
		strInsert << L"{ CALL dbo.lin_CreateActiveSkill (" << charId << L", ";

		for(UINT n=0;n<46;n++)
		{
			if(n == 45)
			{
				strInsert << abnormalData[n][0] << L", " << abnormalData[n][1] << L", " << abnormalData[n][2] << L", " << abnormalData[n][3] << L") }";
			}else
			{
				strInsert << abnormalData[n][0] << L", " << abnormalData[n][1] << L", " << abnormalData[n][2] << L", " << abnormalData[n][3] << L", ";
			}
		}
		stm = strInsert.str();
		pConn->PrepareStatement(stm.c_str());
		pConn->ExecutePrepared(stm.c_str());

	}
	delete pConn;
	
	return false;
}

bool CAbnormalStatus::RequestLoadAbnormalStatus(CSocket *pSocket, const unsigned char *packet)
{
	UINT charId = 0, serverId = 0;
	
	packet = Disassemble(packet, "dd", &charId, &serverId);

	UINT abnormalData[46][4];
	UINT abnormalCount = 0;

	memset(abnormalData, 0, sizeof(abnormalData));

	CDBConn *pConn = new CDBConn();

	for(UINT n=0;n<46;n++)
	{
		pConn->Bind(&abnormalData[n][0]);
		pConn->Bind(&abnormalData[n][1]);
		pConn->Bind(&abnormalData[n][2]);
	}

	for(UINT n=0;n<46;n++)
	{
		pConn->Bind(&abnormalData[n][3]);
	}

	wstringstream str;
	str << L"{ CALL dbo.lin_GetActiveSkills (" << charId << L") }";
	wstring stm(str.str());
	BOOL done = FALSE;
	pConn->PrepareStatement(stm.c_str());
	if(pConn->ExecutePrepared(stm.c_str()))
	{
		if(pConn->Fetch())
		{
			done = TRUE;
		}
	}
	delete pConn;

	if(done)
	{
		char buff[8192];
		int len = 0;
		for(UINT n=0;n<46;n++)
		{
			if(abnormalData[n][0] > 0)
			{
				len += Assemble(&buff[len], (8192 - len), "dddd", abnormalData[n][0],  abnormalData[n][1], abnormalData[n][2], abnormalData[n][3]);
				abnormalCount++;
			}
		}
		if(abnormalCount > 0)
		{
			pSocket->Send("cddb", 0x6A, serverId, abnormalCount, len, buff);
		}else
		{
			pSocket->Send("cdd", 0x6A, serverId, 0);
		}
	}else
	{
		pSocket->Send("cdd", 0x6A, serverId, 0);
	}
	
	return false;
}