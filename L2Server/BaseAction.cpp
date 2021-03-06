#include "StdAfx.h"
#include "BaseAction.h"
#include "UserAction.h"
#include "CreatureAction.h"
#include "NpcAction.h"
#include "UserSocketDB.h"
#include "HookManager.h"
#include "Protector.h"
#include "OfflineShop.h"
#include "EventMatchMgr.h"
#include "ItemDecay.h"
#include "World.h"
#include "Party.h"
#include "CliExt.h"

LPBOOL lpFontCheck = (LPBOOL)0xF0CA7C;
extern vector<pair<WORD, WORD>> g_NamePatterns;
extern INT32 g_ProtocolVersion;
extern UINT g_ShutdownTime;

CUserSocket* CUserSocket_Constructor(CUserSocket *pSocket, SOCKET s)
{
	typedef CUserSocket* (*f)(CUserSocket*, SOCKET);
	pSocket = f(0x93CADC)(pSocket, s);

	/* Initialize new stuff here */
	memset(pSocket->packetTimestamp, 0, sizeof(pSocket->packetTimestamp));
	memset(pSocket->hardwareId.hash, 0, 32);
	pSocket->loadedHardwareId = FALSE;
	pSocket->requestedHardwareId = FALSE;
	pSocket->packetCounter = 0;
	pSocket->packetCS.Init();
	pSocket->pingEx.Init();
	pSocket->shortHWID = 0;

	for(int n=0;n<4;n++)
	{
		pSocket->inKeyEx[n] = 0;
	}
	pSocket->protocolVersion = 0;
	pSocket->dropGetTimestamp = GetTickCount();

	g_UserSocketDB.Add(pSocket);

	/*           end             */
	
	return pSocket;	
}

CUserSocket *CUserSocket_Destructor(CUserSocket *pSocket)
{
	g_UserSocketDB.Del(pSocket);

	typedef CUserSocket*(*f)(CUserSocket*);
	return f(0x92DE7CL)(pSocket);
}

BOOL IsValidPointer(LPDWORD lpPointer)
{
	try
	{
		DWORD value = (*lpPointer);
		if(value > 0x401000 && value < 0xE4B000)
			return TRUE;

	}catch(...)
	{
		g_Log.Add(CLog::Error, "[%s] [%p]!", __FUNCTION__, lpPointer);
	}
	return FALSE;
};

#pragma optimize("", off)
LPBYTE JoinOptionStatUp(unsigned char *p1, unsigned char *p2, int r8, int r9, int rsp20, int rsp28, int rsp30, unsigned char rsp38)
{
	r9 = 0;
	r8 = 0;
	unsigned char v = 0;
	unsigned char *p3 = &rsp38;

	p3[0] = p1[0] + p2[0];
	p3[1] = p1[1] + p2[1];
	p3[2] = p1[2] + p2[2];
	p3[3] = p1[3] + p2[3];
	p3[4] = p1[4] + p2[4];
	p3[5] = p1[5] + p2[5];

	return p3;
}
#pragma optimize("", on)

void BaseAction::Init()
{
	UserAction::Init();
	CreatureAction::Init();
	NpcAction::Init();

	/* Fix the Dance/Song MP Consumption (Min Mp 60) Bug */
	WriteNOP(0x569843, 7);
	//Fix to save is_magic=3 in db
	WriteNOP(0x8F9E37, 7);
	//Save in db when auto_get=true
	WriteNOP(0x8CE8A4, 6);

	WriteJump(0x86DE8A, (UINT64)ValidateObjectInTimer, 2);

	WriteCall(0x95267D, JoinOptionStatUp);
	WriteCall(0x93CEDC, CUserSocket_Constructor);
	WriteCall(0x92E068, CUserSocket_Destructor);
	WriteCall(0x68DF4A, BaseAction::OnSetInventoryChanged);

	WriteCall(0x45A137, BaseAction::OnSetCloseTimer);
	WriteCall(0x45A2BF, BaseAction::OnSetCloseTimer);

	WriteCall(0x4345BF, BaseAction::CheckCharacterName);
	WriteCall(0x7AC989, BaseAction::CheckCharacterName);
	WriteCall(0x7C4FDD, BaseAction::CheckCharacterName);
	WriteCall(0x7CF68C, BaseAction::CheckCharacterName);
	WriteCall(0x7CFE82, BaseAction::CheckCharacterName);
	WriteCall(0x7D0AD6, BaseAction::CheckCharacterName);
	WriteCall(0x9109D2, BaseAction::CheckCharacterName);
	WriteCall(0x9252BD, BaseAction::CheckCharacterName);

	//Used only hooks when it's done by players
	WriteCall(0x6FFB7B, BaseAction::ActivateSkill);
	WriteCall(0x6FFBBB, BaseAction::ActivateSkill);
	WriteCall(0x7109BE, BaseAction::ActivateSkill);
	WriteCall(0x714166, BaseAction::ActivateSkill);
	WriteCall(0x714503, BaseAction::ActivateSkill);
	WriteCall(0x714607, BaseAction::ActivateSkill);
	WriteCall(0x820E4A, BaseAction::ActivateSkill);
	WriteCall(0x820F94, BaseAction::ActivateSkill);
	WriteCall(0x821083, BaseAction::ActivateSkill);
	WriteCall(0x821100, BaseAction::ActivateSkill);
	WriteCall(0x83821A, BaseAction::ActivateSkill);
	WriteCall(0x973A85, BaseAction::ActivateSkill);
	WriteCall(0x973AA7, BaseAction::ActivateSkill);
	WriteCall(0x973ACB, BaseAction::ActivateSkill);

	WriteCall(0x836C8C, BaseAction::MoveTo);
	WriteCall(0x83784D, BaseAction::MoveTo);
	WriteCall(0x8379A5, BaseAction::MoveTo);
	WriteCall(0x84C3B4, BaseAction::MoveTo);
	WriteCall(0x716528, BaseAction::MoveTo);

	WriteCall(0x5C2290, BaseAction::PutItem);
	WriteCall(0x97952A, BaseAction::PutItem);

	WriteCall(0x4F4253, BaseAction::DoAsyncPickItem);	//CCastle::StartSiege

	WriteCall(0x57B5BC, BaseAction::PickAndDeleteItem);	//CCursedWeaponMgr::OnDelete
	WriteCall(0x632A9E, BaseAction::PickAndDeleteItem);	//CFortressSiege::DeleteAllFlags
	WriteCall(0x695FB6, BaseAction::PickAndDeleteItem);	//CItem::TimerExpired

	WriteCall(0x976490, BaseAction::PickItem);	//CWorld::TryPickItem
	WriteCall(0x9766A9, BaseAction::PickItem);	//CWorld::TryPickItem
	WriteCall(0x9768B5, BaseAction::PickItem);	//CWorld::TryPickItem
	WriteCall(0x9768DD, BaseAction::PickItem);	//CWorld::TryPickItem

	WriteNOP(0x8379EF, 5);
	WriteNOP(0x836CE1, 5);
	WriteNOP(0x716545, 5);

	WriteToVMT(VT_CITEM, 0x20, BaseAction::TimerExpired);
	WriteToVMT(VT_CACCESSARY, 0x20, BaseAction::TimerExpired);
	WriteToVMT(VT_CASSET, 0x20, BaseAction::TimerExpired);
	WriteToVMT(VT_CARMOR, 0x20, BaseAction::TimerExpired);
	WriteToVMT(VT_CETCITEM, 0x20, BaseAction::TimerExpired);
	WriteToVMT(VT_CWEAPON, 0x20, BaseAction::TimerExpired);
	WriteToVMT(0xAE0048, 0x18, BaseAction::OnStart);

	WriteCall(0x5D5F59, BaseAction::OnSendLoadCharactersaPacket, 6);

	if(g_ProtocolVersion > 83)
	{
		WriteCall(0x78089E, BaseAction::CParty_OpenAllWindowPacket);
		WriteCall(0x9406FC, BaseAction::CParty_OpenAllWindowPacket);
		WriteCall(0x940707, BaseAction::CParty_OpenAllWindowPacket);
		WriteCall(0x940C52, BaseAction::CParty_OpenAllWindowPacket);
		WriteCall(0x8B2E16, BaseAction::SetPrivateStoreBuyListDisassemble);
		WriteCall(0x8B327A, BaseAction::SetPrivateStoreBuyListDisassemble);
	}

	WriteCall(0x691417, BaseAction::StartShutdownTimer);
	WriteDWORD(0xC746D8, (DWORD)reinterpret_cast<UINT64>(BaseAction::CUserSocket_TimerExpired));

}

void BaseAction::CUserSocket_TimerExpired(CUserSocket *pSocket, UINT id)
{
	guard;
	if(g_CliExt.IsEnabled())
	{
		if(pSocket->status != 2)
		{
			g_CliExt.TimerExpired(pSocket);
		}
	}
	typedef void(*f)(CUserSocket*, UINT);
	f(0x92CA6CL)(pSocket, id);
	unguard;
}

void BaseAction::StartShutdownTimer(CShutDownMessage *pMessage, UINT time, UINT timerId)
{
	pMessage->shutdownTime = g_ShutdownTime;
	pMessage->AddTimer(time, timerId);
}

const unsigned char* BaseAction::SetPrivateStoreBuyListDisassemble(const unsigned char* packet, const char* format, PINT32 pParam1, PSHORT pParam2, PSHORT pParam3, PINT64 pParam4, PINT64 pParam5)
{
	//itemId, damaged, enchanted, amount, price
	//new format dhhQQhhhhhhhh
	WORD h1, h2, h3, h4, h5, h6, h7, h8;
	return Disassemble(packet, "dhhQQhhhhhhhh", pParam1, pParam2, pParam3, pParam4, pParam5, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8);
}

void BaseAction::CParty_OpenAllWindowPacket(CParty *pParty, User *pUser)
{
	guard;
	vector<User*> vMembers;
	pParty->GetAllMembersEx(vMembers);

	CHAR buff[16382];
	int len = 0;
	int memberCount = 0;
	for(UINT n=0;n<vMembers.size();n++)
	{
		if(User *pMember = vMembers[n]->CastUser())
		{
			if(pUser->serverId != pMember->serverId)
			{
				memberCount++;
				len += Assemble(&buff[len], 16382 - len, "dSdddddddddddd", pMember->serverId, pMember->pSD->wName, static_cast<int>(pMember->pSD->cp), static_cast<int>(pMember->pSD->max_cp), static_cast<int>(pMember->pSD->hp), static_cast<int>(pMember->pSD->max_hp), static_cast<int>(pMember->pSD->mp), static_cast<int>(pMember->pSD->max_mp), pMember->pSD->level, pMember->pSD->occupation, pMember->pSD->sex, pMember->pSD->race, 0, 0);
				if(CSummon *pSummon = pMember->GetSummonOrPet())
				{
					len += Assemble(&buff[len], 16382 - len, "dddSddddd", pSummon->serverId, pSummon->pSD->npc_class_id, pSummon->pSD->summon_type, pSummon->wSummonName, static_cast<int>(pSummon->pSD->hp), static_cast<int>(pSummon->pSD->max_hp), static_cast<int>(pSummon->pSD->mp), static_cast<int>(pSummon->pSD->max_mp), pSummon->pSD->level);
				}else
				{
					len += Assemble(&buff[len], 16382 - len, "d", 0);
				}
			}
		}
	}
	pUser->pUserSocket->Send("cdddb", 0x4E, pParty->pSD->partyLeaderId, pParty->itemRoutingType, memberCount, len, buff);
	unguard;
}

void BaseAction::OnSendLoadCharactersaPacket(CUserSocket *pSocket, const char* format, BYTE opCode, DWORD param1, DWORD param2, BYTE param3, DWORD buffLen, LPBYTE lpBuff)
{
	pSocket->Send("cddcb", opCode, param1, param2, param3, buffLen, lpBuff);
	pSocket->Send("cd", Network::OutgoingPacket::BaseId, Network::OutgoingPacket::HardwareId);
	pSocket->requestedHardwareId = TRUE;
}

bool BaseAction::OnStart(CPrivateStoreAction *pAction)
{
	guard;
	if(User *pUser = User::GetUser(&pAction->m_ownerId)->CastUser())
	{
		pUser->DespawnPet();
	}

	typedef bool (*f)(CPrivateStoreAction*);
	bool ret = f(0x6FD0E4L)(pAction); 
	unguard;
	return ret;
}

void BaseAction::TimerExpired(CItem *pItem, int id)
{
	guard;

	if(id == ITEM_DECAY_TIMER)
	{
		if(pItem->ValidItem())
		{
			if(pItem->_unkn20 == 0 && pItem->_unkn24 == 0)
			{
				if(pItem->pSID->canBePickedUp)
				{
					pItem->pSID->canBePickedUp = false;
					g_World.PickAndDeleteItem(pItem);
				}
			}
		}
		unguard;
		return;
	}
	typedef void (*f)(CItem*, int);
	f(0x695F00L)(pItem, id);

	unguard;
}

void BaseAction::PickAndDeleteItem(CWorld *pWorld, CItem *pItem)
{
	g_ItemDecay.OnPickUp(pItem);

	typedef void (*f)(CWorld*, CItem*);
	f(0x971B10L)(pWorld, pItem);
}

bool BaseAction::PickItem(CWorld *pWorld, CItem *pItem, CCreature *pCreature)
{
	typedef bool (*f)(CWorld*, CItem*, CCreature*);
	if(f(0x974400L)(pWorld, pItem, pCreature))
	{
		g_ItemDecay.OnPickUp(pItem);
		return true;
	}
	return false;
}

void BaseAction::DoAsyncPickItem(CWorld *pWorld, CItem *pItem)
{
	g_ItemDecay.OnPickUp(pItem);

	typedef void (*f)(CWorld*, CItem*);
	f(0x971D10L)(pWorld, pItem);
}

bool BaseAction::PutItem(CWorld *pWorld, CItem *pItem, FVector *pPos, UINT serverId, CCreature *pCreature)
{
	typedef bool (*f)(CWorld*, CItem*, FVector*, UINT, CCreature*);
	if(f(0x977A68L)(pWorld, pItem, pPos, serverId, pCreature))
	{
		if(pCreature->IsUser() || pCreature->IsPet())
		{
			g_ItemDecay.HandleDroppedItem(pItem);
		}
		return true;
	}
	return false;
}

bool BaseAction::MoveTo(CWorld* pWorld, CCreature* pCreature, FVector& pos, bool force)
{
	guard;

	if(User *pUser = pCreature->CastUser())
	{
		CEventMatchMgr* pMgr = CEventMatchMgr::GetInstance();
		if(pMgr)
		{
			pMgr->Lock(__FILEW__, __LINE__);
			for(map<UINT, CEventMatch*>::iterator Iter = pMgr->mData.begin();Iter!=pMgr->mData.end();Iter++)
			{
				bool playerInside = IsInside(Iter->second->pSD->pos, Iter->second->fanceWidth, Iter->second->fanceHeight, pUser->pSD->pos);
				bool pointInside = IsInside(Iter->second->pSD->pos, Iter->second->fanceWidth, Iter->second->fanceHeight, pos);
				if(playerInside && !pointInside)
				{
					pMgr->Unlock(__FILEW__, __LINE__);
					unguard;
					return false;
				}else if(!playerInside && pointInside)
				{
					pMgr->Unlock(__FILEW__, __LINE__);
					unguard;
					return false;
				}
			}
			pMgr->Unlock(__FILEW__, __LINE__);
		}
	}

	typedef bool (*f)(CWorld*, CCreature*, FVector&, bool);
	bool ret = f(0x9725A8L)(pWorld, pCreature, pos, force);
	unguard;
	return ret;
}

void BaseAction::ActivateSkill(CSkillInfo *pSI, CCreature *pCreature, CObject* pTarget, double smth, CSkillAction2* pAction2, EffectActivateTiming timing, double timeMultipler)
{
	if(User *pUser = pCreature->CastUser())
	{
		pUser->pEUD->currentActivatedSkillId = pSI->skillId;
		pUser->pEUD->currentActivatedSkillLevel = pSI->level;
	}
	
	pSI->ActivateSkill(pCreature, pTarget, smth, pAction2, timing, timeMultipler);

	if(User *pUser = pCreature->CastUser())
	{
		pUser->pEUD->currentActivatedSkillId = 0;
		pUser->pEUD->currentActivatedSkillLevel = 0;
	}
}

bool BaseAction::CheckCharacterName(const WCHAR* wName, int code, bool pet)
{
	if(code == 1 && g_NamePatterns.size() > 0)
	{
		size_t len = wcslen(wName);
		if((( pet && len < 25 ) || len < 17 ) && len > 0 )
		{
			//normal alphabet 0x41 - A, 0x5A - Z, 0x61 - a, 0x7A - z
			//cyrillic  0x410 - А, 0x44F - я
			//Digs: 0x30 - 0, 0x39 - 9
			//chineese: 0x3040 - 0x3096, 0x30A0 - 0x30FC, 0x4E00 - 0x51A5 
			for(UINT n=0;n<len;n++)
			{
				bool valid = false;
				for(UINT m=0;m<g_NamePatterns.size();m++)
				{
					if(g_NamePatterns[m].first <= wName[n] && g_NamePatterns[m].second >= wName[n])
					{
						valid = true;
						break;
					}
				}
				if(valid)
				{
					if(*lpFontCheck)
					{
						typedef bool (*fontCheck)(INT64, UINT);
						if(fontCheck(0x62B674)(0x22AE070, wName[n]))
						{
							continue;
						}else
						{
							g_Log.Add(CLog::Error, "Font check for name failed");
							return false;
						}
					}
					continue;
				}
				g_Log.Add(CLog::Error, L"[%S] Trying to use invalid character in name!", __FUNCTION__);
				return false;
			}
		}else
		{
			g_Log.Add(CLog::Error, L"[%S] Trying to use invalid length name!", __FUNCTION__);
			return false;
		}
		return true;
	}else
	{
		typedef bool (*f)(const WCHAR*, int, bool);
		return f(0x94E078L)(wName, code, pet);
	}
}

void BaseAction::OnSetCloseTimer(CUserSocket *pSocket)
{
	if(pSocket->ValidUserSocket())
	{
		if(User *pUser = pSocket->pUser->CastUser())
		{
			if(pUser->pEUD->offlineShopTime)
			{
				if(g_OfflineShop.CloseShop(pUser))
				{
					return;
				}
			}
		}
	}

	typedef void(*f)(CUserSocket*);
	f(0x456738L)(pSocket);
}


 PCWSTR lpCTradeCanceled = (PCWSTR)0xC36040; 
 PCWSTR lpDBPacketHandlerReplyTrade = (PCWSTR)0xB143A0;
 PCWSTR lpCTradeAddItems = (PCWSTR)0xC36140;
 PCWSTR lpUserDecreaseEquippedItemDurationOnTimerExp = (PCWSTR)0xC5A500;
 PCWSTR lpManipulateItemPacket = (PCWSTR)0xB16E20;

CCreatureSP* BaseAction::OnSetInventoryChanged(LPVOID lpInstance, CCreatureSP *lpCreatureSP, UINT serverId)
{
	typedef CCreatureSP* (*f)(LPVOID, CCreatureSP*, UINT);

	CCreatureSP* ret = f(0x448F14L)(lpInstance, lpCreatureSP, serverId);

	if(User *pUser = lpCreatureSP->get()->CastUser())
	{
		if(!CCallTrack::IsFunctionCalled(lpCTradeCanceled))
		{
			if(pUser->IsNowTrade())
			{
				if(	!CCallTrack::IsFunctionCalled(lpDBPacketHandlerReplyTrade) && 
					!CCallTrack::IsFunctionCalled(lpCTradeAddItems) &&
					!CCallTrack::IsFunctionCalled(lpUserDecreaseEquippedItemDurationOnTimerExp) &&
					!CCallTrack::IsFunctionCalled(lpManipulateItemPacket))
				{
					pUser->TradeCancel();
				}
			}
		}
	}

	return ret;
}