#pragma once

struct ChatSetting
{
	BOOL enabled;
	DWORD delay;
	INT32 minLevel;
	vector<pair<WORD, WORD>> charset;
	vector<pair<wstring, wstring>> wordToReplace;
	wstring invalidCharMsg;
};

class CChatManager : public CFeature
{
	BOOL enabled;
	BOOL sendMessage;
	BOOL globalChat;
	INT32 messageId;
	ChatSetting chatSetting[18];
	void LoadINI();
public:
	CChatManager();
	~CChatManager();
	void Init();
	bool Handle(CUserSocket *pSocket, const unsigned char* packet, int type);
};

extern CChatManager g_ChatManager;
