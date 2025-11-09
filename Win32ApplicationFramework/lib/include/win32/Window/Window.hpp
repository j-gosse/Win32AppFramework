/*!
lib\include\win32\Window\Window.hpp
Created: October 5, 2025
Updated: November 8, 2025
Copyright (c) 2025, Jacob Gosse

Window header file.
*/

#pragma once

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <win32/framework.h>
#include <win32/resource.h>
#include <win32/Window/HidUsage.hpp>

namespace winxframe
{
	class Window
	{
	private:
		HWND hWindow_;
		WNDCLASSEX windowClass_;
		HINSTANCE hInstance_;
		HACCEL hAccelTable_;
		std::vector<RAWINPUTDEVICE> rawInputDevices_;
		STARTUPINFO startupInfo_;
		PROCESS_INFORMATION processInfo_;
		SYSTEM_INFO systemInfo_;

		std::wstring windowTitle_;
		std::wstring windowClassName_;

		static constexpr WORD WINDOW_WIDTH = 800;
		static constexpr WORD WINDOW_HEIGHT = 600;
		ULONG screenWidth_ = WINDOW_WIDTH;
		ULONG screenHeight_ = WINDOW_HEIGHT;
		ULONG desktopWidth_ = GetSystemMetrics(SM_CXSCREEN);
		ULONG desktopHeight_ = GetSystemMetrics(SM_CYSCREEN);

		std::chrono::nanoseconds elapsedTime_;

		bool isCleaned_ = false;

		/**
		* @brief	Handle messages sent to the window on a switch-case basis.
		* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
		* @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
		* @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
		* @return	DefWindowProcW(hWindow_, uMsg, wParam, lParam) when default switch case or return 0 when switch case/successful
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
		* @param	int extraClassBytes  : The number of extra bytes to allocate following the window-class structure itself. 
					This memory is shared by all windows belonging to the same class.
		* @param	int extraWindowBytes : The number of extra bytes to allocate for each individual window instance created from that class. 
					This memory is unique to each window.
		* @return	RegisterClassExW(&windowClass_)
		*/
		ATOM RegisterWindowClass(int extraClassBytes = 0, int extraWindowBytes = 0);

		/**
		* @brief	Create the main window.
		* @param	int windowWidth		: The width of the main window.
		* @param	int windowHeight	: The height of the main window.
		* @param	DWORD dwExStyle		: Extended window styles specifier.
		* @return	HWND hWnd
		*/
		HWND BuildWindow(int windowWidth = WINDOW_WIDTH, int windowHeight = WINDOW_HEIGHT, DWORD dwExStyle = 0);

		/**
		* @brief	Initialize/Customize the startup info properties.
		*/
		STARTUPINFO InitStartupInfo() noexcept;

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
		* @return	TRUE when switch case or FALSE otherwise
		*/
		static INT_PTR CALLBACK About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

		/**
		* @brief	Retrieve username, computer name, processor architecture and CPU information.
		*/
		void SysInfo() noexcept;

		/**
		* @brief	Retrieve logical processor count and relationships.
		*/
		void ProcessorInfo();

		/**
		* @brief	Unregister the raw input devices and clean raw input device storage.
		*/
		void CleanRawDevices();

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
		~Window();

		/**
		* @brief	Process queued messages sent to the window. If wMsgFilterMin and wMsgFilterMax are both zero,
					PeekMessage returns all available messages (no range filtering is performed).
		* @param	UINT wMsgFilterMin : The value of the first message in the range of messages to be examined.
		* @param	UINT wMsgFilterMax : The value of the last message in the range of messages to be examined.
		* @return	(int)msg.wParam
		*/
		BOOL ProcessMessages(UINT wMsgFilterMin = 0, UINT wMsgFilterMax = 0);

		/**
		* @brief	Register the window to receive messages from devices that supply raw input.
		* @param	std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices : HID usages are organized into pages of related controls (usage ids).
		* @param	DWORD dwFlags : Mode flag that specifies how to interpret the information provided by usUsagePage and usUsage.
		*/
		void RegisterRawInput(std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices, DWORD dwFlags = 0);

		/**
		* @brief	Reposition a window based on the leftX, topY and window width, window height.
		* @param	int leftX		 : The left position of the window.
		* @param	int topY		 : The top position of the window.
		* @param	int windowWidth	 : Width of the window.
		* @param	int windowHeight : Height of the window.
		*/
		void RepositionWindow(int leftX, int topY, int windowWidth, int windowHeight) const noexcept;

		/**
		* @brief	Returns the Window handle.
		* @return	HWND hWindow_
		*/
		HWND GetWindow() const noexcept { return hWindow_; }

		/**
		* @brief	Sets the Window handle.
		* @param	HWND window : const handle to the window.
		*/
		void SetWindow(HWND window) noexcept { hWindow_ = window; }

		/**
		* @brief	Returns the Window class structure.
		* @return	const WNDCLASSEX& windowClass_
		*/
		const WNDCLASSEX& GetWindowClass() const noexcept { return windowClass_; }

		/**
		* @brief	Sets the Window class structure.
		* @param	const WNDCLASSEX& windowClass : const reference to the Window class structure.
		*/
		void SetWindowClass(const WNDCLASSEX& windowClass) noexcept { windowClass_ = windowClass; }

		/**
		* @brief	Returns the Window instance handle.
		* @return	HINSTANCE hInstance_
		*/
		HINSTANCE GetInstance() const noexcept { return hInstance_; }

		/**
		* @brief	Returns the Accelerator Table.
		* @return	HACCEL hAccelTable_
		*/
		HACCEL GetAccelTable() const noexcept { return hAccelTable_; }

		/**
		* @brief	Sets the Accelerator Table.
		* @param	HACCEL accelTable : A handle to the module whose executable file contains the accelerator table to be loaded.
		*/
		void SetAccelTable(HACCEL accelTable) noexcept { hAccelTable_ = accelTable; }

		/**
		* @brief	Returns a vector of the registered raw input devices.
		* @return	const std::vector<RAWINPUTDEVICE>& registeredRawInputDevices_
		*/
		const std::vector<RAWINPUTDEVICE>& GetRawInputDevices() const noexcept { return rawInputDevices_; }

		/**
		* @brief	Sets the registed raw input devices vector with the raw input devices.
		* @param	std::vector<RAWINPUTDEVICE>&& devices
		*/
		void SetRawInputDevices(std::vector<RAWINPUTDEVICE>&& devices) noexcept { rawInputDevices_ = std::move(devices); }
		void SetRawInputDevices(const std::vector<RAWINPUTDEVICE>& devices) { rawInputDevices_ = devices; }

		/**
		* @brief	Returns a reference to the startup info struct.
		*/
		const STARTUPINFO& StartupInfo() const noexcept { return startupInfo_; }

		/**
		* @brief	Sets the startup info struct.
		* @param	const STARTUPINFO& si : The startup info structure to be applied to the member variable.
		*/
		void SetStartupInfo(const STARTUPINFO& si) noexcept { startupInfo_ = si; }

		/**
		* @brief	Returns the width of the Window screen.
		* @return	ULONG screenWidth_
		*/
		ULONG GetScreenWidth() const noexcept { return screenWidth_; }

		/**
		* @brief	Sets the width the Window screen.
		* @param	ULONG width : The width of the screen in pixels.
		*/
		void SetScreenWidth(ULONG width) noexcept { screenWidth_ = width; }

		/**
		* @brief	Returns the height of the Window screen.
		* @return	ULONG screenHeight_
		*/
		ULONG GetScreenHeight() const noexcept { return screenHeight_; }

		/**
		* @brief	Sets the height of the Window screen.
		* @param	ULONG height : The height of the screen in pixels.
		*/
		void SetScreenHeight(ULONG height) noexcept { screenHeight_ = height; }

		/**
		* @brief	Returns the width of the Desktop screen.
		* @return	ULONG desktopWidth_
		*/
		ULONG GetDesktopWidth() const noexcept { return desktopWidth_; }

		/**
		* @brief	Returns the height of the Desktop screen.
		* @return	ULONG desktopHeight_
		*/
		ULONG GetDesktopHeight() const noexcept { return desktopHeight_; }

		/**
		* @brief	Returns the number of nanoseconds elapsed since the initialization of the application.
		* @return	std::chrono::nanoseconds elapsedTime_
		*/
		std::chrono::nanoseconds GetElapsed() const noexcept { return elapsedTime_; }

		/**
		* @brief	Sets the number of nanoseconds elapsed since the initialization of the application.
		* @param	std::chrono::nanoseconds elapsed : The elapsed time in nanoseconds.
		*/
		void SetElapsed(std::chrono::nanoseconds elapsed) noexcept { elapsedTime_ = elapsed; }

		/**
		* @brief	Returns the name of the window class.
		* @return	std::wstring& windowClassName_
		*/
		const std::wstring& GetWindowClassName() const noexcept { return windowClassName_; }

		/**
		* @brief	Sets the name of the window class.
		* @param	const std::wstring& className : The name of the Window class.
		*/
		void SetWindowClassName(const std::wstring& className = L"") noexcept;

		/**
		* @brief	Get the Window title.
		* @return	const std::wstring& windowTitle_
		*/
		const std::wstring& GetWindowTitle() const noexcept { return windowTitle_; }

		/**
		* @brief	Sets the window title.
		* @param	const std::wstring& title : The title of the Window.
		*/
		void SetWindowTitle(const std::wstring& title = L"") noexcept;

		/**
		* @brief	Returns the boolean value checking if Window cleanup has been run.
		* @return	bool isCleaned_
		*/
		bool IsCleaned() const noexcept { return isCleaned_; }

		/**
		* @brief	Sets the isCleaned boolean value.
		* @param	bool isCleaned : The boolean value checking if Window cleanup has been run.
		*/
		void SetIsCleaned(bool isCleaned) noexcept { isCleaned_ = isCleaned; }
	};
}; // end of namespace winxframe

#endif