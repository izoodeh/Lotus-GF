#pragma once
#include "SPointer.h"
#include "PrivateStoreAction.h"

extern "C"
{
	void ValidateObjectInTimer();
	BOOL IsValidPointer(LPDWORD lpPointer);
};

class BaseAction
{
public:
	static void Init();
	static CCreatureSP* OnSetInventoryChanged(LPVOID lpInstance, CCreatureSP* lpCreatureSp, UINT serverId);
	static void OnSetCloseTimer(CUserSocket *pSocket);
	static bool CheckCharacterName(const WCHAR* wName, int code, bool pet);
	static void ActivateSkill(CSkillInfo *pSI, CCreature *pCreature, CObject* pTarget, double smth, CSkillAction2* pAction2, EffectActivateTiming timing, double timeMultipler);
	static bool MoveTo(CWorld* pWorld, CCreature *pCreature, FVector& pos, bool force);
	static bool PutItem(CWorld* pWorld, CItem *pItem, FVector* pPos, UINT serverId, CCreature *pCreature);	//977A68
	static bool PickItem(CWorld* pWorld, CItem *pItem, CCreature *pCreature); //974400
	static void DoAsyncPickItem(CWorld *pWorld, CItem *pItem);
	static void PickAndDeleteItem(CWorld *pWorld, CItem *pItem);
	static void TimerExpired(CItem *pItem, int id);
	static bool OnStart(CPrivateStoreAction *pAction);
	static void OnSendLoadCharactersaPacket(CUserSocket *pSocket, const char* format, BYTE opCode, DWORD param1, DWORD param2, BYTE param3, DWORD buffLen, LPBYTE lpBuff);//cddcb
	static void CParty_OpenAllWindowPacket(CParty *pParty, User *pUser);
	static const unsigned char* SetPrivateStoreBuyListDisassemble(const unsigned char* packet, const char* format, PINT32 pParam1, PSHORT pParam2, PSHORT pParam3, PINT64 pParam4, PINT64 pParam5);
	static void StartShutdownTimer(CShutDownMessage* pMessage, UINT time, UINT timerId);
	static void CUserSocket_TimerExpired(CUserSocket *pSocket, UINT id);
};