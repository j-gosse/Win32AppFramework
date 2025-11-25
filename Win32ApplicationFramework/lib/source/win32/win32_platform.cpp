/*!
lib\source\win32\win32_platform.cpp
Created: October 5, 2025
Updated: November 25, 2025
Copyright (c) 2025, Jacob Gosse

Win32 Platform source file.

\note
Win32 Platform is the main entry point for the application.
*/

#include <win32/Console/Console.hpp>
#include <win32/Window/Window.hpp>
#include <win32/Window/window_manager.hpp>
#include <win32/Error/Error.hpp>
#include <TestSuite/TestSuite.hpp>
#include <win32/utils/win32_utils.hpp>

using namespace winxframe;

// derived window class test
class TestWindow : public Window
{
public:
    TestWindow(
        WNDCLASSEX& wcex, 
        const std::wstring& title = L"", 
        LONG windowWidth = DEFAULT_SCREEN_WIDTH, 
        LONG windowHeight = DEFAULT_SCREEN_HEIGHT, 
        MessagePumpMode mode = MessagePumpMode::RealTime, 
        int nCmdShow = SW_SHOWDEFAULT
    )
        : Window(wcex, title, windowWidth, windowHeight, mode, nCmdShow)
    {
        std::wcout << L"CONSTRUCTOR: TestWindow(WNDCLASSEX& wcex)\n";
    }

    TestWindow(
        HINSTANCE hInstance, 
        WNDCLASSEX& wcex, 
        const std::wstring& title = L"", 
        LONG windowWidth = DEFAULT_SCREEN_WIDTH, 
        LONG windowHeight = DEFAULT_SCREEN_HEIGHT, 
        MessagePumpMode mode = MessagePumpMode::RealTime, 
        int nCmdShow = SW_SHOWDEFAULT
    )
        : Window(hInstance, wcex, title, windowWidth, windowHeight, mode, nCmdShow)
    {
        std::wcout << L"CONSTRUCTOR: TestWindow(WNDCLASSEX& wcex, HINSTANCE hInstance)" << L'\n';
    }

    ~TestWindow() noexcept override
    {
        std::wcout << L"DESTRUCTOR: ~TestWindow()\n";
    }
    
    LRESULT OnCreate() override { return Window::OnCreate(); } // parent class OnCreate() should run BEFORE derived class logic

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

    LRESULT OnDestroy() override { return Window::OnDestroy(); } // parent class OnDestroy() should run AFTER derived class logic
};

static WNDCLASSEX CreateWindowClass(HINSTANCE hInstance, const std::wstring& className, int extraClassBytes, int extraWindowBytes)
{
    WNDCLASSEX wcex = {};

    // window class properties
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = nullptr;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = className.c_str();
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
    wcex.cbClsExtra = extraClassBytes;
    wcex.cbWndExtra = extraWindowBytes;
    wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wcex.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_PRIMARY));
    wcex.hIconSm = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    //wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    return wcex;
}

static void UnregisterWindowClass(const WNDCLASSEX& wcex)
{
    THROW_IF_ERROR_CTX(!UnregisterClassW(wcex.lpszClassName, wcex.hInstance), L"Failed to unregister the window class!");
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

        // run unit tests
        winxframe::TestRegistry::RunAll();

        // create WNDCLASS object
        WNDCLASSEX mainWindowClass = CreateWindowClass(hInstance, L"WindowBaseClass", 0, 0);

        // init window(s)
        LONG windowWidth = 800;
        LONG windowHeight = 600;
        windows.push_back(std::make_unique<TestWindow>(hInstance, mainWindowClass, L"WINDOW1", windowWidth, windowHeight, MessagePumpMode::RealTime, nCmdShow));
        //windows.push_back(std::make_unique<TestWindow>(hInstance, mainWindowClass, L"WINDOW2", 600, 400, MessagePumpMode::RealTime, nCmdShow));
        //windows.push_back(std::make_unique<TestWindow>(hInstance, mainWindowClass, L"WINDOW3", 400, 200, MessagePumpMode::EventDriven, nCmdShow));

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

        // unregister window class
        UnregisterWindowClass(mainWindowClass);
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
        std::cerr << "Caught Error (std::exception):\n" << e.what() << '\n';
        std::wcout << L"Program exit failure. Press any key to continue..." << std::endl;
        _getch();
        console.reset();
        console = nullptr;
        return EXIT_FAILURE;
    }
    catch (const char* msg)
    {
        MessageBoxExA(nullptr, msg, "C-string Exception!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
        std::cerr << "Caught Error (C-string exception):\n" << msg << '\n';
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