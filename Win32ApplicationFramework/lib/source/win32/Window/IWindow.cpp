/*!
lib\source\win32\Window\IWindow.cpp
Created: November 25, 2025
Updated: November 25, 2025
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
        hInstance_(GetModuleHandleW(nullptr)), 
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

    IWindow::~IWindow()
    {
        std::wcout << L"DESTRUCTOR: ~IWindow()\n";
        OutputDebugStringW(L"DESTRUCTOR: ~IWindow()\n");
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
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pInstance));
                pInstance->hWindow_ = hWnd;
            }
        }
        else
            pInstance = reinterpret_cast<IWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        if (pInstance)
            return pInstance->HandleMessage(uMsg, wParam, lParam);
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    ATOM IWindow::RegisterWindowClass(WNDCLASSEX& wcex) const
    {
        // return 1 if class already registered
        WNDCLASSEX temp{};
        if (GetClassInfoExW(hInstance_, wcex.lpszClassName, &temp) != 0)
            return 1;

        if (!wcex.lpfnWndProc)
            wcex.lpfnWndProc = IWindow::WndProc;

        ATOM result = RegisterClassExW(&wcex);
        THROW_IF_ERROR_CTX(!result && GetLastError() != ERROR_CLASS_ALREADY_EXISTS, L"Failed to register the window class!");
        return result;
    }

    bool IWindow::Create(WNDCLASSEX& wcex, const std::wstring& title, int leftX, int topY, LONG windowWidth, LONG windowHeight, HWND parent, HMENU menu, DWORD dwStyle, DWORD dwExStyle)
    {
        hAccelTable_ = LoadAcceleratorsW(hInstance_, MAKEINTRESOURCE(IDR_ACCELERATOR));

        if (!wcex.lpszClassName || wcex.lpszClassName[0] == L'\0')
        {
            WCHAR titleBuffer[MAX_LOADSTRING]{};
            LoadStringW(hInstance_, IDS_WINDOW_CLASS, titleBuffer, MAX_LOADSTRING);
            wcex.lpszClassName = titleBuffer;
        }
        this->RegisterWindowClass(wcex);

        hWindow_ = CreateWindowExW(
            dwExStyle,
            wcex.lpszClassName,
            title.c_str(),
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
            SetWindowLongPtrW(hWindow_, GWLP_USERDATA, 0);
            hWindow_ = nullptr;
            return 0;
        default:
            return DefWindowProcW(hWindow_, uMsg, wParam, lParam);
        }
    }

    void IWindow::IncrementWindowCount() const noexcept
    {
        if (pumpMode_ == MessagePumpMode::RealTime)
            ++IWindow::sRealTimeWindowCount_;
        if (pumpMode_ == MessagePumpMode::EventDriven)
            ++IWindow::sEventDrivenWindowCount_;
    }

    void IWindow::DecrementWindowCount() const noexcept
    {
        if (pumpMode_ == MessagePumpMode::RealTime)
            --IWindow::sRealTimeWindowCount_;
        if (pumpMode_ == MessagePumpMode::EventDriven)
            --IWindow::sEventDrivenWindowCount_;
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