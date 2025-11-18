/*!
lib\include\win32\utils\win32_utils.hpp
Created: October 26, 2025
Updated: November 18, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Utilities header file.
*/

#pragma once

#ifndef WIN32_UTILS_HPP_
#define WIN32_UTILS_HPP_

#include <windows.h>
#include <chrono>
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

    #pragma region Frame Limiters
        inline void HybridVSyncFrameLimiter(const std::chrono::nanoseconds targetFrameTime, const std::chrono::high_resolution_clock::time_point frameStart)
        {
            std::chrono::nanoseconds frameDuration = std::chrono::high_resolution_clock::now() - frameStart;

            if (frameDuration < targetFrameTime)
            {
                std::chrono::nanoseconds sleepTime = targetFrameTime - frameDuration;

                // sleep most of the remaining time
                if (sleepTime > std::chrono::milliseconds(2))
                    std::this_thread::sleep_for(sleepTime - std::chrono::milliseconds(1));

                // busy-wait the remaining ~1-2ms
                while (std::chrono::high_resolution_clock::now() - frameStart < targetFrameTime);
            }
        }

        inline void HighPrecisionFrameLimiter(const std::chrono::nanoseconds targetFrameTime)
        {
            static std::chrono::high_resolution_clock::time_point nextFrameTime = std::chrono::high_resolution_clock::now();
            nextFrameTime += targetFrameTime;

            while (true)
            {
                std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
                std::chrono::nanoseconds remaining = nextFrameTime - now;

                if (remaining <= std::chrono::nanoseconds::zero())
                    break; // time for next frame

                if (remaining > std::chrono::milliseconds(2))
                {
                    // sleep most of the remaining time
                    long long sleepMs = std::chrono::duration_cast<std::chrono::milliseconds>(remaining - std::chrono::milliseconds(1)).count();
                    if (sleepMs > 0)
                        Sleep(static_cast<DWORD>(sleepMs));
                }
                else
                {
                    // cooperative yielding the remaining ~1-2ms
                    std::this_thread::yield();
                }
            }
        }
    #pragma endregion
    }; // end of namespace win32_utils
}; // end of namespace winxframe

#endif