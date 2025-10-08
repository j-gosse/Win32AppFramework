/*!
win32\Window\Window.hpp
Created: October 5, 2025
Updated: October 7, 2025
Copyright (c) 2025, Jacob Gosse

Window header file.
*/

#pragma once

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <win32/framework.h>
#include <win32/resource.h>

class Window
{
private:
	HWND m_hWindow;
	WNDCLASSEX m_windowClass;
	HINSTANCE m_hInstance;
	HACCEL m_hAccelTable;
	STARTUPINFO m_startupInfo;
	PROCESS_INFORMATION m_processInfo;
	SYSTEM_INFO m_systemInfo;

	static constexpr const int MAX_LOADSTRING = 100;
	WCHAR m_windowName[MAX_LOADSTRING];
	WCHAR m_windowClassName[MAX_LOADSTRING];

	static constexpr const LONG WINDOW_WIDTH = 800;
	static constexpr const LONG WINDOW_HEIGHT = 600;
	int m_screenWidth = WINDOW_WIDTH;
	int m_screenHeight = WINDOW_HEIGHT;
	LONG m_desktopWidth = GetSystemMetrics(SM_CXSCREEN);
	LONG m_desktopHeight = GetSystemMetrics(SM_CYSCREEN);

	const ULONGLONG m_startTime = GetTickCount64();
	ULONGLONG m_currentTime;
	ULONGLONG m_elapsedTime;

	/**
	* @brief	Handle messages sent to the window on a switch-case basis.
	* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
	* @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
	* @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
	* @return	DefWindowProcW(m_hWindow, uMsg, wParam, lParam) when default switch case or return 0 when switch case
	*/
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	/**
	* @brief	Callback function to process event messages sent to the window.
	* @param	HWND hWnd		: A handle to the window that is receiving messages.
	* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
	* @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
	* @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
	* @return	DefWindowProcW(hWnd, uMsg, wParam, lParam)
	*/
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/**
	* @brief	Registers the main window class.
	* @return	RegisterClassExW(&m_windowClass)
	*/
	ATOM RegisterWindowClass();

	/**
	* @brief	Create the main window.
	* @param	HINSTANCE hInstance : A handle to the window instance module.
	* @param	int windowWidth		: The width of the main window.
	* @param	int windowHeight	: The height of the main window.
	* @param	DWORD dwExStyle		: Extended window styles specifier.
	* @return	HWND hWnd
	*/
	HWND BuildWindow(HINSTANCE hInstance, int windowWidth, int windowHeight, DWORD dwExStyle);

	/**
	* @brief	Register a window to receive raw input from the keyboard.
	* @param	HWND hWnd : A handle to the window that is receiving messages.
	*/
	void RegisterRawInput(HWND hWnd);

	/**
	* @brief	Reposition a window based on the leftX, topY and window width, window height.
	* @param	HWND hWnd		 : A handle to the window that is receiving messages.
	* @param	int leftX		 : The left position of the window.
	* @param	int topY		 : The top position of the window.
	* @param	int windowWidth	 : Width of the window.
	* @param	int windowHeight : Height of the window.
	*/
	void RepositionWindow(HWND hWnd, int leftX, int topY, int windowWidth, int windowHeight);

	/**
	* @brief	Initialize/Customize window properties and create the window.
	*/
	void InitWindow();

	/**
	* @brief	Callback function to generate the About dialog box.
	* @param	HWND hDlg		: A handle to the window dialog box that is receiving the message.
	* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
	* @param	WPARAM wParam	: Provides additional message-specific information.
	* @param	LPARAM lParam	: Provides additional message-specific information.
	* @return	(INT_PTR)TRUE when switch case or (INT_PTR)FALSE when no switch case
	*/
	static INT_PTR CALLBACK About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/**
	* @brief	Returns the number of milliseconds elapsed since the initialization of the Window.
	* @return	ULONGLONG m_elapsedTime
	*/
	ULONGLONG Elapsed();

	/**
	* @brief	Retrieve username, computer name, processor architecture and CPU information.
	*/
	void SysInfo();

	/**
	* @brief	Retrieve logical processor count and relationships.
	*/
	void ProcessorInfo();

	/**
	* @brief	Destroy the window and unregister window class.
	*/
	void Cleanup();

	/**
	* @brief	Log the last error that occurred to the console.
	* @param	const wchar_t* desc : Description of where the error originated from.
	*/
	void LogLastError(const wchar_t* desc);

public:
	/**
	* @brief	Window Constructor.
	*/
	Window();

	/**
	* @brief	1-arg Window Constructor.
	* @param	HINSTANCE hInstance : A handle to the window instance module.
	*/
	Window(HINSTANCE hInstance);

	/**
	* @brief	Copy constructor. (deleted)
	* @param	const Window& : constant lvalue reference
	*/
	Window(const Window&) = delete;

	/**
	* @brief	Copy assignment operator. (deleted)
	* @param	const Window& : constant lvalue reference
	*/
	Window& operator=(const Window&) = delete;

	/**
	* @brief	Move constructor. (deleted)
	* @param	Window&& : rvalue reference
	*/
	Window(Window&&) = delete;

	/**
	* @brief	Move assignment operator. (deleted)
	* @param	Window&& : rvalue reference
	*/
	Window& operator=(Window&&) = delete;

	/**
	* @brief	Window Destructor.
	*/
	virtual ~Window();

	/**
	* @brief	Process queued messages sent to the window. If wMsgFilterMin and wMsgFilterMax are both zero,
				PeekMessage returns all available messages (no range filtering is performed).
	* @param	UINT wMsgFilterMin : The value of the first message in the range of messages to be examined.
	* @param	UINT wMsgFilterMax : The value of the last message in the range of messages to be examined.
	* @return	(int)msg.wParam
	*/
	BOOL ProcessMessages(UINT wMsgFilterMin, UINT wMsgFilterMax) const;

	/**
	* @brief	Get the window handle.
	* @return	HWND m_hWindow
	*/
	HWND GetWindow() const { return m_hWindow; }

	/**
	* @brief	Get the elapsed time in milliseconds.
	* @return	ULONGLONG m_elapsedTime
	*/
	ULONGLONG GetElapsed() { return Elapsed(); }

	/**
	* @brief	Sets the window title.
	* @param	const wchar_t* title : The title of the window.
	*/
	void SetWindowTitle(const wchar_t* title = nullptr)
	{
		if (title && *title) wcsncpy_s(m_windowName, title, static_cast<size_t>(MAX_LOADSTRING) - 1);
		else LoadStringW(m_hInstance, IDS_WINDOW_TITLE, m_windowName, MAX_LOADSTRING);
		if (m_hWindow) SetWindowTextW(m_hWindow, m_windowName);
	}
};

#endif