/*!
lib\source\win32\Window\Window.cpp
Created: October 5, 2025
Updated: November 18, 2025
Copyright (c) 2025, Jacob Gosse

Window source file.
*/

#include <win32/Window/Window.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
	/* CONSTRUCTORS */

	Window::Window(const std::wstring& windowTitle, LONG windowWidth, LONG windowHeight, MessagePumpMode mode)
		try :
		hWindow_(nullptr),
		hInstance_(GetModuleHandleW(nullptr)),
		hAccelTable_(nullptr),
		startupInfo_({}),
		pumpMode_(mode),
		windowTitle_(windowTitle),
		windowWidth_(windowWidth),
		windowHeight_(windowHeight)
	{
		std::wcout << L"CONSTRUCTOR: Window(const std::wstring& title, LONG windowWidth, LONG windowHeight, MessagePumpMode mode)\n";
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

	Window::Window(HINSTANCE hInstance, const std::wstring& windowTitle, LONG windowWidth, LONG windowHeight, MessagePumpMode mode)
		try :
		hWindow_(nullptr),
		hInstance_(hInstance),
		hAccelTable_(nullptr),
		startupInfo_({}),
		pumpMode_(mode),
		windowTitle_(windowTitle),
		windowWidth_(windowWidth),
		windowHeight_(windowHeight)
	{
		std::wcout << L"CONSTRUCTOR: Window(HINSTANCE hInstance, const std::wstring& title, LONG windowWidth, LONG windowHeight, MessagePumpMode mode)" << L'\n';
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

	void Window::InitWindow()
	{
		// system information
		ZeroMemory(&processInfo_, sizeof(processInfo_));
		ZeroMemory(&systemInfo_, sizeof(systemInfo_));
		//this->SysInfo();
		//this->ProcessorInfo();

		// load accelerator
		hAccelTable_ = LoadAcceleratorsW(hInstance_, MAKEINTRESOURCE(IDR_ACCELERATOR));

		// set window class name
		if (Window::sWindowClassName_.empty())
			this->SetWindowClassName();

		// register window class
		this->RegisterWindowClass();

		// nCmdShow controls how the window is to be shown
		// parameter is ignored the first time an application calls show window if STARTUPINFO structure specified
		this->SetStartupInfo(this->InitStartupInfo());
		int nCmdShow = this->StartupInfo().wShowWindow;

		// properties for window creation
		DWORD dwExStyle = 0;
		LONG leftX = 0;
		LONG topY = 0;
		RECT rect = { leftX, topY, windowWidth_, windowHeight_ };
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, dwExStyle);
		LONG windowWidth = rect.right - rect.left;
		LONG windowHeight = rect.bottom - rect.top;

		// set window title
		this->SetWindowTitle(this->GetWindowTitle());

		// create window
		hWindow_ = this->BuildWindow(
			this->GetWindowTitle(),
			Window::sWindowClassName_,
			windowWidth,
			windowHeight,
			WS_OVERLAPPEDWINDOW,	// window styles
			0						// extended window styles
		);

		// register window to receive raw input from mouse and keyboard
		this->RegisterRawInput(
			{
				{ HidUsagePage::GenericDesktopControls, HidUsageId::Mouse },
				{ HidUsagePage::GenericDesktopControls, HidUsageId::Keyboard }
			}//, RIDEV_INPUTSINK
		);

		// center window position relative to desktop resolution
		int centerWidth = (desktopWidth_ - windowWidth) / 2;
		int centerHeight = (desktopHeight_ - windowHeight) / 2;
		UINT uFlags = SWP_SHOWWINDOW;
		this->RepositionWindow(centerWidth, centerHeight, windowWidth, windowHeight, uFlags);

		// show and update window
		ShowWindow(hWindow_, nCmdShow);
		UpdateWindow(hWindow_);

		// increment window count
		if (pumpMode_ == MessagePumpMode::RealTime)
			++Window::sRealTimeWindowCount_;
		if (pumpMode_ == MessagePumpMode::EventDriven)
			++Window::sEventDrivenWindowCount_;
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

	LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			std::wcout << L"CASE: WM_CREATE" << L'\n';
			if (pumpMode_ == MessagePumpMode::RealTime)
			{
				hwndFPSLabel_ = CreateWindowW(
					L"STATIC", L"FPS:",
					WS_VISIBLE | WS_CHILD,
					10, 10, 40, 20,
					hWindow_, nullptr, nullptr, nullptr);

				hwndFPSValue_ = CreateWindowW(
					L"STATIC", L"0",
					WS_VISIBLE | WS_CHILD,
					60, 10, 40, 20,
					hWindow_, nullptr, nullptr, nullptr);
			}
			return 0;
		}
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
			std::wcout << L"CASE: WM_SIZE" << L'\n';
			std::wcout << LOWORD(lParam) << L'x' << HIWORD(lParam) << L'\n';
			this->SetWindowSize(LOWORD(lParam), HIWORD(lParam));
			return 0;
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
			this->CleanupWindowResources();

			if (pumpMode_ == MessagePumpMode::RealTime)
			{
				--Window::sRealTimeWindowCount_;
				if (Window::sEventDrivenWindowCount_ == 0 && Window::sRealTimeWindowCount_ == 0)
				{
					PostQuitMessage(0);
				}
			}
			if (pumpMode_ == MessagePumpMode::EventDriven)
			{
				--Window::sEventDrivenWindowCount_;
				if (Window::sEventDrivenWindowCount_ == 0 && Window::sRealTimeWindowCount_ == 0)
				{
					PostQuitMessage(0);
				}
			}
			return 0;
		case WM_NCDESTROY:
			std::wcout << L"CASE: WM_NCDESTROY\n";
			SetWindowLongPtrW(hWindow_, GWLP_USERDATA, 0);
			hWindow_ = nullptr;
			return 0;
		default:
			return DefWindowProcW(hWindow_, uMsg, wParam, lParam);
		}
	}

	LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // static
	{
		Window* pWindow = nullptr;

		switch (uMsg)
		{
		case WM_NCCREATE:
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
			break;
		}
		default:
			pWindow = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
			break;
		}

		if (pWindow)
			return pWindow->HandleMessage(uMsg, wParam, lParam);
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	ATOM Window::RegisterWindowClass(int extraClassBytes, int extraWindowBytes) const
	{
		if (sIsClassRegistered_) return 1;

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

	void Window::UnregisterWindowClass() // static
	{
		if (Window::sRealTimeWindowCount_ == 0 && Window::sEventDrivenWindowCount_ == 0 && Window::sIsClassRegistered_)
		{
			THROW_IF_ERROR_CTX(!UnregisterClassW(Window::sWindowClass_.lpszClassName, Window::sWindowClass_.hInstance), L"Failed to unregister the window class!");
			Window::sWindowClass_ = {};
			Window::sIsClassRegistered_ = false;
		}
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

	STARTUPINFO Window::InitStartupInfo() noexcept
	{
		STARTUPINFO startupInfo{};
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_SHOW;
		return startupInfo;
	}

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

	void Window::SysInfo()
	{
		WCHAR buffer1[UNLEN + 1]{};
		DWORD size1 = UNLEN + 1;
		GetUserNameW(buffer1, &size1);
		std::wcout << L"\nUsername: " << buffer1 << L'\n';

		WCHAR buffer2[MAX_COMPUTERNAME_LENGTH + 1]{};
		DWORD size2 = MAX_COMPUTERNAME_LENGTH + 1;
		GetComputerNameW(buffer2, &size2);
		std::wcout << L"Computer Name: " << buffer2 << L'\n';

		/*
		typedef struct _SYSTEM_INFO {
		  union {
			DWORD dwOemId;
			struct {
			  WORD wProcessorArchitecture;
			  WORD wReserved;
			} DUMMYSTRUCTNAME;
		  } DUMMYUNIONNAME;
		  DWORD     dwPageSize;
		  LPVOID    lpMinimumApplicationAddress;
		  LPVOID    lpMaximumApplicationAddress;
		  DWORD_PTR dwActiveProcessorMask;
		  DWORD     dwNumberOfProcessors;
		  DWORD     dwProcessorType;
		  DWORD     dwAllocationGranularity;
		  WORD      wProcessorLevel;
		  WORD      wProcessorRevision;
		} SYSTEM_INFO, *LPSYSTEM_INFO;

		Use the wProcessorArchitecture, wProcessorLevel, and wProcessorRevision members to determine the type of processor.
		*/
		GetNativeSystemInfo(&systemInfo_);

		std::wcout << L"Processor Architecture: ";
		switch (systemInfo_.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_INTEL:
			std::wcout << L"x86\n";
			break;
		case PROCESSOR_ARCHITECTURE_AMD64:
			std::wcout << L"x64 (AMD or Intel)\n";
			break;
		case PROCESSOR_ARCHITECTURE_ARM:
			std::wcout << L"ARM\n";
			break;
		case PROCESSOR_ARCHITECTURE_ARM64:
			std::wcout << L"ARM64\n";
			break;
		case PROCESSOR_ARCHITECTURE_IA64:
			std::wcout << L"Intel Itanium-based\n";
			break;
		default:
			std::wcout << L"Unknown architecture\n";
			break;
		}
		std::wcout << L"Processor Level: " << systemInfo_.wProcessorLevel << L'\n';
		std::wcout << L"Processor Revision: " << systemInfo_.wProcessorRevision << L'\n';
		std::wcout << L"Logical Processors (threads): " << systemInfo_.dwNumberOfProcessors << L'\n';

		WORD revision = systemInfo_.wProcessorRevision;
		BYTE model = HIBYTE(revision);
		BYTE stepping = LOBYTE(revision);

		std::wcout << L"CPU Model: " << model << L'\n';
		std::wcout << L"Stepping: " << stepping << L'\n';

		std::wcout << std::endl;
	}

	void Window::ProcessorInfo()
	{
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
		DWORD size = 0;
		DWORD error = GetLastError();

		THROW_IF_ERROR_CTX(GetLogicalProcessorInformation(nullptr, &size) && error != ERROR_INSUFFICIENT_BUFFER, 
			L"Failed to get required buffer size for logical processor info!");

		/*
		typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION
		{
			ULONG_PTR ProcessorMask;
			LOGICAL_PROCESSOR_RELATIONSHIP Relationship;

			union
			{
				struct
				{
					BYTE Flags;
				} ProcessorCore;

				struct
				{
					DWORD NodeNumber;
				} NumaNode;

				CACHE_DESCRIPTOR Cache;
				ULONGLONG Reserved[2];
			} DUMMYUNIONNAME;
		} SYSTEM_LOGICAL_PROCESSOR_INFORMATION, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;
		*/

		buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(size);
		THROW_IF_ERROR_CTX(!buffer, L"Failed to allocate memory for logical processor info!");

		if (!GetLogicalProcessorInformation(buffer, &size))
		{
			free(buffer);
			THROW_ERROR_CTX(L"Failed to get logical processor information!");
		}

		DWORD count = size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		for (DWORD i = 0; i < count; ++i)
			printf("Processor %u: Relationship = %d\n", i, buffer[i].Relationship);

		free(buffer);
		std::wcout << L'\n';
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

			this->SetRawInputDevices({});
		}
	}

	void Window::CleanupWindowResources()
	{
		if (isWindowCleaned_)
			return;

		std::wcout << L"CLEANUP: Running window cleanup.\n";

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

	void Window::SetFPS(double fps) noexcept
	{
		fps_ = fps;
		std::wstring fpsText = std::to_wstring((int)fps_);
		SetWindowTextW(hwndFPSValue_, fpsText.c_str());
	}
}; // end of namespace winxframe