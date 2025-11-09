/*!
lib\source\win32\Window\Window.cpp
Created: October 5, 2025
Updated: November 8, 2025
Copyright (c) 2025, Jacob Gosse

Window source file.
*/

#include <win32/Window/Window.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
	/* CONSTRUCTORS */

	Window::Window() :
		hWindow_(nullptr),
		windowClass_({}),
		hInstance_(GetModuleHandleW(nullptr)),
		hAccelTable_(nullptr),
		startupInfo_({}),
		windowTitle_(L""),
		windowClassName_(L"")
	{
		std::wcout << L"CONSTRUCTOR: Window()\n";
		this->InitWindow();
	}

	Window::Window(HINSTANCE hInstance) :
		hWindow_(nullptr),
		windowClass_({}),
		hInstance_(hInstance),
		hAccelTable_(nullptr),
		startupInfo_({}),
		windowTitle_(L""),
		windowClassName_(L"")
	{
		std::wcout << L"CONSTRUCTOR: Window(HINSTANCE hInstance)" << L'\n';
		this->InitWindow();
	}

	/* DESTRUCTOR */

	Window::~Window()
	{
		std::wcout << L"DESTRUCTOR: ~Window()" << L'\n';
		OutputDebugStringW(L"DESTRUCTOR: ~Window()\n");
		this->Cleanup();
	}

	/* FUNCTION DEFINITIONS */

	BOOL Window::ProcessMessages(UINT wMsgFilterMin, UINT wMsgFilterMax)
	{
		MSG msg = {};
		const HACCEL accel = this->GetAccelTable();
		while (PeekMessageW(&msg, nullptr, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return FALSE;

			if (!accel || !TranslateAcceleratorW(msg.hwnd, accel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
		return TRUE;
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

	void Window::RepositionWindow(int leftX, int topY, int windowWidth, int windowHeight) const noexcept
	{
		SetWindowPos
		(						//BOOL SetWindowPos(
			this->GetWindow(),	//[in]           HWND hWnd,
			HWND_TOP,			//[in, optional] HWND hWndInsertAfter,
			leftX,				//[in]           int  X,	 // The new position of the left side of the window, in client coordinates
			topY,				//[in]           int  Y,	 // The new position of the top of the window, in client coordinates
			windowWidth,		//[in]           int  cx,	 // The new width of the window, in pixels
			windowHeight,		//[in]           int  cy,	 // The new height of the window, in pixels
			SWP_SHOWWINDOW		//[in]           UINT uFlags // The window sizing and positioning flags
		);
	}

	LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
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
			this->SetScreenWidth(LOWORD(lParam));
			this->SetScreenHeight(HIWORD(lParam));
			return 0;
		case WM_CLOSE:
			std::wcout << L"CASE: WM_CLOSE" << L'\n';
			if (MessageBoxW(this->GetWindow(), L"Do you wish to exit?", this->GetWindowTitle().c_str(), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
				this->Cleanup();
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
					this->Cleanup();
				return 0;
			}
			return 0;
		case WM_SETFOCUS:
			std::wcout << L"CASE: WM_SETFOCUS" << L'\n';
			return 0;
		case WM_KILLFOCUS:
			std::wcout << L"CASE: WM_KILLFOCUS" << L'\n';
			return 0;
		case WM_DESTROY:
			std::wcout << L"CASE: WM_DESTROY" << L'\n';
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProcW(this->GetWindow(), uMsg, wParam, lParam);
		}
	}

	LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* pWindow = nullptr;

		switch (uMsg)
		{
		case WM_NCCREATE:
		{
			LPCREATESTRUCTW createStruct = reinterpret_cast<LPCREATESTRUCTW>(lParam);
			pWindow = static_cast<Window*>(createStruct->lpCreateParams);
			LOG_IF_ERROR_CTX(!pWindow, L"WARNING: lpCreateParams is null in WM_NCCREATE!");
			if (pWindow)
			{
				SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
				pWindow->SetWindow(hWnd);
			}
			break;
		}
		default:
			pWindow = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
			break;
		}

		if (pWindow) return pWindow->HandleMessage(uMsg, wParam, lParam);
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	ATOM Window::RegisterWindowClass(int extraClassBytes, int extraWindowBytes)
	{
		WNDCLASSEX wndClassEx{};
		wndClassEx.cbSize = sizeof(WNDCLASSEX);
		wndClassEx.lpfnWndProc = Window::WindowProc;
		wndClassEx.hInstance = this->GetInstance();
		wndClassEx.lpszClassName = this->GetWindowClassName().c_str();
		wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
		wndClassEx.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
		wndClassEx.cbClsExtra = extraClassBytes;
		wndClassEx.cbWndExtra = extraWindowBytes;
		wndClassEx.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wndClassEx.hIcon = LoadIconW(this->GetInstance(), MAKEINTRESOURCE(IDI_PRIMARY));
		wndClassEx.hIconSm = LoadIconW(this->GetInstance(), MAKEINTRESOURCE(IDI_SMALL));
		wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		//wndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		//wndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

		this->SetWindowClass(wndClassEx);
		ATOM result = RegisterClassExW(&this->GetWindowClass());
		THROW_IF_ERROR_CTX(!result, L"Failed to register the window class!");
		return result;
	}

	HWND Window::BuildWindow(int windowWidth, int windowHeight, DWORD dwExStyle)
	{
		HWND hWnd = CreateWindowExW
		(										//HWND CreateWindowExW(
			dwExStyle,							//[in]			 DWORD	   dwExStyle,
			this->GetWindowClassName().c_str(),	//[in, optional] LPCSTR    lpClassName,
			this->GetWindowTitle().c_str(),		//[in, optional] LPCSTR    lpWindowName,
			WS_OVERLAPPEDWINDOW,				//[in]           DWORD     dwStyle,
			CW_USEDEFAULT,						//[in]           int       X,
			CW_USEDEFAULT,						//[in]           int       Y,
			windowWidth,						//[in]           int       nWidth,
			windowHeight,						//[in]           int       nHeight,
			nullptr,							//[in, optional] HWND      hWndParent,
			nullptr,							//[in, optional] HMENU     hMenu,
			this->GetInstance(),				//[in, optional] HINSTANCE hInstance,
			this								//[in, optional] LPVOID    lpParam
		);

		THROW_IF_ERROR_CTX(hWnd == nullptr, L"Failed to create the window!");
		return hWnd;
	}

	STARTUPINFO Window::InitStartupInfo() noexcept
	{
		STARTUPINFO startupInfo{};
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_SHOW;
		return startupInfo;
	}

	void Window::InitWindow()
	{
		// system information
		ZeroMemory(&processInfo_, sizeof(processInfo_));
		ZeroMemory(&systemInfo_, sizeof(systemInfo_));
		this->SysInfo();
		//this->ProcessorInfo();

		// load accelerator
		this->SetAccelTable(LoadAcceleratorsW(this->GetInstance(), MAKEINTRESOURCE(IDR_ACCELERATOR)));

		// register window class
		this->SetWindowClassName();
		this->RegisterWindowClass();

		// nCmdShow controls how the window is to be shown
		// parameter is ignored the first time an application calls show window if STARTUPINFO structure specified
		this->SetStartupInfo(this->InitStartupInfo());
		int nCmdShow = this->StartupInfo().wShowWindow;

		// properties for window creation
		DWORD dwExStyle = 0;
		LONG leftX = 0;
		LONG topY = 0;
		RECT rect = { leftX, topY, Window::WINDOW_WIDTH, Window::WINDOW_HEIGHT };
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, dwExStyle);
		int windowWidth = rect.right - rect.left;
		int windowHeight = rect.bottom - rect.top;

		// create window
		this->SetWindowTitle(L"WINDOW");
		this->SetWindow(this->BuildWindow(windowWidth, windowHeight));

		// register window to receive raw input from mouse and keyboard
		this->RegisterRawInput(
			{
				{ HidUsagePage::GenericDesktopControls, HidUsageId::Mouse },
				{ HidUsagePage::GenericDesktopControls, HidUsageId::Keyboard }
			}//, RIDEV_INPUTSINK
		);

		// center window position relative to desktop resolution
		int centerWidth = (this->GetDesktopWidth() - windowWidth) / 2;
		int centerHeight = (this->GetDesktopHeight() - windowHeight) / 2;
		this->RepositionWindow(centerWidth, centerHeight, windowWidth, windowHeight);

		// show and update window
		ShowWindow(this->GetWindow(), nCmdShow);
		UpdateWindow(this->GetWindow());
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

	void Window::SysInfo() noexcept
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
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		DWORD size = 0;

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
		GetLogicalProcessorInformation(NULL, &size); // Get the required buffer size
		buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(size);
		THROW_IF_ERROR_CTX(buffer == NULL, L"Failed to allocate memory!");

		if (!GetLogicalProcessorInformation(buffer, &size))
		{
			free(buffer);
			THROW_ERROR_CTX(L"Failed to call GetLogicalProcessorInformation!");
		}

		DWORD count = size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		for (DWORD i = 0; i < count; ++i)
			printf("Processor %u: Relationship = %d\n", i, buffer[i].Relationship);

		free(buffer);
		std::wcout << L'\n';
	}

	void Window::CleanRawDevices()
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

			if (!RegisterRawInputDevices(ridsToRemove.data(), static_cast<UINT>(ridsToRemove.size()), sizeof(RAWINPUTDEVICE)))
			{
				THROW_ERROR_CTX(L"Failed to unregister raw input devices!");
			}

			this->SetRawInputDevices({});
		}
	}

	void Window::Cleanup()
	{
		if (this->IsCleaned()) return;

		this->CleanRawDevices();

		if (this->GetAccelTable())
		{
			DestroyAcceleratorTable(this->GetAccelTable());
			this->SetAccelTable(nullptr);
		}

		if (this->GetWindow())
		{
			DestroyWindow(this->GetWindow());
			this->SetWindow(nullptr);
		}

		THROW_IF_ERROR_CTX(!UnregisterClassW(this->GetWindowClassName().c_str(), this->GetInstance()), L"Failed to unregister the Window class!");

		this->SetIsCleaned(true);
	}

	void Window::SetWindowClassName(const std::wstring& className) noexcept
	{
		if (!className.empty()) windowClassName_ = className;
		else
		{
			WCHAR titleBuffer[MAX_LOADSTRING]{};
			LoadStringW(this->GetInstance(), IDS_WINDOW_CLASS, titleBuffer, MAX_LOADSTRING);
			windowClassName_ = titleBuffer;
		}
	}

	void Window::SetWindowTitle(const std::wstring& title) noexcept
	{
		if (!title.empty()) windowTitle_ = title;
		else
		{
			WCHAR titleBuffer[MAX_LOADSTRING]{};
			LoadStringW(this->GetInstance(), IDS_WINDOW_TITLE, titleBuffer, MAX_LOADSTRING);
			windowTitle_ = titleBuffer;
		}
	}
}; // end of namespace winxframe