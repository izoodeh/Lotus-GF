#pragma once

class CHTMLCacheManager
{
	CHTMLCacheManager* lpInstance;
public:
	CHTMLCacheManager();
	~CHTMLCacheManager();
	BOOL IsValidLink(vector<HTMLCmdData>& vExpectedHtmlReply, WCHAR* wLink, int nLang = 0);
	const WCHAR* GetHTMLFile(const WCHAR* wName, int lang);
};

extern CHTMLCacheManager g_HTMLCacheManager;