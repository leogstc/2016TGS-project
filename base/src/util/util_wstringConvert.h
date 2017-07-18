#pragma once

#include <string> // std::string std::u32string
#include <locale> // std::wstring_convert
#include <codecvt> // std::codecvt_utf8
//#include <cstdint> // std::uint_least32_t

std::wstring s2ws(const std::string& str);
std::string ws2s(const std::wstring& wstr);