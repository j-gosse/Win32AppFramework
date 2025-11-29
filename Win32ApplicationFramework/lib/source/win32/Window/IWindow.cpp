/*!
lib\source\win32\Window\IWindow.cpp
Created: November 25, 2025
Updated: November 29, 2025
Copyright (c) 2025, Jacob Gosse

IWindow source file.
*/

#include <win32/Window/IWindow.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
    /* CONSTRUCTORS */

    IWindow::IWindow(MessagePumpMode mode)
        try :
        hInstance_(GetModuleHandle(nullptr)), 
        pumpMode_(mode)
    {
        std::wcout << L"CONSTRUCTOR: IWindow(MessagePumpMode mode)\n";
    }
    catch (const Error&)
    {
        if (hWindow_)
        {
            DestroyWindow(hWindow_);
        }
        this->Cleanup();

        RETHROW_ERROR_CTX(L"Rethrowing IWindow constructor error!");
    }

    IWindow::IWindow(HINSTANCE hInstance, MessagePumpMode mode)
        try :
        hInstance_(hInstance), 
        pumpMode_(mode)
    {
        std::wcout << L"CONSTRUCTOR: IWindow(HINSTANCE hInstance, MessagePumpMode mode)\n";
    }
    catch (const Error&)
    {
        if (hWindow_)
        {
            DestroyWindow(hWindow_);
        }
        this->Cleanup();

        RETHROW_ERROR_CTX(L"Rethrowing IWindow constructor error!");
    }

    /* DESTRUCTOR */

    IWindow::~IWindow() noexcept
    {
        std::wcout << L"DESTRUCTOR: ~IWindow()\n";
        OutputDebugString(L"DESTRUCTOR: ~IWindow()\n");
        if (hWindow_)
        {
            DestroyWindow(hWindow_);
        }
        this->Cleanup();
    }

    /* STATIC DEFINITIONS */

    unsigned int IWindow::sRealTimeWindowCount_ = 0;
    unsigned int IWindow::sEventDrivenWindowCount_ = 0;

    /* FUNCTION DEFINITIONS */

    LRESULT CALLBACK IWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        IWindow* pInstance = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCTW createStruct = reinterpret_cast<LPCREATESTRUCTW>(lParam);
            pInstance = static_cast<IWindow*>(createStruct->lpCreateParams);
            if (!pInstance)
                return FALSE;
            else
            {
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pInstance));
                pInstance->hWindow_ = hWnd;
            }
        }
        else
            pInstance = reinterpret_cast<IWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (pInstance)
            return pInstance->HandleMessage(uMsg, wParam, lParam);
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    bool IWindow::Create(WindowClassRegistry& windowClassRegistry, std::wstring& windowClassName, const std::wstring& windowTitle, int leftX, int topY, LONG windowWidth, LONG windowHeight, HWND parent, HMENU menu, DWORD dwStyle, DWORD dwExStyle)
    {
        hAccelTable_ = LoadAccelerators(hInstance_, MAKEINTRESOURCE(IDR_ACCELERATOR));

        // set default class name if class name string empty
        if (windowClassName.empty())
        {
            WCHAR classNameBuffer[MAX_LOADSTRING]{};
            LoadString(hInstance_, IDS_WINDOW_CLASS, classNameBuffer, MAX_LOADSTRING);
            windowClassName = classNameBuffer;
        }

        WNDCLASSEX wcex = windowClassRegistry.CreateWindowClass(hInstance_, IWindow::WndProc, windowClassName);
        windowClassRegistry.RegisterWindowClass(wcex);

        hWindow_ = CreateWindowEx(
            dwExStyle,
            windowClassName.c_str(),
            windowTitle.c_str(),
            dwStyle,
            leftX, topY, windowWidth, windowHeight,
            parent,
            menu,
            hInstance_,
            this
        );

        return this->IsWindow();
    }

    LRESULT IWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_NCDESTROY:
            std::wcout << L"CASE: WM_NCDESTROY\n";
            SetWindowLongPtr(hWindow_, GWLP_USERDATA, 0);
            hWindow_ = nullptr;
            return 0;
        default:
            return DefWindowProc(hWindow_, uMsg, wParam, lParam);
        }
    }

    void IWindow::Cleanup()
    {
        if (isCleaned_)
            return;

        std::wcout << L"CLEANUP: Running general cleanup.\n";

        if (hAccelTable_)
        {
            DestroyAcceleratorTable(hAccelTable_);
            hAccelTable_ = nullptr;
        }

        isCleaned_ = true;
    }
}; // end of namespace winxframe