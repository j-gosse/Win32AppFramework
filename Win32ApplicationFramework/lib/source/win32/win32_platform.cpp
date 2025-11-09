/*!
lib\source\win32\win32_platform.cpp
Created: October 5, 2025
Updated: November 8, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Platform source file.

\note
Win32 Platform is the main entry point for the application.
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
    std::unique_ptr<winxframe::Window, std::default_delete<winxframe::Window>> window = std::make_unique<winxframe::Window>(hInstance);

    try
    {
        try
        {
            // run unit tests
            winxframe::TestRegistry::RunAll();

            // console test
            console->WriteText(L"This is a really really really really really really really really long line that if necessary will be written across multiple lines based on the width of the console buffer. Does not wrap words.");
            console->WriteText(L"gggg", console_color::RED);
            console->WriteText(L"1111", console_color::BRIGHT_RED);
            console->WriteText(L"gggg");
            console->WriteText(L"gggg\n\n\n\n");
            console->WriteText(L"gggg", console_color::WHITE_ON_RED);
            console->WriteText(L"gggg", console_color::BRIGHT_WHITE);
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
            LOG_IF_ERROR(hFile == INVALID_HANDLE_VALUE);
            LOG_IF_ERROR_CTX(hFile == INVALID_HANDLE_VALUE, L"Logging a test if error.");
            THROW_IF_ERROR(hFile == INVALID_HANDLE_VALUE);
            THROW_IF_ERROR_CTX(hFile == INVALID_HANDLE_VALUE, L"Throwing a test error.");

            SetLastError(ERROR_ACCESS_DENIED);
            THROW_ERROR();
            THROW_ERROR_CTX(L"Throwing a test error.");

            SetLastError(ERROR_ACCESS_DENIED);
            SetLastError(ERROR_INVALID_NAME);
            LOG_ERROR();
            LOG_ERROR_CTX(L"Logging a test error.");
            */

            std::wcout << L"Entering the main loop...\n";
            OutputDebugStringW(L"Entering the main loop...\n");

            // main loop
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now(), currentTime;
            std::chrono::nanoseconds elapsedTime;
            while (window->ProcessMessages())
            {
                // track elapsed time
                currentTime = std::chrono::high_resolution_clock::now();
                elapsedTime = currentTime - startTime;
                window->SetElapsed(elapsedTime);
                //std::wcout << window->GetElapsed().count() << L'\n';

                /*
                This is where the main loop logic or call to a update/render loop occurs.
                For example:
                    obj->Update(window->GetWindow(), elapsedTime);
                    obj->Render(window->GetWindow());
                */

                Sleep(16);  // simulate ~60 FPS
            }

            std::wcout << L"Exiting the main loop...\n";
            OutputDebugStringW(L"Exiting the main loop...\n");
        }
        catch (const winxframe::Error& e)
        {
            e.MsgBox();
            RETHROW_ERROR_CTX(L"win32_platform.cpp inner catch context.");
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
        std::wcerr << L"Caught Error (Error class):\n" << e.wwhat() << L'\n';
        std::wcerr << e.LogCauseChain();
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;

        window.reset();
        window = nullptr;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        std::wcerr << L"Caught Error (std::exception):\n" << winxframe::string_utils::ToWide(e.what()) << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;

        window.reset();
        window = nullptr;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (const char* msg)
    {
        std::wcerr << L"Caught Error (C-string exception):\n" << winxframe::string_utils::ToWide(msg) << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;

        window.reset();
        window = nullptr;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::wcerr << L"Caught Error (unknown exception).\n";
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;

        window.reset();
        window = nullptr;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }

    window.reset();
    window = nullptr;

    std::wcout << L"\nProgram exit success. Press any key to continue..." << std::endl;
    _getch();

    console.reset();
    console = nullptr;

    return 0;
}