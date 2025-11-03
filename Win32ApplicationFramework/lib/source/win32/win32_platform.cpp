/*! \project    Win32ApplicationFramework
    \file       lib\source\win32\win32_platform.cpp
    \version    PRE-ALPHA 1.8
    \desc	    A lightweight C++ framework that encapsulates core Win32 API functionality 
                for console and window creation, message loops, and error handling. 
                Includes a suite for unit testing and various helper utilities. 
                Ideal as a foundation for GUI apps, game engines, debug utils, etc.
    \author     Jacob Gosse
    \created    October 5, 2025
    \updated    November 2, 2025

    \MSVC       /std:c++20
    \GNUC       -m64 -std=c++20

    Copyright (c) 2025, Jacob Gosse

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at:

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <win32/Console/Console.hpp>
#include <win32/Window/Window.hpp>
#include <win32/Error/error_macros.hpp>
#include <TestSuite/TestSuite.hpp>
#include <utils/string_utils.hpp>

using namespace winxframe;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // enable memory leak checking
    #if defined(_DEBUG) && defined(_WIN32)
    ENABLE_CRT_LEAK_CHECKING;
    #endif

    // init console
    std::unique_ptr<winxframe::Console, std::default_delete<winxframe::Console>> console = std::make_unique<winxframe::Console>(hInstance);

    // init window
    //std::unique_ptr<winxframe::Window, std::default_delete<winxframe::Window>> window = std::make_unique<winxframe::Window>(hInstance);

    // run unit tests
    winxframe::TestRegistry::RunAll();

    try
    {
        try
        {
            // console test
            console->WriteText(L"This is a really really really really really really really really long line that if necessary will be written across multiple lines based on the width of the console buffer. Does not wrap words.");
            console->WriteText(L"gggg", ConsoleColor::RED);
            console->WriteText(L"1111", ConsoleColor::BRIGHT_RED);
            console->WriteText(L"gggg");
            console->WriteText(L"gggg\n\n\n\n");
            console->WriteText(L"gggg", console_color::WHITE_ON_RED);
            console->WriteText(L"gggg", ConsoleColor::BRIGHT_WHITE);
            console->WriteText(L"1111\n");
            console->WriteText(L"gggg\n", console_color::WHITE_ON_GREEN);
            std::cout << console_color::WhiteOnRed << "cout stream color test 1\n" << console_color::Default;
            std::cout << console_color::WhiteOnGreen << "cout stream color test 1\n" << console_color::Default;
            
            // error test
            /*
            throw std::runtime_error("Runtime error test.");

            HANDLE hFile = CreateFileW(
                L"Z:\\this\\path\\does\\not\\exist\\file.txt",
                GENERIC_READ,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
            );
            LOG_IF_ERROR(hFile != INVALID_HANDLE_VALUE);
            LOG_IF_ERROR_CTX(hFile != INVALID_HANDLE_VALUE, L"Logging a test if error.");
            THROW_IF_ERROR(hFile != INVALID_HANDLE_VALUE);
            THROW_IF_ERROR_CTX(hFile != INVALID_HANDLE_VALUE, L"Throwing a test error.");

            SetLastError(ERROR_ACCESS_DENIED);
            THROW_ERROR();
            THROW_ERROR_CTX(L"Throwing a test error.");

            SetLastError(ERROR_ACCESS_DENIED);
            SetLastError(ERROR_INVALID_NAME);
            LOG_ERROR();
            LOG_ERROR_CTX(L"Logging a test error.");
            */

            std::wcout << L"Entering the main loop..." << std::endl;
            OutputDebugStringW(L"Entering the main loop...\n");

            // main loop
            //UINT wMsgFilterMin = 0;
            //UINT wMsgFilterMax = 0;
            //while (window->ProcessMessages(wMsgFilterMin, wMsgFilterMax))
            //{
                //std::wcout << window->GetElapsed() << L'\n';

                /*
                This is where the main loop logic or call to a update/render loop occurs.
                For example:
                    obj->Update(window->GetWindow(), elapsedTime);
                    obj->Render(window->GetWindow());
                */

                //Sleep(16);  // simulate ~60 FPS
            //}

            std::wcout << L"Exiting the main loop..." << std::endl;
            OutputDebugStringW(L"Exiting the main loop...\n");
        }
        catch (const winxframe::Error& e)
        {
            e.MsgBox();
            RETHROW_ERROR_CTX(L"This is the inner catch context.");
        }
        catch (const std::exception& e)
        {
            MessageBoxExA(nullptr, e.what(), "Standard Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
            RETHROW_ERROR();
        }
        catch (const char* msg)
        {
            MessageBoxExA(nullptr, msg, "C-string Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
            RETHROW_ERROR();
        }
        catch (const wchar_t* wideMsg)
        {
            MessageBoxExW(nullptr, wideMsg, L"Wide String Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
            RETHROW_ERROR();
        }
        catch (...)
        {
            const char* msg = "Unknown exception.";
            MessageBoxExA(nullptr, msg, "Unknown Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
            RETHROW_ERROR();
        }
    }
    catch (const winxframe::Error& e)
    {
        std::wcerr << L"Caught Error (Error class): " << e.wwhat() << L'\n';
        std::wcerr << e.LogCauseChain();
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        std::wcerr << L"Caught Error (std::exception): " << winxframe::string_utils::ToWide(e.what()) << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        return EXIT_FAILURE;
    }
    catch (const char* msg)
    {
        std::wcerr << L"Caught Error (C-string exception): " << winxframe::string_utils::ToWide(msg) << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::wcerr << L"Caught Error (unknown exception)\n";
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        return EXIT_FAILURE;
    }

    //window.reset();
    //window = nullptr;

    std::wcout << L"\nProgram exit success. Press any key to continue..." << std::endl;
    _getch();

    console.reset();
    console = nullptr;

    return 0;
}