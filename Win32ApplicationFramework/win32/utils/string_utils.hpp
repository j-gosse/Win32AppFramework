/*!
win32\utils\string_utils.hpp
Created: October 12, 2025
Updated: October 13, 2025
Copyright (c) 2025, Jacob Gosse

String Utilities header file.
*/

#pragma once

#ifndef STRING_UTILS_HPP_
#define STRING_UTILS_HPP_

#include <windows.h>
#include <string_view>
#include <string>

#pragma region Wide Character Conversion
inline std::wstring ToWide(std::string_view string)
{
    if (string.empty()) return L"NULL";
    const int wideLength = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
    if (wideLength <= 0) return L"conversion to utf16 char failed";
    std::wstring wideString(static_cast<std::size_t>(wideLength), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), wideString.data(), wideLength);
    return wideString;
}
inline std::wstring ToWide(const char* string)
{
    return ToWide(std::string_view(string));
}
inline std::wstring ToWide(const std::string& string)
{
    return ToWide(std::string_view(string));
}
#pragma endregion

#pragma region Narrow Character Conversion
inline std::string ToNarrow(std::wstring_view wideString)
{
    if (wideString.empty()) return "NULL";
    const int narrowLength = WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), nullptr, 0, nullptr, nullptr);
    if (narrowLength <= 0) return "conversion to utf8 char failed";
    std::string narrowString(static_cast<std::size_t>(narrowLength), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), narrowString.data(), narrowLength, nullptr, nullptr);
    return narrowString;
}
inline std::string ToNarrow(const wchar_t* wideString)
{
    return ToNarrow(std::wstring_view(wideString));
}
inline std::string ToNarrow(const std::wstring& wideString)
{
    return ToNarrow(std::wstring_view(wideString));
}
#pragma endregion

#pragma region Format System Messages
inline std::wstring FormatSysMessageW(DWORD msgId)
{
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

    if (charCount == 0 || rawMessage == nullptr) return L"Unknown system message!";

    std::wstring sysMsg(rawMessage, charCount);
    LocalFree(rawMessage);

    // remove trailing \r\n
    while (!sysMsg.empty() && (sysMsg.back() == L'\r' || sysMsg.back() == L'\n'))
    {
        sysMsg.pop_back();
    }

    return sysMsg;
}
inline std::string FormatSysMessageA(DWORD msgId)
{
    return ToNarrow(FormatSysMessageW(msgId));
}
#pragma endregion

#endif