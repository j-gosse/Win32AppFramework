/*!
lib\source\win32\win32_platform.cpp
Created: October 5, 2025
Updated: November 15, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Platform source file.

\note
Win32 Platform is the main (wWinMain) entry point for the application.
*/

#include <win32/Console/Console.hpp>
#include <win32/Window/Window.hpp>
#include <win32/Error/error_macros.hpp>
#include <TestSuite/TestSuite.hpp>
#include <utils/string_utils.hpp>

using namespace winxframe;

class MyWindow : public Window
{
public:
    MyWindow(const std::wstring& title = L"", LONG windowWidth = WINDOW_WIDTH, LONG windowHeight = WINDOW_HEIGHT)
        : Window(title, windowWidth, windowHeight) {}

    MyWindow(HINSTANCE hInstance, const std::wstring& title = L"", LONG windowWidth = WINDOW_WIDTH, LONG windowHeight = WINDOW_HEIGHT)
        : Window(hInstance, title, windowWidth, windowHeight) {}

    ~MyWindow() override
    {
        std::wcout << L"DESTRUCTOR: ~MyWindow()\n";
    }

    void Update(std::chrono::nanoseconds deltaTime) override
    {
        // update logic
        //std::wcout << L"Updating window: " << deltaTime.count() << L" ns\n";
    }

    void Render() override
    {
        // render logic
        /*
            HDC dc = GetDC(this->GetWindow());
            // draw stuff
            ReleaseDC(this->GetWindow(), dc);
        */
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

    try {
        // init console
        console = std::make_unique<winxframe::Console>(hInstance, L"CONSOLE", consoleSize.X, consoleSize.Y);

        // init window
        LONG windowWidth = 800;
        LONG windowHeight = 600;
        windows.push_back(std::make_unique<MyWindow>(hInstance, L"WINDOW1", windowWidth, windowHeight));
        windows.push_back(std::make_unique<MyWindow>(hInstance, L"WINDOW2", 400, 400));
        //MyWindow* window1 = new MyWindow(hInstance, L"WINDOW1", windowWidth, windowHeight);
        //MyWindow* window2 = new MyWindow(hInstance, L"WINDOW2", 400, 400);

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
        std::cout << console_color::WhiteOnGreen << "cout stream color test 2\n" << console_color::Default;

        std::wcout << L"Entering the main loop...\n";
        OutputDebugStringW(L"Entering the main loop...\n");

        /* Main Message Loop */
        std::chrono::high_resolution_clock::time_point startTime, previousTime, currentTime;
        std::chrono::nanoseconds elapsedTime{}, deltaTime{}, frameDuration{}, sleepTime{};
        const std::chrono::nanoseconds targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS

        previousTime = std::chrono::high_resolution_clock::now();
        startTime = previousTime;

        while (!windows.empty())
        {
            for (auto& w : windows)
            {
                w->ProcessMessages();
            }

            for (auto it = windows.begin(); it != windows.end(); )
            {
                if (!(*it)->GetWindow())
                {
                    it = windows.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            // track elapsed time and delta time
            currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = currentTime - previousTime;  // time since last frame
            elapsedTime = currentTime - startTime;
            previousTime = currentTime;

            // update/render loop for windows
            for (auto& w : windows)
            {
                w->SetElapsed(elapsedTime);
                //std::wcout << w->GetElapsed().count() << L'\n';

                /*
                // This is where the main loop logic or call to a update/render loop occurs. For example:
                    w->Update(deltaTime);
                    w->Render();
                */
            }

            // limit framerate
            frameDuration = std::chrono::high_resolution_clock::now() - currentTime;
            if (frameDuration < targetFrameTime)
            {
                sleepTime = targetFrameTime > frameDuration ? targetFrameTime - frameDuration : std::chrono::nanoseconds(0);
                std::this_thread::sleep_for(sleepTime);
                //std::cout << "Sleep Time: " << sleepTime << '\n';
            }
        }

        // unregister the static window class
        Window::UnregisterWindowClass();

        std::wcout << L"Exiting the main loop...\n";
        OutputDebugStringW(L"Exiting the main loop...\n");
    }
    catch (const winxframe::Error& e)
    {
        e.MsgBoxAsync();
        std::wcerr << L"Caught Error (Error class):\n" << e.wwhat() << L'\n';
        std::wcerr << e.LogCauseChain();
        OutputDebugStringW(e.wwhat());
        OutputDebugStringW(e.LogCauseChain().c_str());
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