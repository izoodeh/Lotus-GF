#include "StdAfx.h"
#include "UserPacketHandler.h"
#include "HtmlCmd.h"
#include "Trade.h"
#include "HookManager.h"
#include "Firewall.h"
#include "ChatManager.h"
#include "ObsceneFilter.h"
#include "SpawnProtection.h"
#include "OfflineShop.h"
#include "VoteSystem.h"
#include "DonateSystem.h"
#include "ItemEnchant.h"
#include "UserCommandEx.h"
#include "FactionSystem.h"
#include "ItemAutoConsume.h"
#include "Championship.h"
#include "TvT.h"
#include "CliExt.h"
#include "UserDb.h"
#include "HookManager.h"
#include "TeleportData.h"
#include "AllInOne.h"

extern INT32 g_ProtocolVersion;
extern BOOL g_EnableStatisticsPacket;
extern INT64 g_MaxMultiSellQuanity;

LPBYTE lpAcceptLowerProtocol = (LPBYTE)0xF0D11C;
UINT requiredUpdateMsgId = 0;

BOOL g_SuppressClassLog[16];

void CLog_AddInvalidClassLog(LPVOID lpInstance, UINT type, const WCHAR* format, const WCHAR* file, int line, int sid, int cls)
{
	UINT threadIdx = GetThreadIndex();
	if(threadIdx < 16 && g_SuppressClassLog[threadIdx] == FALSE)
	{
		typedef void(*f)(LPVOID, UINT, const WCHAR*, const WCHAR*, int, int, int);
		f(0x6B9EBCL)(lpInstance, type, format, file, line, sid, cls);
	}
}

void CallStackInvalidClassLog(UINT type)
{
	UINT threadIdx = GetThreadIndex();
	if(threadIdx < 16 && g_SuppressClassLog[threadIdx] == FALSE)
	{
		typedef void(*f)(UINT);
		f(0x61B824L)(type);
	}
}

extern vector<UINT> g_BlockedRace;

CUserPacketHandler g_UserPacketHandler;

CUserPacketHandler::CUserPacketHandler()
{
	lpPacketHandler = (LPVOID)0x123A1750;
	lpPacketHandlerEx = (LPVOID)0x121C0D50;
	SetName(L"User Packet Handler");
}

CUserPacketHandler::~CUserPacketHandler()
{
}

bool CUserPacketHandler::Bind(int id, UserCallback func)
{
	typedef bool (*f)(LPVOID, int, UserCallback);
	return f(0x9249F0L)(lpPacketHandler, id, func);
}

bool CUserPacketHandler::BindEx(int id, UserCallback func)
{
	typedef bool (*f)(LPVOID, int, UserCallback);
	return f(0x9125D0L)(lpPacketHandlerEx, id, func);
}

bool PacketHandlerWrapper(CUserSocket *pSocket, const unsigned char *packet, BYTE opCode)
{
	return CUserPacketHandler::Handle(pSocket, packet, opCode);
}

bool CUserPacketHandler::Handle(CUserSocket *pSocket, const unsigned char* packet, BYTE opCode)
{
	guard;

	bool ret = false;
//	try
	{
		bool drop = false;
		bool kick = false;

		g_Firewall.OnPacketRecv(pSocket, opCode, drop, kick);

		if(drop)
		{
			unguard;
			return false;
		}
		if(kick)
		{
			unguard;
			return true;
		}

		bool validCS = true;
		PacketCS& packetCS = pSocket->packetCS;
		if(g_CliExt.IsEnabled() && opCode != 0xE)
		{
			if(packetCS.current != packetCS.calculated)
			{
				validCS = false;
				if(packetCS.current == packetCS.previous)
				{
					ret = false;	//drop
					g_Log.Add(CLog::Error, "[%s] Duplicated packet[%x] checksum[%u] - Account[%S]", __FUNCTION__, packetCS.current, pSocket->accountNameW);
				}else
				{
					ret = true;	//kick
					g_Log.Add(CLog::Error, "[%s] Calculated checksum[%u] doesn't match packet[%x] checksum[%u] - Account[%S]", __FUNCTION__, packetCS.calculated, opCode, packetCS.current, pSocket->accountNameW);
				}
			}
		}
		if(validCS)
		{
			Callback* lpPacketHandler = pSocket->lpPacketHandler;
			Callback handler = lpPacketHandler[opCode];
			ret = handler(pSocket, packet);
			packetCS.previous = packetCS.current;
		}

//		g_Log.Add(CLog::Blue, "[%s] Handle [%x] opcode ret[%d]", __FUNCTION__, opCode, ret);
	}/*catch(...)
	{
		g_Log.Add(CLog::Error, "[%s] Exception with opCode[%x] - dumping packet", __FUNCTION__, opCode);
		DumpPacketToFile(pSocket, packet, opCode);
		ret = true;
	}*/
	unguard;
	return ret;
}


void CUserPacketHandler::Init()
{
	g_Log.Add(CLog::Blue, "[%S] Initializing", GetName());

	requiredUpdateMsgId = GetPrivateProfileInt(_T("Setting"), _T("RequiredUpdateMsgId"), 0, g_ConfigFile);

	if(g_ProtocolVersion > 86)
	{
		for(int n=0;n<16;n++)
		{
			g_SuppressClassLog[n] = 0;
		}
		//ActionPacket invalid class id flood
		WriteCall(0x75A9BC, CLog_AddInvalidClassLog);
		WriteCall(0x75A9C6, CallStackInvalidClassLog);

		//0xFE, 0xA6 -> 0xA7 fix
		WriteBYTE(0x7ED215, 0xA7);
		WriteBYTE(0x7ED21C, 0x57);
		WriteBYTE(0x7ED0DA, 0xA7);
		WriteBYTE(0x7ED0E1, 0x57);
		WriteBYTE(0x7ED3DD, 0xA7);
		WriteBYTE(0x7ED3E4, 0x57);
		WriteBYTE(0x7ED539, 0xA7);
		WriteBYTE(0x7ED540, 0x57);
		WriteBYTE(0x7EDA07, 0xA7);
		WriteBYTE(0x7EDA10, 0x57);
		WriteBYTE(0x7EDF31, 0xA7);
		WriteBYTE(0x7EDF38, 0x57);
		//A7 -> A8 ProductList fix
		WriteBYTE(0x47613E, 0xA8);
		WriteBYTE(0x476145, 0x56);
		WriteBYTE(0x476385, 0xA8);
		WriteBYTE(0x47638C, 0x56);
		//A8 -> A9 ProductInfo fix
		WriteBYTE(0x915BE3, 0xA9);
		WriteBYTE(0x915BEA, 0x55);
		//A9 -> AA BuyProduct
		WriteBYTE(0x5AB7D4, 0xAA);
		WriteBYTE(0x5AB7DB, 0x54);
		WriteBYTE(0x5ABD04, 0xAA);
		WriteBYTE(0x5ABD0B, 0x54);
		WriteBYTE(0x7EE2ED, 0xAA);
		WriteBYTE(0x7EE2F4, 0x54);
		WriteBYTE(0x7EE451, 0xAA);
		WriteBYTE(0x7EE458, 0x54);
		WriteBYTE(0x7EE6EC, 0xAA);
		WriteBYTE(0x7EE6F3, 0x54);
		WriteBYTE(0x7EE8CF, 0xAA);
		WriteBYTE(0x7EE8D6, 0x54);
		WriteBYTE(0x9124B7, 0xAA);
		WriteBYTE(0x9124BE, 0x54);
		WriteBYTE(0x912547, 0xAA);
		WriteBYTE(0x91254E, 0x54);
		WriteBYTE(0x91257C, 0xAA);
		WriteBYTE(0x912583, 0x54);
		//AA -> AB PremiumUserStatus
		WriteBYTE(0x485366, 0xAB);
		WriteBYTE(0x48536D, 0x53);
		WriteBYTE(0x971669, 0xAB);
		WriteBYTE(0x971670, 0x53);
		//AB -> AC ExBrBroadcastEvent
		WriteBYTE(0x8CFBC5, 0xAC);
		WriteBYTE(0x8CFBCC, 0x52);
		//AC -> AD ExBrExtraUserIn <> User__BR_SendExtraCharInfo
		WriteBYTE(0x8ADEF8, 0xAD);
		WriteBYTE(0x8ADEBA, 0xAD);
		WriteBYTE(0x8ADEC1, 0x51);
	}

	Bind(0, CUserPacketHandler::LogoutPacket);
	Bind(0x01, CUserPacketHandler::AttackPacket);
	Bind(0x02, CUserPacketHandler::MoveToLocationBackPacket);
	Bind(0x03, CUserPacketHandler::StartPledgeWarPacket);
//	Bind(0x04, CUserPacketHandler::DummyPacket);
	Bind(0x05, CUserPacketHandler::StopPledgeWarPacket);
//	Bind(0x06, CUserPacketHandler::DummyPacket);
//	Bind(0x07, CUserPacketHandler::DummyPacket);
//	Bind(0x08, CUserPacketHandler::DummyPacket);
	Bind(0x09, CUserPacketHandler::SetPledgeCrestPacket);
//	Bind(0x0A, CUserPacketHandler::DummyPacket);
	Bind(0x0A, CUserPacketHandler::CustomPacket);
	Bind(0x0B, CUserPacketHandler::GiveNicknamePacket);
	Bind(0x0C, CUserPacketHandler::CharacterCreatePacket);
	Bind(0x0D, CUserPacketHandler::CharacterDeletePacket);
	Bind(0x0E, CUserPacketHandler::VersionPacket);
	Bind(0x0F, CUserPacketHandler::MoveToLocationPacket);
	Bind(0x11, CUserPacketHandler::EnterWorldPacket);
	Bind(0x12, CUserPacketHandler::CharacterSelectPacket);
	Bind(0x13, CUserPacketHandler::NewCharacterPacket);
	Bind(0x14, CUserPacketHandler::ItemListPacket);
//	Bind(0x15, CUserPacketHandler::DummyPacket);
	Bind(0x16, CUserPacketHandler::UnequipItemPacket);
	Bind(0x17, CUserPacketHandler::DropItemPacket);
	Bind(0x18, CUserPacketHandler::GetItemPacket);
	Bind(0x19, CUserPacketHandler::UseItemPacket);
	Bind(0x1A, CUserPacketHandler::TradeRequestPacket);
	Bind(0x1B, CUserPacketHandler::TradeAddItemsPacket);
	Bind(0x1C, CUserPacketHandler::TradeDonePacket);
	Bind(0x1F, CUserPacketHandler::ActionPacket);
	Bind(0x22, CUserPacketHandler::LinkHtmlPacket);
	Bind(0x23, CUserPacketHandler::HtmlCMDPacket);
	Bind(0x24, CUserPacketHandler::WriteHtmlPacket);
	Bind(0x26, CUserPacketHandler::JoinPledgePacket);
	Bind(0x27, CUserPacketHandler::AnswerJoinPledgePacket);
	Bind(0x28, CUserPacketHandler::WithdrawPledgePacket);
	Bind(0x29, CUserPacketHandler::OustPledgeMemberPacket);
	Bind(0x2B, CUserPacketHandler::LoginPacket);
	Bind(0x2C, CUserPacketHandler::GetItemFromPetPacket);
	Bind(0x31, CUserPacketHandler::PrivateStoreListSet);
	Bind(0x34, CUserPacketHandler::SocialActionPacket);
	Bind(0x35, CUserPacketHandler::ChangeMoveTypePacket);
	Bind(0x36, CUserPacketHandler::ChangeStopTypePacket);
	Bind(0x37, CUserPacketHandler::SellPacket);
	Bind(0x38, CUserPacketHandler::MagicSkillListPacket);
	Bind(0x39, CUserPacketHandler::MagicSkillUsePacket);
	Bind(0x3A, CUserPacketHandler::AppearingPacket);
	Bind(0x3B, CUserPacketHandler::WarehouseDepositPacket);
	Bind(0x3C, CUserPacketHandler::WarehouseWithdrawPacket);
	Bind(0x3D, CUserPacketHandler::ShortCutRegisterPacket);
//	Bind(0x3E, CUserPacketHandler::DummyPacket);
	Bind(0x3F, CUserPacketHandler::ShortCutDeletePacket);
	Bind(0x40, CUserPacketHandler::BuyPacket);
//	Bind(0x41, CUserPacketHandler::DummyPacket);
	Bind(0x42, CUserPacketHandler::JoinPartyPacket);
	Bind(0x43, CUserPacketHandler::AnswerJoinPartyPacket);
	Bind(0x44, CUserPacketHandler::WithdrawPartyPacket);
	Bind(0x45, CUserPacketHandler::OustPartyMemberPacket);
	Bind(0x46, CUserPacketHandler::DismissPartyPacket);
	Bind(0x47, CUserPacketHandler::CannotMoveAnymorePacket);
	Bind(0x48, CUserPacketHandler::TargetUnselectedPacket);
	Bind(0x49, CUserPacketHandler::SayPacket2);
//	Bind(0x4B, CUserPacketHandler::DummyPacket);
//	Bind(0x4C, CUserPacketHandler::DummyPacket);
	Bind(0x4D, CUserPacketHandler::ShowPledgeMemberListOpenPacket);
//	Bind(0x4E, CUserPacketHandler::DummyPacket);
//	Bind(0x4F, CUserPacketHandler::DummyPacket);
	Bind(0x50, CUserPacketHandler::SkillListPacket);
//	Bind(0x51, CUserPacketHandler::DummyPacket);
	Bind(0x52, CUserPacketHandler::MoveWithDeltaPacket);
	Bind(0x53, CUserPacketHandler::GetOnVehiclePacket);
	Bind(0x54, CUserPacketHandler::GetOffVehiclePacket);
	Bind(0x55, CUserPacketHandler::TradeStartPacket);
	Bind(0x56, CUserPacketHandler::IconAction);
	Bind(0x57, CUserPacketHandler::RestartPacket);
//	Bind(0x58, CUserPacketHandler::DummyPacket);
	Bind(0x59, CUserPacketHandler::ValidateLocationPacket);
	Bind(0x5A, CUserPacketHandler::SekCostume);
	Bind(0x5B, CUserPacketHandler::StartRotatingPacket);
	Bind(0x5C, CUserPacketHandler::FinishRotatingPacket);
//	Bind(0x5D, CUserPacketHandler::DummyPacket);
	Bind(0x5E, CUserPacketHandler::ShowBBSBoard);
	Bind(0x5F, CUserPacketHandler::RequestEnchantItemPacket);
	Bind(0x60, CUserPacketHandler::DestroyItem);
	Bind(0x61, CUserPacketHandler::TargetUserFromMenu);
	Bind(0x63, CUserPacketHandler::DestroyQuest);
	Bind(0x74, CUserPacketHandler::SysCMD2Packet);
	Bind(0x75, CUserPacketHandler::MoveToLocationInVehiclePacket);
	Bind(0x76, CUserPacketHandler::CannotMoveAnymoreInVehiclePacket);
	Bind(0x7C, CUserPacketHandler::RequestAcquireSkill);
	Bind(0x7D, CUserPacketHandler::RestartPointPacket);
	Bind(0x83, CUserPacketHandler::PrivateStoreBuyListSend);
	Bind(0x95, CUserPacketHandler::GiveItemToPetPacket);
	Bind(0x9A, CUserPacketHandler::PrivateStoreBuyListSet);
	Bind(0x9F, CUserPacketHandler::PrivateStoreBuyBuyListSend);
	Bind(0xA7, CUserPacketHandler::RequestPackageSendableItemList);
	Bind(0xA8, CUserPacketHandler::RequestPackageSend);
	Bind(0xB0, CUserPacketHandler::RequestMultiSellChoose);
	Bind(0xB3, CUserPacketHandler::RequestUserCommand);
	Bind(0xC6, CUserPacketHandler::ConfirmDialog);

	BindEx(0xD, CUserPacketHandler::AutoSoulshotExPacket);
	BindEx(0xF, CUserPacketHandler::RequestEnchantSkill);
	BindEx(0x34, CUserPacketHandler::RequestEnchantSkillRouteChange);
	BindEx(0x4C, CUserPacketHandler::TryToPutEnchantTargetItem);

	WriteCall(0x92EDFB, PacketHandlerHook, 2);
	WriteCall(0x92EF08, PacketHandlerHook, 2);
	WriteBYTE(0x9179C9, 0);	//return false instead of true in 0x3D user ex packet when user is null
}

bool CUserPacketHandler::CannotMoveAnymoreInVehiclePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91EF3CL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::MoveToLocationInVehiclePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91EDC4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::SysCMD2Packet(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91ECDCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::TargetUserFromMenu(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x9356B4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::RequestEnchantItemPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91EAFCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ShowBBSBoard(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E9B4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::GiveNicknamePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x93519CL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::SetPledgeCrestPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x92723CL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::StopPledgeWarPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E8CCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::StartPledgeWarPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E7E4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::MoveToLocationBackPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x926E7CL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::FinishRotatingPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E6FCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::StartRotatingPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E604L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::SekCostume(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x926DB4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ValidateLocationPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E4ACL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::GetOffVehiclePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x92694CL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::GetOnVehiclePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E1ECL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::MoveWithDeltaPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x926804L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::SkillListPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91DEF4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ShowPledgeMemberListOpenPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91E0C4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::TargetUnselectedPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91D5D8L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::CannotMoveAnymorePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91D3D0L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ShortCutDeletePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91D2E8L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ShortCutRegisterPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91D1B0L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::MagicSkillListPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x92AA10L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::DismissPartyPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91CFA0L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::OustPartyMemberPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91CE28L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::WithdrawPartyPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91CCD0L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::OustPledgeMemberPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x933F28L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::WithdrawPledgePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x9264FCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::AnswerJoinPledgePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x93994CL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::JoinPledgePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x938DF4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::WriteHtmlPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x929C44L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ChangeStopTypePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x929ABCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ChangeMoveTypePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x9263A4L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::SocialActionPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91CC70L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::TradeDonePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91CA60L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::UnequipItemPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x925F50L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::ItemListPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x91C3C8L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::NewCharacterPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x925954L)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::CharacterDeletePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x9257FCL)(pSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::LoginPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = UserCallback(0x940098L)(pSocket, packet);
	unguard;
	return ret;

}

bool CUserPacketHandler::CharacterCreatePacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;

	WCHAR wName[25];
	UINT race, sex, classId, intStat, strStat, conStat, menStat, dexStat, witStat, hairStyle, hairColor, face;
	Disassemble(packet, "Sdddddddddddd", sizeof(wName), wName, &race, &sex, &classId, &intStat, &strStat, &conStat, &menStat, &dexStat, &witStat, &hairStyle, &hairColor, &face);

	for(UINT n=0;n<g_BlockedRace.size();n++)
	{
		if(race == g_BlockedRace[n])
		{
			pSocket->lastUsedPacketId = 0x13;
			unguard;
			return false;
		}
	}

	unguard;
	return UserCallback(0x924EF0L)(pSocket, packet);
}

bool CUserPacketHandler::AnswerJoinPartyPacket(CUserSocket *pSocket, const unsigned char *packet)
{
	guard;
	if(User *pUser = pSocket->pUser->CastUser())
	{
		if(pUser->pEUD->dungeonUser.id && !pUser->pEUD->dungeonUser.canJoinInviteParty)
		{
			unguard;
			return false;
		}
		
		bool ret = UserCallback(0x9401E4L)(pSocket, packet);
		unguard;
		return ret;
	}
	unguard;
	return true;
}

bool CUserPacketHandler::JoinPartyPacket(CUserSocket *pSocket, const unsigned char *packet)
{
	guard;
	if(User *pUser = pSocket->pUser->CastUser())
	{
		if(pUser->pEUD->dungeonUser.id && !pUser->pEUD->dungeonUser.canJoinInviteParty)
		{
			unguard;
			return false;
		}

		bool ret = UserCallback(0x939DE4L)(pSocket, packet);
		unguard;
		return ret;
	}
	unguard;
	return true;
}

bool CUserPacketHandler::RequestAcquireSkill(CUserSocket *pSocket, const unsigned char *packet)
{
	guard;
	if(User *pUser = pSocket->pUser->CastUser())
	{
		pSocket->dropGetTimestamp = GetTickCount() + g_DropGetReuse;
		unguard;
		return UserCallback(0x91F53CL)(pSocket, packet);
	}
	unguard;
	return true;
}


bool CUserPacketHandler::RequestEnchantSkill(CUserSocket *pSocket, const unsigned char *packet)
{
	guard;

	if(User *pUser = pSocket->pUser->CastUser())
	{
		pSocket->dropGetTimestamp = GetTickCount() + g_DropGetReuse;
		unguard;
		return UserCallback(0x91A87CL)(pSocket, packet);
	}
	unguard;
	return true;
}

bool CUserPacketHandler::CustomPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;
	bool ret = true;
	if(g_CliExt.IsEnabled())
	{
		ret = g_CliExt.HandleCustomPacket(pSocket, packet);
	}
	unguard;
	return ret;
}

bool CUserPacketHandler::UseItemPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	guard;

	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT serverId = 0;
		Disassemble(packet, "d", &serverId);

		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}

		CItem *pItem = pUser->inventory.GetByServerID(serverId);
		if(pItem->ValidItem())
		{
			if(pUser->pEUD->championshipUser.state > Championship::UserTeleporting)
			{
				if(!g_Championship.CanUse(pItem))
				{
					unguard;
					return false;
				}
			}
			if(pUser->pEUD->tvtUser.status == TvT::UserPreparing || pUser->pEUD->tvtUser.status == TvT::UserFighting)
			{
				if(!g_TvT.CanUse(pItem))
				{
					unguard;
					return false;
				}
			}
		}

		unguard;
		return UserCallback(0x91C490L)(pSocket, packet);
	}
	unguard;
	return true;
}

bool CUserPacketHandler::GetItemPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;

			UINT serverId = 0;
			INT64 count = 0;

			Disassemble(packet, "dQ", &serverId, &count);
			
			if(count < 1)
			{
				pUser->ActionFailed();
				return false;
			}
			
			return UserCallback(0x91C6C0L)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::ConfirmDialog(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT messageId = 0, answer = 0, requestorId = 0;
		Disassemble(packet, "ddd", &messageId, &answer, &requestorId);

		if( g_FactionSystem.HandleReply(pUser, messageId, answer) )
		{
			return false;
		}
		if( g_TvT.HandleReply(pUser, messageId, answer) )
		{
			return false;
		}
		if( g_TeleportData.HandleReply(pUser, messageId, answer) )
		{
			return false;
		}

		return UserCallback(0x9238C8L)(pUserSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::AutoSoulshotExPacket(CUserSocket *pSocket, const unsigned char *packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		INT32 itemId = 0;
		BOOL enable = 0;
		Disassemble(packet, "dd", &itemId, &enable);

		if(g_ItemAutoConsume.IsAutoConsume(itemId))
		{
			if(enable)
			{
				g_ItemAutoConsume.TurnOn(pUser, itemId);
			}else
			{
				g_ItemAutoConsume.TurnOff(pUser, itemId);
			}
			return false;
		}

		return UserCallback(0x90EF80L)(pSocket, packet);
	}
	return false;
}

bool CUserPacketHandler::TryToPutEnchantTargetItem(CUserSocket *pSocket, const unsigned char *packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT itemId = 0;
		Disassemble(packet, "d", &itemId);

		if(itemId > 0)
		{
			CItem *pItem = pUser->inventory.GetByServerID(itemId);
			if(pItem)
			{
				CItem *pScroll = pUser->GetPendingUseETCItem(PUI_SCROLL);
				if(pScroll)
				{
					if(!g_ItemEnchant.CanEnchant(pUser, pItem, pScroll))
					{
						pUser->SetPendingUseETCItem(0, PUI_ENCHANT_BONUS);
						pUser->enchantRateBonus = 0.0;
						pUser->pUserSocket->Send("chd", 0xFE, 0x81, 0);
						return false;
					}
				}
			}
		}

		return UserCallback(0x917E10L)(pSocket, packet);
	}

	return true;
}


bool CUserPacketHandler::RequestEnchantSkillRouteChange(CUserSocket *pSocket, const unsigned char *packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		INT32 skillId = 0, routeLevel = 0;
		Disassemble(packet, "dd", &skillId, &routeLevel);

		INT32 currentLevel = pUser->GetAcquiredSkillLevel(skillId);
		if(currentLevel <= 100)
		{
			g_Log.Add(CLog::Error, "[%s] User[%S] tried to acquire invalid skill[%d][%d]!", __FUNCTION__, pUser->pSD->wName, skillId, routeLevel);
			return false;
		}
		if((currentLevel%100) != (routeLevel%100))
		{
			g_Log.Add(CLog::Error, "[%s] User[%S] tried to acquire invalid skill level![%d][%d]!", __FUNCTION__, pUser->pSD->wName, skillId, routeLevel);
			return false;
		}

		return UserCallback(0x91AB94L)(pSocket, packet);
	}

	return true;
}

bool CUserPacketHandler::CharacterSelectPacket(CUserSocket *pUserSocket, const unsigned char* packet)
{
	guard;

	if((*lpAcceptLowerProtocol) && requiredUpdateMsgId > 0)
	{
		if(pUserSocket->protocolVersion < g_ProtocolVersion)
		{
			pUserSocket->SendSystemMessage(requiredUpdateMsgId);
			unguard;
			return false;
		}
	}

	if(g_CliExt.IsEnabled())
	{
		if(pUserSocket->shortHWID > 0)
		{
			//check if it's banned
			if(g_CliExt.IsBanned(pUserSocket->hardwareId.hash))
			{
				pUserSocket->Send("cdSd", 0x19, 0, g_CliExt.GetBannedHtml(), 0);
				unguard;
				return false;
			}
			//check box limit
			if(g_CliExt.GetBoxLimit() > 0 && g_UserDb.GetUserAmountByHWID(pUserSocket->hardwareId.hash) >= g_CliExt.GetBoxLimit())
			{
				pUserSocket->Send("cdSd", 0x19, 0, g_CliExt.GetBoxLimitHtml(), 0);
				unguard;
				return false;
			}

		}else
		{
			unguard;
			return false;	//drop the packet
		}
	}

	bool ret = UserCallback(0x93F2F8L)(pUserSocket, packet);
	unguard;
	return ret;
}

bool CUserPacketHandler::DestroyQuest(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT questId = 0;
		Disassemble(packet, "d", &questId);

		if(questId == 750)
		{
			pUserSocket->SendSystemMessage(5072);
			return false;
		}

		return UserCallback(0x92D5B8L)(pUserSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::RequestUserCommand(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT cmdId = 0;
		Disassemble(packet, "d", &cmdId);

		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}

		if(cmdId == 127)
		{
			g_OfflineShop.RequestActivate(pUser);
			return false;
		}
		return UserCallback(0x921884L)(pUserSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::RequestMultiSellChoose(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->IsNowTrade())
			{
				pUser->TradeCancel();
			}

			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			UINT listId = 0, entryId = 0;
			INT64 count = 0;
			Disassemble(packet, "ddQ", &listId, &entryId, &count);
			if(count > g_MaxMultiSellQuanity)
			{
				pUserSocket->SendSystemMessage(1338);
				return false;
			}

			return UserCallback(0x928550L)(pUserSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::DropItemPacket(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			UINT serverId = 0;
			INT64 count = 0;
			INT32 x = 0, y = 0, z = 0;
			Disassemble(packet, "dQddd", &serverId, &count, &x, &y, &z);
			
			if(count < 1)
				return false;

			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}


			if(pUser->pEUD->spawnProtection)
			{
				g_SpawnProtection.Deactivate(pUser);
			}
			if(pUser->olympiadUser.IsInside())
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot drop any item during olympiad match.");
				return false;
			}
			if(pUser->pEUD->championshipUser.state > Championship::UserTeleporting)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot drop any item during championship match.");
				return false;
			}
			return UserCallback(0x91C588L)(pUserSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::DestroyItem(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;

			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}
		
			if(pUser->pEUD->spawnProtection)
			{
				g_SpawnProtection.Deactivate(pUser);
			}

			UINT serverId = 0;
			INT64 count = 0;

			Disassemble(packet, "dQ", &serverId, &count);

			if(count < 1)
				return false;

			return UserCallback(0x91EBE4L)(pUserSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::RestartPacket(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->pEUD->offlineShop)
		{
			pUserSocket->Send("c", 0x84);
			return false;
		}
	}
	return UserCallback(0x93A9BCL)(pUserSocket, packet);
}

bool CUserPacketHandler::LogoutPacket(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->pEUD->offlineShop)
		{
			pUserSocket->Send("c", 0x84);
			return false;
		}
	}
	return UserCallback(0x925DE8L)(pUserSocket, packet);
}

bool CUserPacketHandler::AttackPacket(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT serverId = 0;
		INT32 x, y, z;
		BYTE shift = 0;
		Disassemble(packet, "ddddc", &serverId, &x, &y, &z, &shift);

		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}

		CCreature *pCreature = CCreature::GetCreatureByObjectId(&serverId);
		if(pCreature->ValidCreature())
		{
			if(pCreature->hideMode)
			{
				pUser->ActionFailed();
				return false;
			}
		}
		return UserCallback(0x91C220L)(pUserSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::MagicSkillUsePacket(CUserSocket *pUserSocket, const unsigned char* packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		INT32 skillId = 0;
		Disassemble(packet, "d", &skillId);

		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}
		if(pUser->pEUD->championshipUser.state == Championship::UserFighting)
		{
			if(g_Championship.IsBlockedSkill(skillId))
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot use this skill during championship match.");
				return false;
			}
		}

		if(pUser->pEUD->tvtUser.status == TvT::UserPreparing || pUser->pEUD->tvtUser.status == TvT::UserFighting)
		{
			if(g_TvT.IsSkillBlocked(skillId))
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot use this skill during TvT match.");
				return false;
			}
		}

		if(pUser->targetId != pUser->serverId)
		{
			CCreature *pCreature = CCreature::GetCreatureByObjectId(&pUser->targetId);
			if(pCreature->ValidCreature())
			{
				if(pCreature->hideMode)
				{
					pUser->ActionFailed();
					return false;
				}
			}
		}

		if(g_AllInOne.Enabled())
		{
			if(pUser->pEUD->aioUser.aioId > 0)
			{
				if(!g_AllInOne.CanUse(pUser, skillId))
				{
					pUserSocket->SendSystemMessage(L"You cannot use AllInOne skills outside Peace Zone!");
					return false;
				}
			}
		}

		return UserCallback(0x91DFBCL)(pUserSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::MoveToLocationPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}
		return UserCallback(0x924D48L)(pUserSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::VersionPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	INT32 revision = 0;
	Disassemble(packet, "d", &revision);

	//-2 - ping packet
	//-3 - statistics packet
	if(revision == g_ProtocolVersion || revision == -2 || (revision == -3 && g_EnableStatisticsPacket))
	{
		pUserSocket->protocolVersion = revision;
		return UserCallback(0x93344CL)(pUserSocket, packet);
	}

	if( revision > 0 && revision < g_ProtocolVersion && (*lpAcceptLowerProtocol))
	{
		pUserSocket->protocolVersion = revision;
		return UserCallback(0x93344CL)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::HtmlCMDPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(pUserSocket->pUser->ValidUser())
	{
		WCHAR wBypass[4096];
		Disassemble(packet, "S", 4096, wBypass);

		return g_HtmlCmd.Handle(pUserSocket, pUserSocket->pUser, wBypass, packet);
	}

	return true;
}


bool CUserPacketHandler::ActionPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->IsNowTrade())
		{
			pUser->TradeCancel();
		}

		UINT serverId = 0;
		INT32 x,y,z;
		bool withShift = false;
		Disassemble(packet, "ddddc", &serverId, &x, &y, &z, &withShift);
		
		/* Auto Target disables spawn protection
		if(serverId != pUser->serverId && pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}
		*/

		if(serverId)
		{
			UINT threadIdx = GetThreadIndex();
			if(threadIdx < 16)
			{
				g_SuppressClassLog[threadIdx] = TRUE;
			}
			CItem* pItem = reinterpret_cast<CItem*>(CObject::GetObject(serverId));
			if(threadIdx < 16)
			{
				g_SuppressClassLog[threadIdx] = FALSE;
			}
			if(!pItem)
			{
			//	g_Log.Add(CLog::Error, "[%s] User[%S] sent invalid ActionPacket with target[%d] x[%d](%d) y[%d](%d) z[%d](%d) shift[%d]", __FUNCTION__, pUser->pSD->wName, serverId, x, (int)pUser->pSD->pos.x, y, (int)pUser->pSD->pos.y, z, (int)pUser->pSD->pos.z, withShift);
				if(serverId > 0)
				{
					pUser->ActionFailed();
					return false;
				}
			}
			if(pItem->ValidItem())
			{
				UINT currentTick = GetTickCount();
				if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
				{
					pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
					return UserCallback(0x91CB48L)(pUserSocket, packet);
				}
				pUser->ActionFailed();
				return false;
			}else
			{
				if(pUser->serverId != serverId)
				{
					CCreature *pCreature = CCreature::GetCreatureByObjectId(&serverId);
					if(pCreature->ValidCreature())
					{
						if(pCreature->hideMode)
						{
							pUser->ActionFailed();
							return false;
						}
					}
				}
			}
		}
		return UserCallback(0x91CB48L)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::IconAction(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->IsNowTrade())
		{
			pUser->TradeCancel();
		}
		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}

		return UserCallback(0x91E3A4L)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::EnterWorldPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			pUserSocket->pUser->TradeCancel();
		}

		return UserCallback(0x925CE0L)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::WarehouseDepositPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->IsNowTrade())
		{
			pUser->TradeCancel();
			return false;
		}
		if(pUser->pEUD->isItemDataLock)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
			return false;
		}

		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			return UserCallback(0x91D6C0L)(pUserSocket, packet);
		}
		return false;
	}
	
	return true;
}

bool CUserPacketHandler::WarehouseWithdrawPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{

	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			pUserSocket->pUser->TradeCancel();
		}

		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			return UserCallback(0x91D9C8L)(pUserSocket, packet);
		}
		return false;
	}
	
	return true;
}

bool CUserPacketHandler::AppearingPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{

	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			pUserSocket->pUser->TradeCancel();
		}

		return UserCallback(0x91D0F8L)(pUserSocket, packet);
	}
	
	return true;
}


bool CUserPacketHandler::RestartPointPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->IsNowTrade())
		{
			pUser->TradeCancel();
		}
		
		if(pUser->pEUD->championshipUser.state == Championship::UserFighting)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot use this option during championship match.");
			return false;
		}
		if(pUser->pEUD->tvtUser.status == TvT::UserFighting)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot use this option during TvT match.");
			return false;
		}
		if(pUser->pEUD->tvtUser.respawnTick > 0)
		{
			pUser->pEUD->tvtUser.respawnTick = 0;
			pUser->pEUD->tvtUser.respawnPos.x = 0;
			pUser->pEUD->tvtUser.respawnPos.y = 0;
			pUser->pEUD->tvtUser.respawnPos.z = 0;
		}

		return UserCallback(0x91F644L)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::GetItemFromPetPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{

	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			pUserSocket->pUser->TradeCancel();
		}

		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			return UserCallback(0x92099CL)(pUserSocket, packet);
		}
		return false;
	}
	
	return true;
}

bool CUserPacketHandler::GiveItemToPetPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->IsNowTrade())
			{
				pUser->TradeCancel();
			}
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			return UserCallback(0x92B9B4L)(pUserSocket, packet);
		}
		return false;
	}
	
	return true;
}

bool CUserPacketHandler::LinkHtmlPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{

	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			pUserSocket->pUser->TradeCancel();
		}

		return UserCallback(0x94A098L)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::RequestPackageSendableItemList(CUserSocket *pUserSocket, const unsigned char *packet)
{

	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			pUserSocket->pUser->TradeCancel();
		}

		return UserCallback(0x9212CCL)(pUserSocket, packet);
	}
	
	return true;
}

bool CUserPacketHandler::RequestPackageSend(CUserSocket *pUserSocket, const unsigned char *packet)
{

	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;

			if(pUser->IsNowTrade())
			{
				pUser->TradeCancel();
			}

			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			return UserCallback(0x92BF44L)(pUserSocket, packet);
		}
		return false;
	}
	
	return true;
}

bool CUserPacketHandler::SellPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			return UserCallback(0x931090L)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::PrivateStoreBuyBuyListSend(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			return UserCallback(0x93675CL)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::PrivateStoreBuyListSend(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			return UserCallback(0x93612CL)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::PrivateStoreBuyListSet(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				pUser->QuitPrivateStore();
				return false;
			}

			return UserCallback(0x920B64L)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::PrivateStoreListSet(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				pUser->QuitPrivateStore();
				return false;
			}

			return UserCallback(0x91FAD4L)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::BuyPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		UINT currentTick = GetTickCount();
		if(pSocket->dropGetTimestamp < currentTick || ((pSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
		{
			pSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
			if(pUser->pEUD->isItemDataLock)
			{
				pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
				return false;
			}

			return UserCallback(0x92F924L)(pSocket, packet);
		}
		return false;
	}
	return true;
}

bool CUserPacketHandler::TradeRequestPacket(CUserSocket *pSocket, const unsigned char* packet)
{
	if(User *pUser = pSocket->pUser->CastUser())
	{
		if(pUser->pEUD->isItemDataLock)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
			return false;
		}

		return UserCallback(0x91C7C8)(pSocket, packet);
	}
	return true;
}

bool CUserPacketHandler::TradeAddItemsPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(pUserSocket->pUser->ValidUser())
	{
		if(pUserSocket->pUser->IsNowTrade())
		{
			UINT currentTick = GetTickCount();
			if(pUserSocket->dropGetTimestamp < currentTick || ((pUserSocket->dropGetTimestamp - g_DropGetReuse) > currentTick))
			{
				pUserSocket->dropGetTimestamp = currentTick + g_DropGetReuse;
				return UserCallback(0x91C998L)(pUserSocket, packet);	
			}
			return false;
		}else
		{
			g_Log.Add(CLog::Error, "User[%S] tried to trade add items without active trade!", pUserSocket->pUser->pSD->wName);
			return false;
		}
	}
	
	return true;
}

bool CUserPacketHandler::TradeStartPacket(CUserSocket *pUserSocket, const unsigned char *packet)
{
	if(User *pUser = pUserSocket->pUser->CastUser())
	{
		if(pUser->pEUD->isItemDataLock)
		{
			pUser->pUserSocket->SendSystemMessage(L"You cannot perform this action due to Item Lock.");
			return false;
		}

		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}
		if(CTrade *pTrade = pUser->GetTrade())
		{
			if(pTrade->userServerId1 == pUser->serverId)
			{
				pUser->TradeCancel();
				g_Log.Add(CLog::Error, "User[%S] tried to reply to own trade request!", pUser->pSD->wName);
				return false;
			}

			return UserCallback(0x91C8B0L)(pUserSocket, packet);
		}else
		{
		//	g_Log.Add(CLog::Error, "User[%S] tried to start trade without active trade!", pUser->pSD->wName);
			return false;
		}
	}
	
	return true;
}

bool CUserPacketHandler::SayPacket2(CUserSocket *pUserSocket, const unsigned char *packet)
{
	User *pUser = pUserSocket->pUser;
	if(pUser->ValidUser())
	{
		WCHAR wMessage[4096];
		memset(wMessage, 0, sizeof(wMessage));
		DWORD type = 0;
		Disassemble(packet, "Sd", 2048, wMessage, &type);

	//	g_Log.Add(CLog::Blue, "[%s] [%S]", __FUNCTION__, wMessage);

		try
		{
			//check for \n start
			WCHAR *wStr = (PWCHAR)packet;
			WCHAR *wEndLine = wcsstr(wStr, L"\\n");
			int k = 0;
			while( wEndLine  && k < 2048)
			{
				k++;
				wEndLine[0] = L' ';
				wEndLine[1] = L' ';
				wEndLine = wcsstr(wStr, L"\\n");
			}
		}catch(...)
		{
			g_Log.Add(CLog::Error, "[%s] Invalid string - User[%S]", __FUNCTION__, pUser->pSD->wName);
			return false;
		}

		if(pUser->pEUD->spawnProtection)
		{
			g_SpawnProtection.Deactivate(pUser);
		}

		size_t len = wcslen(wMessage);
		if(len < 2046)
		{
			WCHAR* wStr = (WCHAR*)packet;
			bool drop = false;
			bool kick = false;
			
			g_ObsceneFilter.CheckString(pUser, wStr, drop, kick);

			if(kick)
			{
				return true;
			}
			if(drop)
			{
				return false;
			}

			if(g_UserCommandEx.IsEnabled() && len > 1)
			{
				if(g_UserCommandEx.IsCommand(wMessage))
				{
					g_UserCommandEx.Handle(pUserSocket, pUser, wMessage);
					return false;
				}
			}

			return g_ChatManager.Handle(pUserSocket, packet, type);
		}


		return false;
	}
	
	return true;
}