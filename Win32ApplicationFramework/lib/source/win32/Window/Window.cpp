/*!
lib\source\win32\Window\Window.cpp
Created: October 5, 2025
Updated: November 25, 2025
Copyright (c) 2025, Jacob Gosse

Window source file.
*/

#include <win32/Window/Window.hpp>
#include <win32/utils/win32_utils.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
	/* CONSTRUCTORS */

	Window::Window(WNDCLASSEX& wcex, const std::wstring& windowTitle, LONG screenWidth, LONG screenHeight, MessagePumpMode mode, int nCmdShow)
		try :
		IWindow(mode),
		showCmd_(win32_utils::ResolveShowCmd(nCmdShow)),
		windowTitle_(windowTitle),
		screenWidth_(screenWidth),
		screenHeight_(screenHeight)
	{
		std::wcout << L"CONSTRUCTOR: Window(WNDCLASSEX& wcex)\n";
		this->InitWindow(wcex);
	}
	catch (const Error&)
	{
		if (this->IsWindow())
		{
			DestroyWindow(this->GetWindow());
		}

		RETHROW_ERROR_CTX(L"Rethrowing Window constructor error!");
	}

	Window::Window(HINSTANCE hInstance, WNDCLASSEX& wcex, const std::wstring& windowTitle, LONG screenWidth, LONG screenHeight, MessagePumpMode mode, int nCmdShow)
		try :
		IWindow(hInstance, mode),
		showCmd_(win32_utils::ResolveShowCmd(nCmdShow)),
		windowTitle_(windowTitle),
		screenWidth_(screenWidth),
		screenHeight_(screenHeight)
	{
		std::wcout << L"CONSTRUCTOR: Window(WNDCLASSEX& wcex, HINSTANCE hInstance)" << L'\n';
		this->InitWindow(wcex);
	}
	catch (const Error&)
	{
		if (this->IsWindow())
		{
			DestroyWindow(this->GetWindow());
		}

		RETHROW_ERROR_CTX(L"Rethrowing Window constructor error!");
	}

	/* DESTRUCTOR */

	Window::~Window() noexcept
	{
		std::wcout << L"DESTRUCTOR: ~Window()" << L'\n';
		OutputDebugStringW(L"DESTRUCTOR: ~Window()\n");
		if (this->IsWindow())
		{
			DestroyWindow(this->GetWindow());
		}
	}

	/* FUNCTION DEFINITIONS */

#pragma region WINDOW CREATION
	void Window::ShowAndUpdateWindow(int nCmdShow) const noexcept
	{
		if (nCmdShow == -1)
			nCmdShow = showCmd_;
			
		ShowWindow(this->GetWindow(), nCmdShow);
		UpdateWindow(this->GetWindow());
	}

	void Window::InitWindow(WNDCLASSEX& wcex)
	{
		// system information
		//win32_utils::SysInfo();
		//win32_utils::ProcessorInfo();

		// properties for window creation
		int leftX = 0;
		int topY = 0;
		LONG windowWidth;
		LONG windowHeight;
		this->GetWindowSize(windowWidth, windowHeight);

		Create(
			wcex,
			this->GetWindowTitle(),
			leftX,
			topY,
			windowWidth,
			windowHeight,
			nullptr,
			nullptr,
			WS_OVERLAPPEDWINDOW,
			0
		);
	}

	void Window::SetWindowTitle(const std::wstring& title) noexcept
	{
		if (!title.empty())
			windowTitle_ = title;
		else
		{
			WCHAR titleBuffer[MAX_LOADSTRING]{};
			LoadStringW(this->GetInstance(), IDS_WINDOW_TITLE, titleBuffer, MAX_LOADSTRING);
			windowTitle_ = titleBuffer;
		}
		if (this->IsWindow())
			SetWindowTextW(this->GetWindow(), this->GetWindowTitle().c_str());
	}

	void Window::CreateMemoryBitmap()
	{
		THROW_IF_ERROR_CTX(screenWidth_ < 0 || screenHeight_ < 0, L"Window dimensions are invalid!");
		THROW_IF_ERROR_CTX(!this->IsWindow(), L"Window handle is invalid!");
		if (screenWidth_ == 0 || screenHeight_ == 0)
			return;

		this->DestroyMemoryBitmap();

		HDC hdc = GetDC(this->GetWindow());

		hMemoryDC_ = CreateCompatibleDC(hdc);
		hMemoryBitmap_ = CreateCompatibleBitmap(hdc, screenWidth_, screenHeight_);
		hOldMemoryBitmap_ = (HBITMAP)SelectObject(hMemoryDC_, hMemoryBitmap_);

		ReleaseDC(this->GetWindow(), hdc);
	}

	void Window::RegisterRawInput(std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices, DWORD dwFlags)
	{
		std::vector<RAWINPUTDEVICE> rids;
		rids.reserve(devices.size());

		for (const auto& [usagePage, usageId] : devices)
		{
			RAWINPUTDEVICE rid{};
			rid.usUsagePage = static_cast<USHORT>(usagePage);
			rid.usUsage = static_cast<USHORT>(usageId);
			rid.dwFlags = dwFlags; // if no RIDEV_INPUTSINK, only receives input when focused
			rid.hwndTarget = this->GetWindow();
			rids.push_back(rid);
		}

		THROW_IF_ERROR_CTX(!RegisterRawInputDevices(rids.data(), static_cast<UINT>(rids.size()), sizeof(RAWINPUTDEVICE)), L"Failed to register raw input devices!");
		this->SetRawInputDevices(std::move(rids));
	}

	LRESULT Window::OnCreate()
	{
		std::cout << "Calling OnCreate()\n";

		// set window title
		this->SetWindowTitle(this->GetWindowTitle());

		// create memory bitmap
		this->CreateMemoryBitmap();

		// register window to receive raw input from mouse and keyboard
		this->RegisterRawInput(
			{
				{ HidUsagePage::GenericDesktopControls, HidUsageId::Mouse },
				{ HidUsagePage::GenericDesktopControls, HidUsageId::Keyboard }
			}//, RIDEV_INPUTSINK
		);

		// center window position relative to desktop resolution
		LONG windowWidth;
		LONG windowHeight;
		this->GetWindowSize(windowWidth, windowHeight);

		int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
		int desktopHeight = GetSystemMetrics(SM_CYSCREEN);
		int centerWidth = (desktopWidth - windowWidth) / 2;
		int centerHeight = (desktopHeight - windowHeight) / 2;
		UINT uFlags = SWP_SHOWWINDOW;
		this->RepositionWindow(centerWidth, centerHeight, windowWidth, windowHeight, uFlags);

		// show and update window
		this->ShowAndUpdateWindow();

		// increment window count
		this->IncrementWindowCount();

		return 0;
	}
#pragma endregion WINDOW CREATION

#pragma region WINDOW MODIFICATIONS
	void Window::RepositionWindow(int leftX, int topY, int windowWidth, int windowHeight, UINT uFlags) const noexcept
	{
		SetWindowPos
		(						//BOOL SetWindowPos(
			this->GetWindow(),	//[in]           HWND hWnd,
			HWND_TOP,			//[in, optional] HWND hWndInsertAfter,
			leftX,				//[in]           int  X,	 // The new position of the left side of the window, in client coordinates
			topY,				//[in]           int  Y,	 // The new position of the top of the window, in client coordinates
			windowWidth,		//[in]           int  cx,	 // The new width of the window, in pixels
			windowHeight,		//[in]           int  cy,	 // The new height of the window, in pixels
			uFlags				//[in]           UINT uFlags // The window sizing and positioning flags
		);
	}

	void Window::ClearMemoryBitmapBuffer(COLORREF color) const
	{
		THROW_IF_ERROR_CTX(!hMemoryDC_, L"Handle to the device context for memory bitmap is invalid!");
		THROW_IF_ERROR_CTX(!hMemoryBitmap_, L"Memory Bitmap handle is invalid!");

		RECT rect{ 0, 0, screenWidth_, screenHeight_ };
		HBRUSH brush = CreateSolidBrush(color);
		FillRect(hMemoryDC_, &rect, brush);
		DeleteObject(brush);
	}

	void Window::BeginFrame(COLORREF clearColor)
	{
		this->ClearMemoryBitmapBuffer(clearColor);
	}

	void Window::Present() const
	{
		HDC hdc = GetDC(this->GetWindow());
		BitBlt(hdc, 0, 0, screenWidth_, screenHeight_, hMemoryDC_, 0, 0, SRCCOPY);
		ReleaseDC(this->GetWindow(), hdc);
	}
#pragma endregion WINDOW MODIFICATIONS

#pragma region WINDOW MESSAGE HANDLING
	LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CREATE:
			std::wcout << L"CASE: WM_CREATE" << L'\n';
			return this->OnCreate();
		case WM_KEYUP:
		case WM_KEYDOWN:
		{
			unsigned int vkCode = static_cast<unsigned int>(wParam);
			bool isDown = ((lParam & (1LL << 31)) == 0);
			//bool isDown = (uMsg == WM_KEYDOWN);
			std::wstring key = isDown ? L"Down" : L"Up";
			std::wcout << key << L": " << vkCode << L'\n';
			return 0;
		}
		case WM_CHAR:
			return 0;
		case WM_SIZE:
		{
			std::wcout << L"CASE: WM_SIZE" << L'\n';
			std::wcout << LOWORD(lParam) << L'x' << HIWORD(lParam) << L'\n';
			this->SetScreenSize(LOWORD(lParam), HIWORD(lParam));
			try
			{
				this->CreateMemoryBitmap();
			}
			catch (const Error& e)
			{
				e.MsgBox();
				if (this->IsWindow())
				{
					DestroyWindow(this->GetWindow());
				}
			}
			return 0;
		}
		case WM_SETFOCUS:
			std::wcout << L"CASE: WM_SETFOCUS\n";
			return 0;
		case WM_KILLFOCUS:
			std::wcout << L"CASE: WM_KILLFOCUS\n";
			return 0;
		case WM_COMMAND:
			std::wcout << L"CASE: WM_COMMAND" << L'\n';
			switch (LOWORD(wParam))
			{
			case IDM_ABOUT:
				std::wcout << L"CASE: IDM_ABOUT" << L'\n';
				DialogBoxParamW(this->GetInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), this->GetWindow(), Window::About, 0);
				return 0;
			case IDM_EXIT:
				std::wcout << L"CASE: IDM_EXIT" << L'\n';
				if (MessageBoxW(this->GetWindow(), L"Do you wish to exit?", this->GetWindowTitle().c_str(), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
					DestroyWindow(this->GetWindow());
				return 0;
			}
			return 0;
		case WM_CLOSE:
			std::wcout << L"CASE: WM_CLOSE" << L'\n';
			if (MessageBoxW(this->GetWindow(), L"Do you wish to exit?", this->GetWindowTitle().c_str(), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
				DestroyWindow(this->GetWindow());
			return 0;
		case WM_DESTROY:
			std::wcout << L"CASE: WM_DESTROY\n";
			return this->OnDestroy();
		case WM_NCDESTROY:
			return IWindow::HandleMessage(uMsg, wParam, lParam);
		default:
			return DefWindowProcW(this->GetWindow(), uMsg, wParam, lParam);
		}
	}

	void Window::PeekMessages(UINT wMsgFilterMin, UINT wMsgFilterMax) const noexcept
	{
		MSG msg = {};
		const HACCEL accelTable = this->GetAccelTable();
		while (PeekMessageW(&msg, nullptr, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
		{
			if (!accelTable || !TranslateAcceleratorW(msg.hwnd, accelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	int Window::GetMessages(UINT wMsgFilterMin, UINT wMsgFilterMax) const noexcept
	{
		MSG msg{};
		const HACCEL accelTable = this->GetAccelTable();
		int returnValue = 0;

		while ((returnValue = GetMessageW(&msg, nullptr, wMsgFilterMin, wMsgFilterMax)) > 0)
		{
			if (!accelTable || !TranslateAcceleratorW(msg.hwnd, accelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}

		return (returnValue == 0);
	}
#pragma endregion WINDOW MESSAGE HANDLING

#pragma region WINDOW DIALOGUE HANDLING
	INT_PTR CALLBACK Window::About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (uMsg)
		{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
#pragma endregion WINDOW DIALOGUE HANDLING

#pragma region WINDOW QUERY
	void Window::GetWindowSize(LONG& outWidth, LONG& outHeight, DWORD dwStyle, BOOL hasMenu, DWORD dwExStyle) const noexcept
	{
		RECT rect = { 0, 0, screenWidth_, screenHeight_ };
		AdjustWindowRectEx(&rect, dwStyle, hasMenu, dwExStyle);
		outWidth = rect.right - rect.left;
		outHeight = rect.bottom - rect.top;
	}
#pragma endregion WINDOW QUERY

#pragma region WINDOW CLEANUP
	void Window::DestroyMemoryBitmap()
	{
		if (hMemoryDC_)
		{
			if (hOldMemoryBitmap_)
				SelectObject(hMemoryDC_, hOldMemoryBitmap_);

			hOldMemoryBitmap_ = nullptr;
		}

		if (hMemoryBitmap_)
		{
			DeleteObject(hMemoryBitmap_);
			hMemoryBitmap_ = nullptr;
		}

		if (hMemoryDC_)
		{
			DeleteDC(hMemoryDC_);
			hMemoryDC_ = nullptr;
		}
	}

	void Window::CleanupRawDevices()
	{
		const std::vector<RAWINPUTDEVICE>& devices = this->GetRawInputDevices();

		if (!devices.empty())
		{
			std::vector<RAWINPUTDEVICE> ridsToRemove = devices;

			for (auto& rid : ridsToRemove)
			{
				rid.dwFlags = RIDEV_REMOVE;
				rid.hwndTarget = nullptr;
			}

			try
			{
				THROW_IF_ERROR_CTX(!RegisterRawInputDevices(ridsToRemove.data(), static_cast<UINT>(ridsToRemove.size()), sizeof(RAWINPUTDEVICE)), 
					L"Failed to unregister raw input devices!");
			}
			catch (const Error& e)
			{
				e.MsgBox();
				std::wcerr << L"Caught Error (Error class):\n" << e.wwhat() << L'\n';
			}

			this->SetRawInputDevices(std::vector<RAWINPUTDEVICE>{});
		}
	}

	void Window::CleanupWindowResources()
	{
		if (isWindowCleaned_)
			return;

		std::wcout << L"CLEANUP: Running window cleanup.\n";

		this->DestroyMemoryBitmap();
		this->CleanupRawDevices();

		isWindowCleaned_ = true;
	}

	LRESULT Window::OnDestroy()
	{
		std::cout << "Calling OnDestroy()\n";

		this->CleanupWindowResources();
		this->DecrementWindowCount();
		if (!this->HasEventDrivenWindow() && !this->HasRealTimeWindow())
		{
			PostQuitMessage(0);
		}
		return 0;
	}
#pragma endregion WINDOW CLEANUP
}; // end of namespace winxframe