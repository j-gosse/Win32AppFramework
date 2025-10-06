/*!
win32\Console\Console.cpp
Created: October 5, 2025
Updated: October 5, 2025
Copyright (c) 2025, Jacob Gosse

Console source file.
*/

#include "Console.hpp"
#include <win32/debug.h>

/* CONSTRUCTORS */

Console::Console() :
	m_consoleAllocated(AllocConsole()),
	m_hConsoleWindow(GetConsoleWindow()),
	m_hConsoleOutput(INVALID_HANDLE_VALUE),
	m_hConsoleInput(INVALID_HANDLE_VALUE),
	m_hInstance(GetModuleHandleW(nullptr)),
	m_consoleTitle(L"")
{
	OutputDebugStringW(L"CONSTRUCTOR: Console()\n");
	Console::InitConsole();
}

Console::Console(HINSTANCE hInstance) :
	m_consoleAllocated(AllocConsole()),
	m_hConsoleWindow(GetConsoleWindow()),
	m_hConsoleOutput(INVALID_HANDLE_VALUE),
	m_hConsoleInput(INVALID_HANDLE_VALUE),
	m_hInstance(hInstance),
	m_consoleTitle(L"")
{
	OutputDebugStringW(L"CONSTRUCTOR: Console(HINSTANCE hInstance)\n");
	Console::InitConsole();
}

/* DESTRUCTOR */

Console::~Console()
{
	OutputDebugStringW(L"DESTRUCTOR: ~Console()\n");
	Console::Cleanup();
}

/* FUNCTION DEFINITIONS */

void Console::InitConsole()
{
	// console allocation check
	if (!m_consoleAllocated)
	{
		OutputDebugStringW(L"Failed to allocate console!");
		OutputDebugStringW(L"Attempting to attach console from parent process...");
		if (!AttachConsole(ATTACH_PARENT_PROCESS))
		{
			throw std::runtime_error("Failed to attach console!");
		}
	}

	// console window allocation check
	if (m_hConsoleWindow == nullptr)
	{
		throw std::runtime_error("Failed to retrieve the console window!");
	}

	Console::CreateOutputHandle();
	Console::CreateInputHandle();
	Console::RedirectStdIO();

	// set console window title
	LoadStringW(m_hInstance, IDS_CONSOLE_TITLE, m_consoleTitle, MAX_LOADSTRING);
	SetConsoleTitleW(m_consoleTitle);

	// console screen buffer
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(m_hConsoleOutput, &csbi);
	COORD bufferSize = csbi.dwSize;
	SMALL_RECT windowRect = csbi.srWindow;
	int windowWidth = windowRect.Right - windowRect.Left + 1;
	int windowHeight = windowRect.Bottom - windowRect.Top + 1;
	printf("Buffer size: %dx%d\n", bufferSize.X, bufferSize.Y);
	printf("Window size: %dx%d\n", windowWidth, windowHeight);

#if defined(_DEBUG) && defined(_WIN32)
	ENABLE_CRT_LEAK_CHECKING;       // enable memory leak checking
	DISABLE_CRT_DELAY_FREE_MEM_DF;  // disable the delay of freeing memory
#endif

	// resize console window
	//if (m_hConsoleWindow)
	//{
	//	MoveWindow(m_hConsoleWindow, 100, 100, CONSOLE_WIDTH, CONSOLE_HEIGHT, TRUE);
	//}
}

void Console::CreateOutputHandle()
{
	m_hConsoleOutput = CreateFileW
	(
		L"CONOUT$",
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (m_hConsoleOutput == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("Invalid handle value assigned to console output!");
	}
}

void Console::CreateInputHandle()
{
	m_hConsoleInput = CreateFileW
	(
		L"CONIN$",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (m_hConsoleInput == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("Invalid handle value assigned to console input!");
	}
}

void Console::RedirectStdIO()
{
	FILE* dummyStream = nullptr;

	freopen_s(&dummyStream, "CONOUT$", "w", stdout);    // Redirect stdout
	std::wcout.clear();
	std::cout.clear();

	freopen_s(&dummyStream, "CONOUT$", "w", stderr);    // Redirect stderr
	std::cerr.clear();

	freopen_s(&dummyStream, "CONIN$", "r", stdin);      // Redirect stdin
	std::cin.clear();
}

void Console::Cleanup()
{
	if (m_hConsoleOutput && m_hConsoleOutput != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hConsoleOutput);
		m_hConsoleOutput = nullptr;
	}

	if (m_hConsoleInput && m_hConsoleInput != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hConsoleInput);
		m_hConsoleInput = nullptr;
	}

	if (m_hConsoleWindow != nullptr)
	{
		DestroyWindow(m_hConsoleWindow);
		m_hConsoleWindow = nullptr;
	}

	if (m_consoleAllocated)
	{
		FreeConsole();
	}
}