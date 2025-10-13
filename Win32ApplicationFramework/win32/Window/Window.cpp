/*!
win32\Window\Window.cpp
Created: October 5, 2025
Updated: October 13, 2025
Copyright (c) 2025, Jacob Gosse

Window source file.
*/

#include "Window.hpp"

/* CONSTRUCTORS */

Window::Window() :
	hWindow_(nullptr),
	windowClass_({}),
	hInstance_(GetModuleHandleW(nullptr)),
	hAccelTable_(nullptr),
	startupInfo_({}),
	windowName_(L""),
	windowClassName_(L"")
{
	std::wcout << L"CONSTRUCTOR: Window()\n";
	Window::InitWindow();
}

Window::Window(HINSTANCE hInstance) :
	hWindow_(nullptr),
	windowClass_({}),
	hInstance_(hInstance),
	hAccelTable_(nullptr),
	startupInfo_({}),
	windowName_(L""),
	windowClassName_(L"")
{
	std::wcout << L"CONSTRUCTOR: Window(HINSTANCE hInstance)" << L'\n';
	Window::InitWindow();
}

/* DESTRUCTOR */

Window::~Window()
{
	std::wcout << L"DESTRUCTOR: ~Window()" << L'\n';
	OutputDebugStringW(L"DESTRUCTOR: ~Window()\n");
	Window::Cleanup();
}

/* FUNCTION DEFINITIONS */

BOOL Window::ProcessMessages(UINT wMsgFilterMin, UINT wMsgFilterMax) const
{
	MSG msg = {};
	while (PeekMessageW(&msg, nullptr, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
	{
		if (msg.message == WM_QUIT) return FALSE;

		if (hAccelTable_ && !TranslateAcceleratorW(msg.hwnd, hAccelTable_, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return TRUE;
}
/*
BOOL Window::ProcessMessages(UINT wMsgFilterMin, UINT wMsgFilterMax) const
{
	MSG msg = {};
	while (GetMessageW(&msg, nullptr, wMsgFilterMin, wMsgFilterMax))
	{
		if (hAccelTable_ && !TranslateAcceleratorW(msg.hwnd, hAccelTable_, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return (int)msg.wParam;
}
*/

LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYUP:
	case WM_KEYDOWN:
	{
		unsigned int vkCode = static_cast<unsigned int>(wParam);
		bool isDown = ((lParam & (1LL << 31)) == 0);
		std::wstring key = isDown ? L"Down" : L"Up";
		std::wcout << key << L": " << vkCode << L'\n';
		//bool isDown = (uMsg == WM_KEYDOWN);
		return 0;
	}
	case WM_CHAR:
		return 0;
	case WM_SIZE:
		std::wcout << L"CASE: WM_SIZE" << L'\n';
		std::wcout << LOWORD(lParam) << L'x' << HIWORD(lParam) << L'\n';
		screenWidth_ = LOWORD(lParam);
		screenHeight_ = HIWORD(lParam);
		return 0;
	case WM_CLOSE:
		std::wcout << L"CASE: WM_CLOSE" << L'\n';
		if (MessageBoxW(hWindow_, L"Do you wish to exit?", L"Windows App", MB_OKCANCEL) == IDOK)
		{
			Window::Cleanup();
		}
		return 0;
	case WM_COMMAND:
		std::wcout << L"CASE: WM_COMMAND" << L'\n';
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			std::wcout << L"CASE: IDM_ABOUT" << L'\n';
			DialogBox(hInstance_, MAKEINTRESOURCE(IDD_ABOUTBOX), hWindow_, About);
			return 0;
		case IDM_EXIT:
			std::wcout << L"CASE: IDM_EXIT" << L'\n';
			if (MessageBoxW(hWindow_, L"Do you wish to exit?", L"Windows App", MB_OKCANCEL) == IDOK)
			{
				Window::Cleanup();
			}
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
		return DefWindowProcW(hWindow_, uMsg, wParam, lParam);
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
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		pWindow->hWindow_ = hWnd;
		break;
	}
	default:
		pWindow = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		break;
	}

	if (pWindow) return pWindow->Window::HandleMessage(uMsg, wParam, lParam);
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

ATOM Window::RegisterWindowClass()
{
	int extraClassBytes = 0;
	int extraWindowBytes = 0;
	windowClass_.cbSize = sizeof(WNDCLASSEX);
	windowClass_.lpfnWndProc = Window::WindowProc;
	windowClass_.hInstance = hInstance_;
	windowClass_.lpszClassName = windowClassName_;
	windowClass_.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	windowClass_.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
	windowClass_.cbClsExtra = extraClassBytes;
	windowClass_.cbWndExtra = extraWindowBytes;
	windowClass_.hCursor = LoadCursorW(NULL, IDC_ARROW);
	windowClass_.hIcon = LoadIconW(hInstance_, MAKEINTRESOURCE(IDI_PRIMARY));
	windowClass_.hIconSm = LoadIcon(windowClass_.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	windowClass_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//windowClass_.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//windowClass_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClassExW(&windowClass_))
	{
		throw std::runtime_error("Failed to register the window class!");
	}

	return RegisterClassExW(&windowClass_);
}

HWND Window::BuildWindow(HINSTANCE hInstance, int windowWidth, int windowHeight, DWORD dwExStyle)
{
	HWND hWnd = CreateWindowExW
	(							//HWND CreateWindowExW(
		dwExStyle,				//[in]			 DWORD	   dwExStyle,
		windowClassName_,		//[in, optional] LPCSTR    lpClassName,
		windowName_,			//[in, optional] LPCSTR    lpWindowName,
		WS_OVERLAPPEDWINDOW,	//[in]           DWORD     dwStyle,
		CW_USEDEFAULT,			//[in]           int       X,
		CW_USEDEFAULT,			//[in]           int       Y,
		windowWidth,			//[in]           int       nWidth,
		windowHeight,			//[in]           int       nHeight,
		nullptr,				//[in, optional] HWND      hWndParent,
		nullptr,				//[in, optional] HMENU     hMenu,
		hInstance,				//[in, optional] HINSTANCE hInstance,
		this					//[in, optional] LPVOID    lpParam
	);

	if (hWnd == nullptr)
	{
		throw std::runtime_error("Failed to create the window!");
	}

	return hWnd;
}

void Window::RegisterRawInput(HWND hWnd)
{
	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;		// Generic desktop controls
	rid.usUsage = 0x06;			// Keyboard
	rid.dwFlags = 0;			// No RIDEV_INPUTSINK, so only receive input when focused
	rid.hwndTarget = hWnd;

	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
	{
		throw std::runtime_error("Failed to register raw input device!");
	}
}

void Window::RepositionWindow(HWND hWnd, int leftX, int topY, int windowWidth, int windowHeight)
{
	SetWindowPos
	(							//BOOL SetWindowPos(
		hWnd,					//[in]           HWND hWnd,
		HWND_TOP,				//[in, optional] HWND hWndInsertAfter,
		leftX,					//[in]           int  X,	 // The new position of the left side of the window, in client coordinates
		topY,					//[in]           int  Y,	 // The new position of the top of the window, in client coordinates
		windowWidth,			//[in]           int  cx,	 // The new width of the window, in pixels
		windowHeight,			//[in]           int  cy,	 // The new height of the window, in pixels
		SWP_SHOWWINDOW			//[in]           UINT uFlags // The window sizing and positioning flags
	);
}

void Window::InitWindow()
{
	// elapsed time
	currentTime_ = GetTickCount64();
	elapsedTime_ = currentTime_ - startTime_;

	// system information
	Window::SysInfo();
	//Window::ProcessorInfo();

	// load accelerator
	hAccelTable_ = LoadAcceleratorsW(hInstance_, MAKEINTRESOURCE(IDR_ACCELERATOR));

	// register window class
	LoadStringW(hInstance_, IDS_WINDOW_CLASS, windowClassName_, MAX_LOADSTRING);
	Window::RegisterWindowClass();

	// nCmdShow controls how the window is to be shown
	// parameter is ignored the first time an application calls show window if STARTUPINFO structure specified
	ZeroMemory(&startupInfo_, sizeof(startupInfo_));
	startupInfo_.cb = sizeof(startupInfo_);
	ZeroMemory(&processInfo_, sizeof(processInfo_));
	startupInfo_.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo_.wShowWindow = SW_SHOW;
	int nCmdShow = (startupInfo_.dwFlags & STARTF_USESHOWWINDOW) ? startupInfo_.wShowWindow : SW_SHOWDEFAULT;

	// properties for window creation
	DWORD dwExStyle = 0;
	LONG leftX = 0;
	LONG topY = 0;
	RECT rect = { leftX, topY, Window::WINDOW_WIDTH, Window::WINDOW_HEIGHT };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, dwExStyle);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	// load window title
	SetWindowTitle(L"WINDOW");

	// create window
	hWindow_ = Window::BuildWindow(hInstance_, windowWidth, windowHeight, dwExStyle);

	// register window to receive raw input from keyboards
	Window::RegisterRawInput(hWindow_);

	// center window position relative to screen resolution
	int centerWidth = (desktopWidth_ - windowWidth) / 2;
	int centerHeight = (desktopHeight_ - windowHeight) / 2;
	Window::RepositionWindow(hWindow_, centerWidth, centerHeight, windowWidth, windowHeight);

	// show and update window
	ShowWindow(hWindow_, nCmdShow);
	UpdateWindow(hWindow_);
}

INT_PTR CALLBACK Window::About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

ULONGLONG Window::Elapsed()
{
	currentTime_ = GetTickCount64();
	elapsedTime_ = currentTime_ - startTime_;
	return elapsedTime_;
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
	if (buffer == NULL)
	{
		throw std::runtime_error("Failed to allocate memory!");
	}

	if (!GetLogicalProcessorInformation(buffer, &size))
	{
		free(buffer);
		throw std::runtime_error("Failed to call GetLogicalProcessorInformation!");
	}

	DWORD count = size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
	for (DWORD i = 0; i < count; ++i)
	{
		printf("Processor %u: Relationship = %d\n", i, buffer[i].Relationship);
	}

	free(buffer);

	std::wcout << std::endl;
}

void Window::Cleanup()
{
	if (isCleaned_) return;

	if (hAccelTable_ && hAccelTable_ != nullptr)
	{
		DestroyAcceleratorTable(hAccelTable_);
		hAccelTable_ = nullptr;
	}

	if (hWindow_ && hWindow_ != nullptr)
	{
		DestroyWindow(hWindow_);
		hWindow_ = nullptr;
	}

	if (!UnregisterClassW(windowClassName_, hInstance_))
	{
		DWORD err = GetLastError();
		OutputDebugStringW(L"UnregisterClassW failed!\n");
		std::wcerr << L"UnregisterClassW failed (error " << err << L")\n";
	}

	isCleaned_ = true;
}