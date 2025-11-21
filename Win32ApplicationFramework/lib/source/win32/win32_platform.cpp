/*!
lib\source\win32\win32_platform.cpp
Created: October 5, 2025
Updated: November 20, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Platform source file.

\note
Win32 Platform is the main (wWinMain) entry point for the application.
*/

#include <win32/Console/Console.hpp>
#include <win32/Window/Window.hpp>
#include <win32/Error/error_macros.hpp>
#include <TestSuite/TestSuite.hpp>
#include <win32/utils/win32_utils.hpp>
#include <utils/string_utils.hpp>

using namespace winxframe;

// derived window class test
class TestWindow : public Window
{
public:
    TestWindow(
        const std::wstring& title = L"",
        LONG windowWidth = WINDOW_WIDTH,
        LONG windowHeight = WINDOW_HEIGHT,
        MessagePumpMode mode = MessagePumpMode::RealTime
    )
        : Window(title, windowWidth, windowHeight, mode) {}

    TestWindow(
        HINSTANCE hInstance,
        const std::wstring& title = L"",
        LONG windowWidth = WINDOW_WIDTH,
        LONG windowHeight = WINDOW_HEIGHT,
        MessagePumpMode mode = MessagePumpMode::RealTime
    )
        : Window(hInstance, title, windowWidth, windowHeight, mode) {}

    ~TestWindow() override
    {
        std::wcout << L"DESTRUCTOR: ~MyWindow()\n";
    }

    void Update(std::chrono::nanoseconds deltaTime) override
    {
        UNREFERENCED_PARAMETER(deltaTime);
        // update logic
        //std::wcout << L"Updating window: " << deltaTime.count() << L" ns\n";
    }

    void Render() override
    {
        if (!GetMemoryBitmap())
            return;

        BeginFrame();
        win32_utils::RenderFPS(GetMemoryDC(), GetFPS());
        Present();
    }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // enable memory leak checking
    #if defined(_DEBUG) && defined(_WIN32)
        ENABLE_CRT_LEAK_CHECKING;
    #endif

    // console pointer
    COORD consoleSize = { 120, 30 };
    std::unique_ptr<winxframe::Console> console;

    // windows vector
    std::vector<std::unique_ptr<Window>> windows;

    try
    {
        // init console
        console = std::make_unique<winxframe::Console>(hInstance, L"CONSOLE", consoleSize.X, consoleSize.Y);

        // init window(s)
        LONG windowWidth = 800;
        LONG windowHeight = 600;
        windows.push_back(std::make_unique<TestWindow>(hInstance, L"WINDOW1", windowWidth, windowHeight, MessagePumpMode::RealTime));
        //windows.push_back(std::make_unique<TestWindow>(hInstance, L"WINDOW2", 600, 400, MessagePumpMode::RealTime));
        //windows.push_back(std::make_unique<TestWindow>(hInstance, L"WINDOW3", 400, 200, MessagePumpMode::EventDriven));

        // run unit tests
        winxframe::TestRegistry::RunAll();

        // console test
        console->WriteText(L"This is a really really really really really really really really long line that if necessary will be written across multiple lines based on the width of the console buffer. Does not wrap words.");
        console->WriteText(L"gggg", console_color::RED);
        console->WriteText(L"1111", console_color::BRIGHT_RED);
        console->WriteText(L"gggg", console_color::BRIGHT_WHITE);
        std::cout << console_color::WhiteOnRed << "cout stream color test 1\n" << console_color::Default;
        std::cout << console_color::WhiteOnGreen << "cout stream color test 2\n" << console_color::Default;

        /* Main Loop */
        std::wcout << L"Entering the main loop...\n";
        OutputDebugStringW(L"Entering the main loop...\n");

        timeBeginPeriod(1); // sets system timer resolution to 1 ms

        while (ManageWindows(windows))
        {

        }

        timeEndPeriod(1);   // restore system timer resolution

        std::wcout << L"Exiting the main loop...\n";
        OutputDebugStringW(L"Exiting the main loop...\n");
        /* End of Main Loop */

        // unregister the static window class
        Window::UnregisterWindowClass();
    }
    catch (const winxframe::Error& e)
    {
        e.MsgBoxAsync();
        std::wcerr << L"Caught Error (Error class):\n" << e.wwhat() << L'\n';
        std::wcerr << e.LogCauseChain();
        OutputDebugStringW(e.wwhat());
        OutputDebugStringW(e.LogCauseChain().c_str());

        if (!windows.empty())
            windows.clear();

        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        MessageBoxExA(nullptr, e.what(), "Standard Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
        std::wcerr << L"Caught Error (std::exception):\n" << winxframe::string_utils::ToWide(e.what()) << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (const char* msg)
    {
        MessageBoxExA(nullptr, msg, "C-string Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
        std::wcerr << L"Caught Error (C-string exception):\n" << winxframe::string_utils::ToWide(msg) << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (const wchar_t* wideMsg)
    {
        MessageBoxExW(nullptr, wideMsg, L"Wide String Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
        std::wcerr << L"Caught Error (Wide string exception):\n" << wideMsg << L'\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        const char* msg = "Unknown exception.";
        MessageBoxExA(nullptr, msg, "Unknown Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
        std::wcerr << L"Caught Error (unknown exception).\n";
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }

    std::wcout << L"\nProgram exit success. Press any key to continue..." << std::endl;
    _getch();

    console.reset();
    console = nullptr;

    return 0;
}