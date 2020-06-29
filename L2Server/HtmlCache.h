#pragma once

class CHtmlCache
{
	CSLock m_Lock;
	map<wstring, wstring> m_Htmls[9];
	bool m_Enabled;
	bool m_MultiLang;
	bool m_Caching;

	void LoadINI();
public:
	CHtmlCache();
	inline bool IsEnabled() { return m_Enabled; };
	inline bool IsMultiLang() { return m_MultiLang; };
	void Init();
	void LoadHtmls(UINT lang = 1);
	const WCHAR* Get(wstring name, UINT lang);
	const WCHAR* Load(wstring name, UINT lang);
	void ClearCache();
	static const WCHAR* GetHTMLFileHook(LPVOID lpInstance, const WCHAR* name, UINT lang);
	static void ToggleCachingHook(LPVOID lpInstance);
};

extern CHtmlCache g_HtmlCache;