/*!
lib\source\win32\Window\Window.cpp
Created: October 5, 2025
Updated: November 22, 2025
Copyright (c) 2025, Jacob Gosse

Window source file.
*/

#include <win32/Window/Window.hpp>
#include <win32/Error/error_macros.hpp>
#include <win32/utils/win32_utils.hpp>

namespace winxframe
{
	/* CONSTRUCTORS */

	Window::Window(const std::wstring& windowTitle, LONG screenWidth, LONG screenHeight, MessagePumpMode mode, int nCmdShow)
		try :
		hInstance_(GetModuleHandleW(nullptr)),
		showCmd_(win32_utils::ResolveShowCmd(nCmdShow)),
		pumpMode_(mode),
		windowTitle_(windowTitle),
		screenWidth_(screenWidth),
		screenHeight_(screenHeight)
	{
		std::wcout << L"CONSTRUCTOR: Window(const std::wstring& title, LONG windowWidth, LONG windowHeight, MessagePumpMode mode, int nCmdShow)\n";
		this->InitWindow();
	}
	catch (const Error&)
	{
		if (hWindow_)
		{
			DestroyWindow(hWindow_);
		}
		this->Cleanup();

		RETHROW_ERROR_CTX(L"Rethrowing Window constructor error!");
	}

	Window::Window(HINSTANCE hInstance, const std::wstring& windowTitle, LONG screenWidth, LONG screenHeight, MessagePumpMode mode, int nCmdShow)
		try :
		hInstance_(hInstance),
		showCmd_(win32_utils::ResolveShowCmd(nCmdShow)),
		pumpMode_(mode),
		windowTitle_(windowTitle),
		screenWidth_(screenWidth),
		screenHeight_(screenHeight)
	{
		std::wcout << L"CONSTRUCTOR: Window(HINSTANCE hInstance, const std::wstring& title, LONG windowWidth, LONG windowHeight, MessagePumpMode mode, int nCmdShow)" << L'\n';
		this->InitWindow();
	}
	catch (const Error&)
	{
		if (hWindow_)
		{
			DestroyWindow(hWindow_);
		}
		this->Cleanup();

		RETHROW_ERROR_CTX(L"Rethrowing Window constructor error!");
	}

	/* DESTRUCTOR */

	Window::~Window() noexcept
	{
		std::wcout << L"DESTRUCTOR: ~Window()" << L'\n';
		OutputDebugStringW(L"DESTRUCTOR: ~Window()\n");
		if (hWindow_)
		{
			DestroyWindow(hWindow_);
		}
		this->Cleanup();
	}

	/* STATIC DEFINITIONS */

	WNDCLASSEXW Window::sWindowClass_ = {};
	std::wstring Window::sWindowClassName_ = L"";
	bool Window::sIsClassRegistered_ = false;
	unsigned int Window::sRealTimeWindowCount_ = 0;
	unsigned int Window::sEventDrivenWindowCount_ = 0;

	/* FUNCTION DEFINITIONS */

#pragma region WINDOW CREATION
	void Window::ShowAndUpdateWindow(int nCmdShow) const noexcept
	{
		if (nCmdShow == -1)
			nCmdShow = showCmd_;
			
		ShowWindow(hWindow_, nCmdShow);
		UpdateWindow(hWindow_);
	}

	void Window::InitWindow()
	{
		// system information
		//win32_utils::SysInfo();
		//win32_utils::ProcessorInfo();

		// load accelerator
		hAccelTable_ = LoadAcceleratorsW(hInstance_, MAKEINTRESOURCE(IDR_ACCELERATOR));

		// register window class
		this->RegisterWindowClass();

		// properties for window creation
		LONG windowWidth;
		LONG windowHeight;
		this->GetWindowSize(windowWidth, windowHeight);

		// create window
		hWindow_ = this->BuildWindow(
			this->GetWindowTitle(),
			Window::GetWindowClassName(),
			windowWidth,
			windowHeight,
			WS_OVERLAPPEDWINDOW,	// window styles
			0						// extended window styles
		);
	}

	void Window::SetWindowClassName(const std::wstring& className) const noexcept
	{
		if (!className.empty())
			Window::sWindowClassName_ = className;
		else
		{
			WCHAR titleBuffer[MAX_LOADSTRING]{};
			LoadStringW(hInstance_, IDS_WINDOW_CLASS, titleBuffer, MAX_LOADSTRING);
			Window::sWindowClassName_ = titleBuffer;
		}
	}

	void Window::SetWindowTitle(const std::wstring& title) noexcept
	{
		if (!title.empty())
			windowTitle_ = title;
		else
		{
			WCHAR titleBuffer[MAX_LOADSTRING]{};
			LoadStringW(hInstance_, IDS_WINDOW_TITLE, titleBuffer, MAX_LOADSTRING);
			windowTitle_ = titleBuffer;
		}
		if (hWindow_)
			SetWindowTextW(hWindow_, this->GetWindowTitle().c_str());
	}

	ATOM Window::RegisterWindowClass(int extraClassBytes, int extraWindowBytes) const
	{
		if (sIsClassRegistered_)
			return 1;

		// window class name
		if (Window::sWindowClassName_.empty())
			this->SetWindowClassName();

		// window class properties
		Window::sWindowClass_.cbSize = sizeof(WNDCLASSEX);
		Window::sWindowClass_.lpfnWndProc = Window::WindowProc;
		Window::sWindowClass_.hInstance = hInstance_;
		Window::sWindowClass_.lpszClassName = sWindowClassName_.c_str();
		Window::sWindowClass_.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
		Window::sWindowClass_.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
		Window::sWindowClass_.cbClsExtra = extraClassBytes;
		Window::sWindowClass_.cbWndExtra = extraWindowBytes;
		Window::sWindowClass_.hCursor = LoadCursorW(NULL, IDC_ARROW);
		Window::sWindowClass_.hIcon = LoadIconW(hInstance_, MAKEINTRESOURCE(IDI_PRIMARY));
		Window::sWindowClass_.hIconSm = LoadIconW(hInstance_, MAKEINTRESOURCE(IDI_SMALL));
		Window::sWindowClass_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		//Window::sWindowClass_.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		//Window::sWindowClass_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

		ATOM result = RegisterClassExW(&Window::sWindowClass_);
		THROW_IF_ERROR_CTX(!result && GetLastError() != ERROR_CLASS_ALREADY_EXISTS, L"Failed to register the window class!");
		Window::sIsClassRegistered_ = true;
		return result;
	}

	HWND Window::BuildWindow(const std::wstring& windowTitle, const std::wstring& className, LONG windowWidth, LONG windowHeight, DWORD dwStyle, DWORD dwExStyle)
	{
		HWND hWnd = CreateWindowExW
		(							//HWND CreateWindowExW(
			dwExStyle,				//[in]			 DWORD	   dwExStyle,
			className.c_str(),		//[in, optional] LPCSTR    lpClassName,
			windowTitle.c_str(),	//[in, optional] LPCSTR    lpWindowName,
			dwStyle,				//[in]           DWORD     dwStyle,
			CW_USEDEFAULT,			//[in]           int       X,
			CW_USEDEFAULT,			//[in]           int       Y,
			windowWidth,			//[in]           int       nWidth,
			windowHeight,			//[in]           int       nHeight,
			nullptr,				//[in, optional] HWND      hWndParent,
			nullptr,				//[in, optional] HMENU     hMenu,
			hInstance_,				//[in, optional] HINSTANCE hInstance,
			this					//[in, optional] LPVOID    lpParam
		);

		THROW_IF_ERROR_CTX(!hWnd, L"Failed to create the window!");
		return hWnd;
	}

	void Window::CreateMemoryBitmap()
	{
		THROW_IF_ERROR_CTX(screenWidth_ < 0 || screenHeight_ < 0, L"Window dimensions are invalid!");
		THROW_IF_ERROR_CTX(!hWindow_, L"Window handle is invalid!");
		if (screenWidth_ == 0 || screenHeight_ == 0)
			return;

		this->DestroyMemoryBitmap();

		HDC hdc = GetDC(hWindow_);

		hMemoryDC_ = CreateCompatibleDC(hdc);
		hMemoryBitmap_ = CreateCompatibleBitmap(hdc, screenWidth_, screenHeight_);
		hOldMemoryBitmap_ = (HBITMAP)SelectObject(hMemoryDC_, hMemoryBitmap_);

		ReleaseDC(hWindow_, hdc);
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
			rid.hwndTarget = hWindow_;
			rids.push_back(rid);
		}

		THROW_IF_ERROR_CTX(!RegisterRawInputDevices(rids.data(), static_cast<UINT>(rids.size()), sizeof(RAWINPUTDEVICE)), L"Failed to register raw input devices!");
		this->SetRawInputDevices(std::move(rids));
	}

	void Window::IncrementWindowCount() const noexcept
	{
		if (pumpMode_ == MessagePumpMode::RealTime)
			++Window::sRealTimeWindowCount_;
		if (pumpMode_ == MessagePumpMode::EventDriven)
			++Window::sEventDrivenWindowCount_;
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
		(					//BOOL SetWindowPos(
			hWindow_,		//[in]           HWND hWnd,
			HWND_TOP,		//[in, optional] HWND hWndInsertAfter,
			leftX,			//[in]           int  X,	 // The new position of the left side of the window, in client coordinates
			topY,			//[in]           int  Y,	 // The new position of the top of the window, in client coordinates
			windowWidth,	//[in]           int  cx,	 // The new width of the window, in pixels
			windowHeight,	//[in]           int  cy,	 // The new height of the window, in pixels
			uFlags			//[in]           UINT uFlags // The window sizing and positioning flags
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
		HDC hdc = GetDC(hWindow_);
		BitBlt(hdc, 0, 0, screenWidth_, screenHeight_, hMemoryDC_, 0, 0, SRCCOPY);
		ReleaseDC(hWindow_, hdc);
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
				if (hWindow_)
				{
					DestroyWindow(hWindow_);
				}
				this->Cleanup();
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
				DialogBoxParamW(hInstance_, MAKEINTRESOURCE(IDD_ABOUTBOX), hWindow_, Window::About, 0);
				return 0;
			case IDM_EXIT:
				std::wcout << L"CASE: IDM_EXIT" << L'\n';
				if (MessageBoxW(hWindow_, L"Do you wish to exit?", this->GetWindowTitle().c_str(), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
					DestroyWindow(hWindow_);
				return 0;
			}
			return 0;
		case WM_CLOSE:
			std::wcout << L"CASE: WM_CLOSE" << L'\n';
			if (MessageBoxW(hWindow_, L"Do you wish to exit?", this->GetWindowTitle().c_str(), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
				DestroyWindow(hWindow_);
			return 0;
		case WM_DESTROY:
			std::wcout << L"CASE: WM_DESTROY\n";
			return this->OnDestroy();
		case WM_NCDESTROY:
			std::wcout << L"CASE: WM_NCDESTROY\n";
			SetWindowLongPtrW(hWindow_, GWLP_USERDATA, 0);
			hWindow_ = nullptr;
			return 0;
		default:
			return DefWindowProcW(hWindow_, uMsg, wParam, lParam);
		}
	}

	void Window::PeekMessages(UINT wMsgFilterMin, UINT wMsgFilterMax) const noexcept
	{
		MSG msg = {};
		const HACCEL accelTable = hAccelTable_;
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
		const HACCEL accelTable = hAccelTable_;
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

	LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // static
	{
		Window* pWindow = nullptr;

		if (uMsg == WM_NCCREATE)
		{
			LPCREATESTRUCTW createStruct = reinterpret_cast<LPCREATESTRUCTW>(lParam);
			pWindow = static_cast<Window*>(createStruct->lpCreateParams);
			try
			{
				if (!pWindow)
				{
					THROW_ERROR_CTX(L"WARNING! lpCreateParams is null in WM_NCCREATE!");
				}
				else
				{
					SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
					pWindow->hWindow_ = hWnd;
				}
			}
			catch (const Error& e)
			{
				e.MsgBox();
				std::wcerr << L"Caught Error (Error class):\n" << e.wwhat() << L'\n';
			}
		}
		else
		{
			pWindow = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		}

		if (pWindow)
			return pWindow->HandleMessage(uMsg, wParam, lParam);
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
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

	void Window::Cleanup()
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

	void Window::DecrementWindowCount() const noexcept
	{
		if (pumpMode_ == MessagePumpMode::RealTime)
			--Window::sRealTimeWindowCount_;
		if (pumpMode_ == MessagePumpMode::EventDriven)
			--Window::sEventDrivenWindowCount_;
	}

	LRESULT Window::OnDestroy()
	{
		std::cout << "Calling OnDestroy()\n";

		this->CleanupWindowResources();
		this->DecrementWindowCount();
		if (Window::sEventDrivenWindowCount_ == 0 && Window::sRealTimeWindowCount_ == 0)
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	void Window::UnregisterWindowClass() // static
	{
		if (Window::sRealTimeWindowCount_ == 0 && Window::sEventDrivenWindowCount_ == 0 && Window::sIsClassRegistered_)
		{
			THROW_IF_ERROR_CTX(!UnregisterClassW(Window::sWindowClass_.lpszClassName, Window::sWindowClass_.hInstance), L"Failed to unregister the window class!");
			Window::sWindowClass_ = {};
			Window::sIsClassRegistered_ = false;
		}
	}
#pragma endregion WINDOW CLEANUP
}; // end of namespace winxframe