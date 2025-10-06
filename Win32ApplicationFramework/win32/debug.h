/*!
win32\debug.h
Created: October 5, 2025
Updated: October 5, 2025
Copyright (c) 2025, Jacob Gosse

Debug header file : win32 debug file, currently focuses on memory leak checking.

_CrtSetDbgFlag Example Usage:
https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/crtsetdbgflag?view=msvc-170
*/

#pragma once

#ifndef DEBUG_H_
#define DEBUG_H_

#if defined(_DEBUG) && defined(_WIN32)
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC

#ifdef _CRTDBG_MAP_ALLOC
#define ENABLE_CRT_LEAK_CHECKING\
            do {\
                int newFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);\
                newFlag |= _CRTDBG_CHECK_ALWAYS_DF;\
                newFlag |= _CRTDBG_DELAY_FREE_MEM_DF;\
                newFlag |= _CRTDBG_LEAK_CHECK_DF;\
                _CrtSetDbgFlag(newFlag);\
                _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);\
                _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);\
            } while (0)

#define ENABLE_CRT_DELAY_FREE_MEM_DF\
            do {\
                int newFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);\
                newFlag |= _CRTDBG_DELAY_FREE_MEM_DF;\
                _CrtSetDbgFlag(newFlag);\
            } while (0)

#define DISABLE_CRT_DELAY_FREE_MEM_DF\
            do {\
                int newFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);\
                newFlag &= ~_CRTDBG_DELAY_FREE_MEM_DF;\
                _CrtSetDbgFlag(newFlag);\
            } while (0)
#endif

#else
#define ENABLE_CRT_LEAK_CHECKING    ((void)0)
#define DISABLE_CRT_DELAY_FREE_MEM  ((void)0)
#endif

#if defined(_DEBUG) && defined(_WIN32)
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define new DEBUG_NEW
#endif

#endif
