#include "stdafx.h"
#include "Parser.h"

WCHAR Parser::ToWLower(WCHAR c)
{
	if(c >= 0x410 && c <= 0x42F)
	{
		c += 0x20;
	}else
	{
		c = towlower(c);
	}
	
	return c;
}

wstring Parser::Trim(wstring str)
{
	wstring ret;

	for(UINT n=0;n<str.size();n++)
	{
		if(str[n] != L' ' && str[n] != L'\t' && str[n] != L'\r' && str[n] != L'\n')
		{
			ret.push_back(str[n]);
		}
	}

	return ret;
}

int Parser::GetBypassValueInt(const WCHAR* wBypass, const WCHAR* wName, int defaultValue)
{
	if(wBypass && wName)
	{
		size_t bypassLen = wcslen(wBypass);
		size_t nameLen = wcslen(wName);
		if(bypassLen > nameLen && nameLen)
		{
			WCHAR wStr[4096]; memset(wStr, 0, sizeof(wStr));
			//trim str
			for(int n=0, m=0; n < bypassLen; n++)
			{
				if( wBypass[n] != L' ' && wBypass[n] != L'\t' )
				{
					wStr[m] = wBypass[n];
					m++;
				}
			}

			 const WCHAR* wPos = wcsstr(wStr, wName);
			 bool found = false;
			 while(wPos && !found)
			 {
				 if( wPos != wStr && wPos[-1] == L'?' || wPos[-1] == L'&')
				 {
					 if( wPos[nameLen] == L'=' )
					 {
						 found = true;
						 break;
					 }
				 }
				 wPos = wcsstr(&wPos[1], wName);
			 }

			 if(found)
			 {

				 INT32 ret = _wtoi(&wPos[nameLen+1]);
				
				 return ret;
			 }
		}
	}

	return defaultValue;
}

wstring Parser::GetBypassValueString(wstring wBypass, wstring wSub)
{
	wstring wRet;
	//Remove white spaces from bypass
	wBypass = Trim(wBypass);
	//Find wSub
	size_t pos = wBypass.find(wSub);
	if(pos!=wstring::npos)
	{
		wBypass = wBypass.substr(pos, (wBypass.size() - pos));
		pos = wBypass.find(L"&");
		if(pos!=wstring::npos)
		{
			wBypass = wBypass.substr(0, pos);
		}

		pos = wBypass.find(L"=");
		if(pos == wstring::npos)
		{
			//not found
			return L"";
		}
		pos++;

		wRet = wBypass.substr(pos, (wBypass.size() - pos));
	}
	return wRet;
}

INT32 Parser::ParseOptionInt(string line, string option, INT32 defaultValue)
{
	string param;
	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=string::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != ' ' && line[pos-1] != '\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != '=' && line[pos + optionSize] != ' ' && line[pos + optionSize] != '\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find("=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			stringstream sstr(param);
			sstr >> defaultValue;
		}
		break;
	}

	return defaultValue;
}

INT32 Parser::ParseOptionInt(wstring line, wstring option, INT32 defaultValue)
{
	wstring param;
	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=wstring::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != L' ' && line[pos-1] != L'\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != L'=' && line[pos + optionSize] != L' ' && line[pos + optionSize] != L'\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find(L"=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			wstringstream sstr(param);
			sstr >> defaultValue;
		}
		break;
	}

	return defaultValue;
}

UINT Parser::ParseOptionUInt(wstring line, wstring option, UINT defaultValue)
{
	wstring param;
	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=wstring::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != L' ' && line[pos-1] != L'\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != L'=' && line[pos + optionSize] != L' ' && line[pos + optionSize] != L'\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find(L"=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			wstringstream sstr(param);
			sstr >> defaultValue;
		}
		break;
	}

	return defaultValue;
}

double Parser::ParseOptionDouble(string line, string option, double defaultValue)
{
	string param;
	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=string::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != ' ' && line[pos-1] != '\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != '=' && line[pos + optionSize] != ' ' && line[pos + optionSize] != '\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find("=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			stringstream sstr(param);
			sstr >> defaultValue;
		}
		break;
	}

	return defaultValue;
}

double Parser::ParseOptionDouble(wstring line, wstring option, double defaultValue)
{
	wstring param;
	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=wstring::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != L' ' && line[pos-1] != L'\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != L'=' && line[pos + optionSize] != L' ' && line[pos + optionSize] != L'\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find(L"=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			wstringstream sstr(param);
			sstr >> defaultValue;
		}
		break;
	}

	return defaultValue;
}

string Parser::ParseOptionString(string line, string option, string sDefault)
{
	string param;
	
	string value(sDefault);

	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=string::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != ' ' && line[pos-1] != '\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != '=' && line[pos + optionSize] != ' ' && line[pos + optionSize] != '\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find("=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			stringstream sstr(param);
			sstr >> value;
		}
		break;
	}

	return value;
}

wstring Parser::ParseOptionString(wstring line, wstring option, wstring sDefault)
{
	wstring param;
	wstring value(sDefault);

	size_t optionSize = option.size();
	size_t pos = line.find(option);
	while( pos!=wstring::npos )
	{
		if( pos > 0 )
		{
			if( line[pos-1] != L' ' && line[pos-1] != L'\t' )
			{
				pos++;
				pos = line.find(option, pos);
				continue;
			}
		}
		
		if( line[pos + optionSize] != L'=' && line[pos + optionSize] != L' ' && line[pos + optionSize] != L'\t' )
		{
			pos++;
			pos = line.find(option, pos);
			continue;
		}

		pos = line.find(L"=", pos);
		if( pos != string::npos )
		{
			param = line.substr((pos + 1), (line.size() - pos  -1));
			wstringstream sstr(param);
			sstr >> value;
		}
		break;
	}

	return value;
}

wstring Parser::Replace(wstring str, wstring oldStr, wstring newStr)
{
	size_t pos = str.find(oldStr);
	while(pos!=wstring::npos)
	{
		str = str.replace(pos, oldStr.size(), newStr);
		pos = str.find(oldStr);
	}
	return str;
}

string Parser::Replace(string str, string oldStr, string newStr)
{
	size_t pos = str.find(oldStr);
	while(pos!=string::npos)
	{
		str = str.replace(pos, oldStr.size(), newStr);
		pos = str.find(oldStr);
	}
	return str;
}

wstring Parser::Replace(wstring str, wchar_t sign, wchar_t new_sign)
{
	size_t pos = str.find(sign);
	while(pos!=wstring::npos)
	{
		str[pos] = new_sign;
		pos = str.find(sign);
	}
	return str;
}

string Parser::Replace(string str, char sign, char new_sign)
{
	size_t pos = str.find(sign);
	while(pos!=string::npos)
	{
		str[pos] = new_sign;
		pos = str.find(sign);
	}
	return str;
}

vector<string> Parser::Split(string line, string separator)
{
	vector<string> data;
	string ex = "";
	for(UINT n=0;n<line.size();n++)
	{
		bool newL = false;
		for(UINT m = 0; m < separator.size(); m++)
		{
			if(line[n] == separator[m])
			{
				newL = true;
				break;
			}
		}
		if(newL)
		{
			data.push_back(ex);
			ex = "";
			continue;
		}
		ex += line[n];
	}
	
	if(ex.size() > 0)
	{
		data.push_back(ex);
	}
	return data;
}

vector<wstring> Parser::Split(wstring line, wstring separator)
{
	vector<wstring> data;
	wstring ex = L"";
	for(UINT n=0;n<line.size();n++)
	{
		bool newL = false;
		for(UINT m = 0; m < separator.size(); m++)
		{
			if(line[n] == separator[m])
			{
				newL = true;
				break;
			}
		}
		if(newL)
		{
			data.push_back(ex);
			ex = L"";
			continue;
		}
		ex += line[n];
	}
	if(ex.size() > 0)
	{
		data.push_back(ex);
	}
	return data;
}

bool Parser::IsValidSqlString(const WCHAR* wInput)
{
	bool ret = true;

	const WCHAR* invalidChars = L"\'\"\\;`";

	//check for not allowed signs
	size_t len = wcslen(wInput);
	for(size_t n=0;n<len;n++)
	{
		for(UINT m=0;m<5;m++)
		{
			if(wInput[n] == invalidChars[m])
			{
				ret = false;
				break;
			}
		}
		if(!ret)
		{
			break;
		}
	}

	return ret;
}