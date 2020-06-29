#pragma once

extern "C"
{
	bool PacketHandlerHook(CUserSocket *pSocket, const unsigned char* packet, BYTE opCode);
	bool PacketHandlerWrapper(CUserSocket *pSocket, const unsigned char* packet, BYTE opCode);
};

void CLog_AddInvalidClassLog(LPVOID lpInstance, UINT type, const WCHAR* format, const WCHAR* file, int line, int sid, int cls);
void CallStackInvalidClassLog(UINT type);

class CUserPacketHandler : public CFeature
{
	LPVOID lpPacketHandler;
	LPVOID lpPacketHandlerEx;
	bool Bind(int id, UserCallback func);
	bool BindEx(int id, UserCallback func);
public:
	CUserPacketHandler();
	~CUserPacketHandler();
	void Init();
	static bool Handle(CUserSocket* pUserSocket, const unsigned char* packet, BYTE opCode);

	/* Ex Packet */
	static bool RequestEnchantSkillRouteChange(CUserSocket *pSocket, const unsigned char* packet);
	static bool TryToPutEnchantTargetItem(CUserSocket *pSocket, const unsigned char* packet);
	static bool AutoSoulshotExPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool RequestEnchantSkill(CUserSocket *pSocket, const unsigned char* packet);
	static bool RequestAcquireSkill(CUserSocket *pSocket, const unsigned char *packet);
	static bool CannotMoveAnymorePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool CannotMoveAnymoreInVehiclePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool CustomPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool ChangeMoveTypePacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool ChangeStopTypePacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool ActionPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool AttackPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool BuyPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool ConfirmDialog(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool DestroyQuest(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool DismissPartyPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool DropItemPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool DestroyItem(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool FinishRotatingPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool GetItemPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool GetOnVehiclePacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool GetOffVehiclePacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool GiveNicknamePacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool UseItemPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool CharacterCreatePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool CharacterDeletePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool NewCharacterPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool CharacterSelectPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool ItemListPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool UnequipItemPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool VersionPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool MagicSkillListPacket(CUserSocket* pUserSocket, const unsigned char* packet);
	static bool MoveToLocationPacket(CUserSocket* pUserSocket, const unsigned char* packet);
	static bool MoveToLocationBackPacket(CUserSocket* pUserSocket, const unsigned char* packet);
	static bool MoveToLocationInVehiclePacket(CUserSocket* pUserSocket, const unsigned char* packet);
	static bool MoveWithDeltaPacket(CUserSocket* pUserSocket, const unsigned char* packet);
	static bool HtmlCMDPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool LinkHtmlPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool WriteHtmlPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool JoinPledgePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool AnswerJoinPledgePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool LogoutPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool MagicSkillUsePacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool IconAction(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool JoinPartyPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool AnswerJoinPartyPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool EnterWorldPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool WarehouseDepositPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool WarehouseWithdrawPacket(CUserSocket *pUserSocket, const unsigned char* packet);
	static bool AppearingPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool RestartPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool RestartPointPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool GetItemFromPetPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool GiveItemToPetPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool OustPartyMemberPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool OustPledgeMemberPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool PrivateStoreListSet(CUserSocket *pSocket, const unsigned char* packet);
	static bool PrivateStoreBuyListSet(CUserSocket *pSocket, const unsigned char* packet);
	static bool PrivateStoreBuyListSend(CUserSocket *pSocket, const unsigned char* packet);
	static bool PrivateStoreBuyBuyListSend(CUserSocket *pSocket, const unsigned char* packet);
	static bool RequestEnchantItemPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool RequestPackageSendableItemList(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool RequestPackageSend(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool RequestMultiSellChoose(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool RequestUserCommand(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool SekCostume(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool StartRotatingPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool SayPacket2(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool ShowBBSBoard(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool ShortCutDeletePacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool ShortCutRegisterPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool ShowPledgeMemberListOpenPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool SellPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool SkillListPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool SocialActionPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool StartPledgeWarPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool StopPledgeWarPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool SetPledgeCrestPacket(CUserSocket *pSocket, const unsigned char* packet);
	static bool SysCMD2Packet(CUserSocket *pSocket, const unsigned char* packet);
	static bool TargetUnselectedPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool TargetUserFromMenu(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool TradeAddItemsPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool TradeDonePacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool TradeStartPacket(CUserSocket *pUserSocket, const unsigned char *packet);
	static bool TradeRequestPacket(CUserSocket *pSocket, const unsigned char *packet);
	static bool ValidateLocationPacket(CUserSocket *pSocket, const unsigned char *packet);
	static bool WithdrawPartyPacket(CUserSocket *pSocket, const unsigned char *packet);
	static bool WithdrawPledgePacket(CUserSocket *pSocket, const unsigned char *packet);
	static bool LoginPacket(CUserSocket *pSocket, const unsigned char *packet);
	
};

extern CUserPacketHandler g_UserPacketHandler;