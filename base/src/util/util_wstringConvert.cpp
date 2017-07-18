#pragma once

#include "util_wstringConvert.h"

std::wstring s2ws(const std::string& str)
{
	//not interested in the semantic of the content
	//return std::wstring((const wchar_t*)&str[0], sizeof(char) / sizeof(wchar_t)*str.size());


	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
	//not interested in the semantic of the content
	//return std::string((const char*)&wstr[0], sizeof(wchar_t) / sizeof(char)*wstr.size());


	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr);
}