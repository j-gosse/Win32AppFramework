/*!
win32\win32utils.hpp
Created: October 12, 2025
Updated: October 12, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Utilities header file.
*/

#pragma once

#ifndef WIN32_UTILS_HPP_
#define WIN32_UTILS_HPP_

#include <windows.h>
#include <string>

inline std::wstring WideChar(const char* cString)
{
    if (!cString) return L"NULL";
    const auto charLength = MultiByteToWideChar(CP_UTF8, 0, cString, -1, nullptr, 0);
    if (charLength == 0) return L"conversion failed";
    std::wstring wideString(static_cast<size_t>(charLength) - 1, L'\0'); // Exclude null terminator
    MultiByteToWideChar(CP_UTF8, 0, cString, -1, wideString.data(), charLength);
    return wideString;
}

inline std::wstring WideChar(const std::string& string)
{
    return WideChar(string.c_str());
}

#endif