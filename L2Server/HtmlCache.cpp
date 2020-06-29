#include "stdafx.h"
#include "HtmlCache.h"
#include "HookManager.h"
#include "HTMLCacheManager.h"

CHtmlCache g_HtmlCache;

CHtmlCache::CHtmlCache() : m_MultiLang(false), m_Caching(true)
{
	
}

void CHtmlCache::Init()
{
	m_Enabled = false;
	LoadINI();
	if(m_Enabled)
	{
		WriteCall(0x6AF73B, CHtmlCache::ToggleCachingHook);

		WriteCall(0x445159, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x445321, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x47D771, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x47F7A6, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x54E783, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x5B5B31, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x5B6377, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x61D735, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x673D63, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x73C060, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x746E95, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x77BF81, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x81B4BA, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x920390, CHtmlCache::GetHTMLFileHook);
		WriteCall(0x94A4B4, CHtmlCache::GetHTMLFileHook);


		WriteBYTE(0x6688C8, 0xC3);
		WriteBYTE(0x6688C9, 0x90);
		WriteBYTE(0x6688CA, 0x90);

		g_Log.Add(CLog::Blue, "[%s] Feature is enabled.", __FUNCTION__);
	}

}

const WCHAR* CHtmlCache::GetHTMLFileHook(LPVOID lpInstance, const WCHAR *name, UINT lang)
{
	guard;
	const WCHAR* wHtml = L"<html><body>NO HTML</body></html>";
	if(g_HtmlCache.m_Caching)
	{
		try
		{
			if(name)
			{
				wHtml = g_HtmlCache.Get(name, lang);
			}
		}catch(...)
		{
			EXCEPTION_LOG;
		}
	}else
	{
		if(name)
		{
			wHtml = g_HtmlCache.Load(name, lang);
		}
	}
	unguard;
	return wHtml;
}

void CHtmlCache::ToggleCachingHook(LPVOID lpInstance)
{
	guard;

	if(g_HtmlCache.IsEnabled())
	{
		if(g_HtmlCache.m_Caching)
		{
			g_HtmlCache.m_Caching = false;
		}else
		{
			g_HtmlCache.m_Caching = true;
		}
	}

	typedef void(*f)(LPVOID);
	f(0x669364L)(lpInstance);
	unguard;
}

void CHtmlCache::LoadINI()
{
	guard;
	const TCHAR* section = _T("HtmlCache");
	m_Enabled = GetPrivateProfileInt(section, _T("Enabled"), FALSE, g_ConfigFile) ? true : false;
	m_MultiLang = GetPrivateProfileInt(section, _T("MultiLang"), FALSE, g_ConfigFile) ? true : false;
	unguard;
}

void CHtmlCache::LoadHtmls(UINT lang)
{
	guard;
	if(m_Enabled)
	{
		if(lang > 8)
		{
			unguard;
			return;
		}

		TCHAR path[260] = { 0 };
		TCHAR searchPath[260] = { 0 };
		GetCurrentDirectory(MAX_PATH, path);
		//;Default 0, Korea=0, USA=1, Japan=2, Taiwan=3, China=4, Thailand=5, Philippines = 6, Russia = 8
		if(m_MultiLang)
		{
			if(lang == 0)
			{
				lstrcat(path, TEXT("\\..\\html\\Korea\\"));
			}else if(lang == 1)
			{
				lstrcat(path, TEXT("\\..\\html\\USA\\"));
			}else if(lang == 2)
			{
				lstrcat(path, TEXT("\\..\\html\\Japan\\"));
			}else if(lang == 3)
			{
				lstrcat(path, TEXT("\\..\\html\\Taiwan\\"));
			}else if(lang == 4)
			{
				lstrcat(path, TEXT("\\..\\html\\China\\"));
			}else if(lang == 5)
			{
				lstrcat(path, TEXT("\\..\\html\\Thailand\\"));
			}else if(lang == 6)
			{
				lstrcat(path, TEXT("\\..\\html\\Philippines\\"));
			}else if(lang == 7)
			{
				lstrcat(path, TEXT("\\..\\html\\Other\\"));
			}else if(lang == 8)
			{
				lstrcat(path, TEXT("\\..\\html\\Russia\\"));
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Invalid language[%d]!", __FUNCTION__, lang);
			}
		}else
		{
			lstrcat(path, TEXT("\\..\\html\\"));
		}

		lstrcat(searchPath, path);
		lstrcat(searchPath, L"*.htm");

		g_Log.Add(CLog::Blue, "[%s] Initializing path[%S] lang[%d].", __FUNCTION__, searchPath, lang);

		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(searchPath, &findFileData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == false)
				{
					wstringstream filePath;
					filePath << path << findFileData.cFileName;
					LPBYTE lpFile = 0;
					UINT len = ReadFileBinary(filePath.str().c_str(), lpFile);

					if(lpFile)
					{
						if(len < 16384)
						{
							if(len >= 2)
							{
								if(lpFile[0] == 0xFF && lpFile[1] == 0xFE)
								{
									wstring html((PWCHAR)&lpFile[2]);

									wstring name(findFileData.cFileName);
									transform(name.begin(), name.end(), name.begin(), towlower);
									m_Htmls[lang][name] = html;
								}else
								{
									g_Log.Add(CLog::Error, "[%s] File not in unicode encoding [%S] !", __FUNCTION__, filePath.str().c_str());
								}
							}else
							{
								g_Log.Add(CLog::Error, "[%s] File not in unicode encoding [%S] !", __FUNCTION__, filePath.str().c_str());
							}
						}else
						{
							g_Log.Add(CLog::Error, "[%s] Too big html file[%S] size[%d]!", __FUNCTION__, filePath.str().c_str(), len);
						}
						delete [] lpFile;
					}
				}

			}while( FindNextFile(hFind, &findFileData) != FALSE );
			g_Log.Add(CLog::Blue, "[%s] Loaded [%d] html(s).", __FUNCTION__, m_Htmls[lang].size());
		}else
		{
			g_Log.Add(CLog::Error, "[%s] Cannot find first file - path[%S] !", __FUNCTION__, path);
		}
		FindClose(hFind);
		m_Caching = false;
		g_HtmlCache.ToggleCachingHook((LPVOID)0x9136810L);
	}
	unguard;
}

const WCHAR* CHtmlCache::Get(wstring name, UINT lang)
{
	guard;
	const WCHAR* wHtml = 0;
	if(m_Enabled)
	{
		if(!m_MultiLang || lang > 8)
		{
			lang = 1;
		}

		if(m_Caching)
		{
			transform(name.begin(), name.end(), name.begin(), towlower);
			map<wstring, wstring>::iterator it = m_Htmls[lang].find(name);
			if(it!=m_Htmls[lang].end())
			{
				wHtml = it->second.c_str();
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Cannot find html[%S] lang[%d]!", __FUNCTION__, name.c_str(), lang);
				wHtml = L"<html><body>NO HTML</body></html>";
			}
		}else
		{
			wHtml = Load(name, lang);
		}
	}else
	{
		wHtml = g_HTMLCacheManager.GetHTMLFile(name.c_str(), 0);
	}

	unguard;
	return wHtml;
}

const WCHAR* CHtmlCache::Load(wstring name, UINT lang)
{
	guard;
	const WCHAR* wHtml = L"<html><body>NO HTML</body></html>";
	wstring path;
	if(!m_MultiLang)
	{
		path = L"..\\html\\";
	}else
	{
		if(lang == 0)
		{
			path = L"\\..\\html\\Korea\\";
		}else if(lang == 1)
		{
			path = L"..\\html\\USA\\";
		}else if(lang == 2)
		{
			path = L"..\\html\\Japan\\";
		}else if(lang == 3)
		{
			path = L"..\\html\\Taiwan\\";
		}else if(lang == 4)
		{
			path = L"..\\html\\China\\";
		}else if(lang == 5)
		{
			path = L"..\\html\\Thailand\\";
		}else if(lang == 6)
		{
			path = L"..\\html\\Philippines\\";
		}else if(lang == 7)
		{
			path = L"..\\html\\Other\\";
		}else if(lang == 8)
		{
			path = L"..\\html\\Russia\\";
		}else
		{
			path = L"..\\html\\";
		}
	}
	path += name;
	if(path.find(L".htm") == (path.size() - 4))
	{
		LPBYTE lpFile = 0;
		UINT len = ReadFileBinary(path.c_str(), lpFile);

		if(lpFile)
		{
			if(len < 16384)
			{
				if(len >= 2)
				{
					if(lpFile[0] == 0xFF && lpFile[1] == 0xFE)
					{
						wstring html((PWCHAR)&lpFile[2]);
						transform(name.begin(), name.end(), name.begin(), towlower);
						m_Htmls[lang][name] = html;
						wHtml = m_Htmls[lang][name].c_str();
						g_Log.Add(CLog::Black, "[%s] path[%S] lang[%d]", __FUNCTION__, path.c_str(), lang);
					}else
					{
						g_Log.Add(CLog::Error, "[%s] File not in unicode encoding [%S] !", __FUNCTION__, path.c_str());
					}
				}else
				{
					g_Log.Add(CLog::Error, "[%s] File not in unicode encoding [%S] !", __FUNCTION__, path.c_str());
				}
			}else
			{
				g_Log.Add(CLog::Error, "[%s] Too big html file[%S] size[%d]!", __FUNCTION__, path.c_str(), len);
			}
			delete [] lpFile;
		}
	}
	unguard;
	return wHtml;
}


void CHtmlCache::ClearCache()
{
	guard;

	m_Lock.Enter();
	for(UINT n=0;n<9;n++)
	{
		m_Htmls[n].clear();
	}
	m_Lock.Leave();
	unguard;
}