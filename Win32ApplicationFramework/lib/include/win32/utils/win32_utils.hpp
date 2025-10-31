/*!
lib\include\win32\utils\win32_utils.hpp
Created: October 26, 2025
Updated: October 28, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Utilities header file.
*/

#pragma once

#ifndef WIN32_UTILS_HPP_
#define WIN32_UTILS_HPP_

#include <windows.h>
#include <utils/string_utils.hpp>

namespace winxframe
{
    namespace win32_utils
    {
    #pragma region Format System Message
        inline std::wstring FormatSysMessageW(DWORD msgId)
        {
            const DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER
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

            while (!sysMsg.empty() && (sysMsg.back() == L'\r' || sysMsg.back() == L'\n'))
            {
                sysMsg.pop_back();
            }

            return sysMsg;
        }
        inline std::string FormatSysMessageA(DWORD msgId)
        {
            return string_utils::ToNarrow(FormatSysMessageW(msgId));
        }
    #pragma endregion
    }; // end of namespace win32_utils
}; // end of namespace winxframe

#endif