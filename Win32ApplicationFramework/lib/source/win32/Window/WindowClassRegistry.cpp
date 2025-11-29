/*!
lib\source\win32\Window\WindowClassRegistry.cpp
Created: November 29, 2025
Updated: November 29, 2025
Copyright (c) 2025, Jacob Gosse

Window Class Registry source file.
*/

#include <win32/Window/WindowClassRegistry.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
	/* CONSTRUCTOR */

	WindowClassRegistry::WindowClassRegistry()
	{
		std::wcout << L"CONSTRUCTOR: WindowClassRegistry()\n";
	}

	/* DESTRUCTOR */

	WindowClassRegistry::~WindowClassRegistry() noexcept
	{
		std::wcout << L"DESTRUCTOR: ~WindowClassRegistry()\n";
	}

	/* FUNCTION DEFINITIONS */

    WNDCLASSEX WindowClassRegistry::CreateWindowClass(HINSTANCE hInstance, WNDPROC wndProc, const std::wstring& className, int extraClassBytes, int extraWindowBytes)
    {
        WNDCLASSEX wcex = {};

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
        wcex.lpfnWndProc = wndProc;
        wcex.cbClsExtra = extraClassBytes;
        wcex.cbWndExtra = extraWindowBytes;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRIMARY));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
        wcex.lpszClassName = className.c_str();
        wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

        return wcex;
    }

    ATOM WindowClassRegistry::RegisterWindowClass(WNDCLASSEX& wcex)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        WNDCLASSEX temp{};
        if (GetClassInfoEx(wcex.hInstance, wcex.lpszClassName, &temp) != 0)
            return 1; // return 1 if class already registered

        ATOM atom = RegisterClassEx(&wcex);
        THROW_IF_ERROR_CTX(atom == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS, L"Failed to register the window class!");

        const std::wstring className = wcex.lpszClassName;
        registeredClasses_[className] = atom;
        return atom;
    }

    void WindowClassRegistry::Unregister(HINSTANCE hInstance, const std::wstring& className)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        LOG_IF_ERROR_CTX(!UnregisterClass(className.c_str(), hInstance), L"Failed to unregister the window class!");
        registeredClasses_.erase(className);
    }

    void WindowClassRegistry::UnregisterAll(HINSTANCE hInstance)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& [className, atom] : registeredClasses_)
        {
            LOG_IF_ERROR_CTX(!UnregisterClass(className.c_str(), hInstance), L"Failed to unregister the window class!");
        }
        registeredClasses_.clear();
    }
}; // end of namespace winxframe