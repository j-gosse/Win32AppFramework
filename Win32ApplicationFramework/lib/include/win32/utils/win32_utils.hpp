/*!
lib\include\win32\utils\win32_utils.hpp
Created: October 26, 2025
Updated: November 22, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Utilities header file.
*/

#pragma once

#ifndef WIN32_UTILS_HPP_
#define WIN32_UTILS_HPP_

#include <win32/Error/error_macros.hpp>
#include <windows.h>
#include <chrono>

namespace winxframe
{
    namespace win32_utils
    {
    #pragma region STARTUP
        inline int ResolveShowCmd(int nCmdShow = SW_SHOWDEFAULT)
        {
            STARTUPINFO si = { sizeof(si) };
            GetStartupInfoW(&si);

            if (si.dwFlags & STARTF_USESHOWWINDOW)
                return si.wShowWindow;
            else if (nCmdShow == SW_SHOWDEFAULT)
                return SW_SHOWNORMAL;
            else
                return nCmdShow;
        }
    #pragma endregion STARTUP

    #pragma region RENDERING
        inline void RenderFPS(HDC hMemoryDC, double fps)
        {
            if (!hMemoryDC)
                return;

            wchar_t fpsText[32];
            swprintf_s(fpsText, 32, L"FPS: %d", (int)std::round(fps));

            HFONT oldFont = (HFONT)SelectObject(hMemoryDC, GetStockObject(DEFAULT_GUI_FONT));
            SetBkMode(hMemoryDC, TRANSPARENT);
            SetTextColor(hMemoryDC, RGB(255, 255, 255)); // white text

            SIZE textSize;
            GetTextExtentPoint32W(hMemoryDC, fpsText, (int)wcslen(fpsText), &textSize);

            const int x = 10;
            const int y = 10;
            TextOutW(hMemoryDC, x, y, fpsText, (int)wcslen(fpsText));

            SelectObject(hMemoryDC, oldFont);
        }
    #pragma endregion RENDERING

    #pragma region FRAME LIMITERS
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
    #pragma endregion FRAME LIMITERS

    #pragma region SYSTEM MESSAGES
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
        //inline std::string FormatSysMessageA(DWORD msgId)
        //{
        //    return string_utils::ToNarrow(FormatSysMessageW(msgId));
        //}
    #pragma endregion SYSTEM MESSAGES

    #pragma region SYSTEM INFORMATION
        inline void SysInfo()
        {
            SYSTEM_INFO systemInfo_{};

            WCHAR buffer1[UNLEN + 1]{};
            DWORD size1 = UNLEN + 1;
            GetUserNameW(buffer1, &size1);
            std::wcout << L"\nUsername: " << buffer1 << L'\n';

            WCHAR buffer2[MAX_COMPUTERNAME_LENGTH + 1]{};
            DWORD size2 = MAX_COMPUTERNAME_LENGTH + 1;
            GetComputerNameW(buffer2, &size2);
            std::wcout << L"Computer Name: " << buffer2 << L'\n';

            /*
            typedef struct _SYSTEM_INFO {
              union {
                DWORD dwOemId;
                struct {
                  WORD wProcessorArchitecture;
                  WORD wReserved;
                } DUMMYSTRUCTNAME;
              } DUMMYUNIONNAME;
              DWORD     dwPageSize;
              LPVOID    lpMinimumApplicationAddress;
              LPVOID    lpMaximumApplicationAddress;
              DWORD_PTR dwActiveProcessorMask;
              DWORD     dwNumberOfProcessors;
              DWORD     dwProcessorType;
              DWORD     dwAllocationGranularity;
              WORD      wProcessorLevel;
              WORD      wProcessorRevision;
            } SYSTEM_INFO, *LPSYSTEM_INFO;

            Use the wProcessorArchitecture, wProcessorLevel, and wProcessorRevision members to determine the type of processor.
            */
            GetNativeSystemInfo(&systemInfo_);

            std::wcout << L"Processor Architecture: ";
            switch (systemInfo_.wProcessorArchitecture)
            {
            case PROCESSOR_ARCHITECTURE_INTEL:
                std::wcout << L"x86\n";
                break;
            case PROCESSOR_ARCHITECTURE_AMD64:
                std::wcout << L"x64 (AMD or Intel)\n";
                break;
            case PROCESSOR_ARCHITECTURE_ARM:
                std::wcout << L"ARM\n";
                break;
            case PROCESSOR_ARCHITECTURE_ARM64:
                std::wcout << L"ARM64\n";
                break;
            case PROCESSOR_ARCHITECTURE_IA64:
                std::wcout << L"Intel Itanium-based\n";
                break;
            default:
                std::wcout << L"Unknown architecture\n";
                break;
            }
            std::wcout << L"Processor Level: " << systemInfo_.wProcessorLevel << L'\n';
            std::wcout << L"Processor Revision: " << systemInfo_.wProcessorRevision << L'\n';
            std::wcout << L"Logical Processors (threads): " << systemInfo_.dwNumberOfProcessors << L'\n';

            WORD revision = systemInfo_.wProcessorRevision;
            BYTE model = HIBYTE(revision);
            BYTE stepping = LOBYTE(revision);

            std::wcout << L"CPU Model: " << model << L'\n';
            std::wcout << L"Stepping: " << stepping << L'\n';

            std::wcout << std::endl;
        }

        inline void ProcessorInfo()
        {
            PROCESS_INFORMATION processInfo_{};

            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
            DWORD size = 0;
            DWORD error = GetLastError();

            THROW_IF_ERROR_CTX(GetLogicalProcessorInformation(nullptr, &size) && error != ERROR_INSUFFICIENT_BUFFER, L"Failed to get required buffer size for logical processor info!");

            /*
            typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION
            {
                ULONG_PTR ProcessorMask;
                LOGICAL_PROCESSOR_RELATIONSHIP Relationship;

                union
                {
                    struct
                    {
                        BYTE Flags;
                    } ProcessorCore;

                    struct
                    {
                        DWORD NodeNumber;
                    } NumaNode;

                    CACHE_DESCRIPTOR Cache;
                    ULONGLONG Reserved[2];
                } DUMMYUNIONNAME;
            } SYSTEM_LOGICAL_PROCESSOR_INFORMATION, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;
            */

            buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(size);
            THROW_IF_ERROR_CTX(!buffer, L"Failed to allocate memory for logical processor info!");

            if (!GetLogicalProcessorInformation(buffer, &size))
            {
                free(buffer);
                THROW_ERROR_CTX(L"Failed to get logical processor information!");
            }

            DWORD count = size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            for (DWORD i = 0; i < count; ++i)
                printf("Processor %u: Relationship = %d\n", i, buffer[i].Relationship);

            free(buffer);
            std::wcout << L'\n';
        }
    #pragma endregion SYSTEM INFORMATION
    }; // end of namespace win32_utils
}; // end of namespace winxframe

#endif