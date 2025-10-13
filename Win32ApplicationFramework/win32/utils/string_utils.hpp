/*!
win32\win32_utils.hpp
Created: October 12, 2025
Updated: October 13, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Utilities header file.
*/

#pragma once

#ifndef WIN32_UTILS_HPP_
#define WIN32_UTILS_HPP_

#include <windows.h>
#include <string>

inline std::wstring ToWide(const char* string)
{
	if (!string) return L"NULL";
	const int wideLength = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);
	if (wideLength <= 0) return L"conversion failed";
	std::wstring wideString(static_cast<std::size_t>(wideLength - 1), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, string, -1, wideString.data(), wideLength);
	return wideString;
}
inline std::wstring ToWide(const std::string& string)
{
	return ToWide(string.c_str());
}

inline std::string ToNarrow(const wchar_t* wideString)
{
	if (!wideString) return "NULL";
	const int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
	if (utf8Length <= 0) return "conversion failed";
	std::string utf8String(static_cast<std::size_t>(utf8Length - 1), '\0');
	WideCharToMultiByte(CP_UTF8, 0, wideString, -1, utf8String.data(), utf8Length, nullptr, nullptr);
	return utf8String;
}
inline std::string ToNarrow(const std::wstring& wideString)
{
	return ToNarrow(wideString.c_str());
}

inline std::wstring FormatSysMsg(DWORD msgId) {
    const DWORD dwFlags = 
        FORMAT_MESSAGE_ALLOCATE_BUFFER
        | FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_MAX_WIDTH_MASK;

    LPWSTR rawMessage = nullptr;
    DWORD charCount = 0;
    LANGID langId = GetUserDefaultLangID();

    charCount = FormatMessageW
    (
        dwFlags,
        nullptr,
        msgId,
        langId,
        reinterpret_cast<LPWSTR>(&rawMessage),
        0,
        nullptr
    );

    if (charCount == 0 || rawMessage == nullptr)
    {
        langId = GetSystemDefaultLangID();
        charCount = FormatMessageW
        (
            dwFlags,
            nullptr,
            msgId,
            langId,
            reinterpret_cast<LPWSTR>(&rawMessage),
            0,
            nullptr
        );
    }

    if (charCount == 0 || rawMessage == nullptr)
    {
        langId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        charCount = FormatMessageW
        (
            dwFlags,
            nullptr,
            msgId,
            langId,
            reinterpret_cast<LPWSTR>(&rawMessage),
            0,
            nullptr
        );
    }

    if (charCount == 0 || rawMessage == nullptr) return L"Unknown error message";

    std::wstring result(rawMessage, charCount);
    LocalFree(rawMessage);
    return result;
}

#endif