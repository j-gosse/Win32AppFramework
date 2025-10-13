/*!
win32\Window\Window.hpp
Created: October 5, 2025
Updated: October 13, 2025
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
	HWND hWindow_;
	WNDCLASSEX windowClass_;
	HINSTANCE hInstance_;
	HACCEL hAccelTable_;
	STARTUPINFO startupInfo_;
	PROCESS_INFORMATION processInfo_;
	SYSTEM_INFO systemInfo_;

	static constexpr const int MAX_LOADSTRING = 100;
	WCHAR windowName_[MAX_LOADSTRING];
	WCHAR windowClassName_[MAX_LOADSTRING];

	static constexpr const LONG WINDOW_WIDTH = 800;
	static constexpr const LONG WINDOW_HEIGHT = 600;
	int screenWidth_ = WINDOW_WIDTH;
	int screenHeight_ = WINDOW_HEIGHT;
	LONG desktopWidth_ = GetSystemMetrics(SM_CXSCREEN);
	LONG desktopHeight_ = GetSystemMetrics(SM_CYSCREEN);

	const ULONGLONG startTime_ = GetTickCount64();
	ULONGLONG currentTime_;
	ULONGLONG elapsedTime_;

	bool isCleaned_ = false;

	/**
	* @brief	Handle messages sent to the window on a switch-case basis.
	* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
	* @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
	* @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
	* @return	DefWindowProcW(hWindow_, uMsg, wParam, lParam) when default switch case or return 0 when switch case
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
	* @return	RegisterClassExW(&windowClass_)
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
	* @return	ULONGLONG elapsedTime_
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
	* @return	HWND hWindow_
	*/
	HWND GetWindow() const { return hWindow_; }

	/**
	* @brief	Get the elapsed time in milliseconds.
	* @return	ULONGLONG elapsedTime_
	*/
	ULONGLONG GetElapsed() { return Elapsed(); }

	/**
	* @brief	Sets the window title.
	* @param	const wchar_t* title : The title of the window.
	*/
	void SetWindowTitle(const wchar_t* title = nullptr)
	{
		if (title && *title) wcsncpy_s(windowName_, title, static_cast<size_t>(MAX_LOADSTRING) - 1);
		else LoadStringW(hInstance_, IDS_WINDOW_TITLE, windowName_, MAX_LOADSTRING);
		if (hWindow_) SetWindowTextW(hWindow_, windowName_);
	}
};

#endif