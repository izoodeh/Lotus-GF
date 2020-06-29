#include "StdAfx.h"
#include "HTMLCacheManager.h"

CHTMLCacheManager g_HTMLCacheManager;

CHTMLCacheManager::CHTMLCacheManager()
{
	lpInstance = (CHTMLCacheManager*) 0x9136810;
}

CHTMLCacheManager::~CHTMLCacheManager()
{

}

BOOL CHTMLCacheManager::IsValidLink(vector<HTMLCmdData>& vExpectedHtmlReply, WCHAR* wLink, int nLang)
{
	typedef BOOL (*f)(CHTMLCacheManager*, vector<HTMLCmdData>&, WCHAR*, int);
	return f(0x669D8CL)(lpInstance, vExpectedHtmlReply, wLink, nLang);
}

const WCHAR* CHTMLCacheManager::GetHTMLFile(const WCHAR *wName, int lang)
{
	typedef const WCHAR*(*f)(CHTMLCacheManager*, const WCHAR*,int);
	return f(0x6690F4L)(lpInstance, wName, lang);
}