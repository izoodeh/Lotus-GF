#pragma once

namespace Parser
{
	INT32 GetBypassValueInt(const WCHAR* wBypass, const WCHAR* wName, int defaultValue = 0);
	wstring GetBypassValueString(wstring wBypass, wstring wSub);
	INT32 ParseOptionInt(string line, string option, INT32 defaultValue = 0);
	INT32 ParseOptionInt(wstring line, wstring option, INT32 defaultValue = 0);
	UINT ParseOptionUInt(wstring line, wstring option, UINT defaultValue = 0);
	double ParseOptionDouble(wstring line, wstring option, double defaultValue = 0);
	double ParseOptionDouble(string line, string option, double defaultValue = 0);
	string ParseOptionString(string line, string option, string sDefault = "");
	wstring ParseOptionString(wstring line, wstring option, wstring sDefault = L"");
	wstring Replace(wstring str, wchar_t sign, wchar_t new_sign);
	wstring Replace(wstring str, wstring oldStr, wstring newStr);
	string Replace(string str, string oldStr, string newStr);
	string Replace(string str, char sign, char new_sign);
	vector<wstring> Split(wstring line, wstring separators);
	vector<string> Split(string line, string separators);
	bool IsValidSqlString(const WCHAR* wInput);
	wstring Trim(wstring str);
	WCHAR ToWLower(WCHAR c);
};