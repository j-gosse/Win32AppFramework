/*!
lib\include\win32\Window\Window.hpp
Created: October 5, 2025
Updated: November 15, 2025
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
	/**
	* @brief	Create a GUI Window for an application.
	*/
	class Window
	{
	private:
		HWND hWindow_;
		static WNDCLASSEX sWindowClass_;
		static bool sIsClassRegistered_;
		static unsigned int sWindowCount_;
		HINSTANCE hInstance_;
		HACCEL hAccelTable_;
		std::vector<RAWINPUTDEVICE> rawInputDevices_;
		STARTUPINFO startupInfo_;
		PROCESS_INFORMATION processInfo_;
		SYSTEM_INFO systemInfo_;

		std::wstring windowTitle_;
		static std::wstring sWindowClassName_;

		LONG windowWidth_;
		LONG windowHeight_;
		ULONG desktopWidth_ = GetSystemMetrics(SM_CXSCREEN);
		ULONG desktopHeight_ = GetSystemMetrics(SM_CYSCREEN);

		std::chrono::nanoseconds elapsedTime_;

		bool isCleaned_ = false;
		bool isWindowValid = false;

		/**
		* @brief	Initialize window properties and create the window.
		*/
		void InitWindow();

		/**
		* @brief	Handle messages sent to the window on a switch-case basis.
		* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
		* @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
		* @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
		* @return	DefWindowProcW(hWindow_, uMsg, wParam, lParam) when default switch case or return 0 when switch case successful
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
		* @brief	Registers the WNDCLASSEX structure.
		* @param	int extraClassBytes  : The number of extra bytes to allocate following the window-class structure itself. 
					This memory is shared by all windows belonging to the same class.
		* @param	int extraWindowBytes : The number of extra bytes to allocate for each individual window instance created from that class. 
					This memory is unique to each window.
		* @return	ATOM result (0 or 1)
		*/
		ATOM RegisterWindowClass(int extraClassBytes = 0, int extraWindowBytes = 0) const;

		/**
		* @brief	Initialize STARTUPINFO properties.
		*/
		STARTUPINFO InitStartupInfo() noexcept;

		/**
		* @brief	Sets the STARTUPINFO structure.
		* @param	const STARTUPINFO& si : The startup info structure to be applied to the member variable.
		*/
		void SetStartupInfo(const STARTUPINFO& si) noexcept { startupInfo_ = si; }

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
		void SysInfo();

		/**
		* @brief	Retrieve logical processor count and relationships.
		*/
		void ProcessorInfo();

		/**
		* @brief	Unregister the raw input devices and clean raw input device storage.
		*/
		void CleanupRawDevices();

		/**
		* @brief	Clean and release any internal window resources before the window is destroyed.
		*/
		void Cleanup();

	public:
		/**
		* @brief	Default Window Constructor.
		* @param	const std::wstring& title	: Title of the window.
		* @param	LONG windowWidth			: Width of the window in pixels.
		* @param	LONG windowHeight			: Height of the window in pixels.
		*/
		Window(const std::wstring& windowTitle = L"", LONG windowWidth = WINDOW_WIDTH, LONG windowHeight = WINDOW_HEIGHT);

		/**
		* @brief	HINSTANCE Window Constructor.
		* @param	HINSTANCE hInstance			: A handle to the window instance module.
		* @param	const std::wstring& title	: Title of the window.
		* @param	LONG windowWidth			: Width of the window in pixels.
		* @param	LONG windowHeight			: Height of the window in pixels.
		*/
		Window(HINSTANCE hInstance, const std::wstring& windowTitle = L"", LONG windowWidth = WINDOW_WIDTH, LONG windowHeight = WINDOW_HEIGHT);

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
		virtual ~Window() noexcept;

		/**
		* @brief	Default window width in pixels.
		*/
		static constexpr WORD WINDOW_WIDTH = 800;

		/**
		* @brief	Default window height in pixels.
		*/
		static constexpr WORD WINDOW_HEIGHT = 600;

		/**
		* @brief	Virtual Update method. Override in a derived class.
		* @param	std::chrono::nanoseconds deltaTime : The time in nanoseconds since the last frame.
		*/
		virtual void Update(std::chrono::nanoseconds deltaTime) = 0;

		/**
		* @brief	Virtual Render method. Override in a derived class.
		*/
		virtual void Render() = 0;

		/**
		* @brief	Process queued messages sent to the window. If wMsgFilterMin and wMsgFilterMax are both zero,
					PeekMessage returns all available messages (no range filtering is performed).
		* @param	UINT wMsgFilterMin : The value of the first message in the range of messages to be examined.
		* @param	UINT wMsgFilterMax : The value of the last message in the range of messages to be examined.
		*/
		void ProcessMessages(UINT wMsgFilterMin = 0, UINT wMsgFilterMax = 0) const noexcept;

		/**
		* @brief	Unregister the static window class.
		*/
		static void UnregisterWindowClass();

		/**
		* @brief	Returns the window handle.
		* @return	HWND hWindow_
		*/
		HWND GetWindow() const noexcept { return hWindow_; }

		/**
		* @brief	Returns the name of the window class WNDCLASSEX structure.
		* @return	std::wstring& windowClassName_
		*/
		static const std::wstring& GetWindowClassName() noexcept { return sWindowClassName_; }

		/**
		* @brief	Sets the name of the window class WNDCLASSEX structure.
		* @param	const std::wstring& className : The name of the Window class WNDCLASSEX structure.
		*/
		void SetWindowClassName(const std::wstring& className = L"") const noexcept;

		/**
		* @brief	Returns the window title.
		* @return	const std::wstring& windowTitle_
		*/
		const std::wstring& GetWindowTitle() const noexcept { return windowTitle_; }

		/**
		* @brief	Sets the window title.
		* @param	const std::wstring& title : The title of the window.
		*/
		void SetWindowTitle(const std::wstring& title = L"") noexcept;

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

	protected:
		/**
		* @brief	Create a window using the specified values.
		* @param	const std::wstring& windowTitle : Title of the window.
		* @param	const std::wstring& className	: The name of the Window class WNDCLASSEX structure.
		* @param	LONG windowWidth				: The width of the window in pixels.
		* @param	LONG windowHeight				: The height of the window in pixels.
		* @param	DWORD dwStyle					: Window styles specifier.
		* @param	DWORD dwExStyle					: Extended window styles specifier.
		* @return	HWND hWnd
		*/
		HWND BuildWindow(const std::wstring& windowTitle, const std::wstring& className, LONG windowWidth, LONG windowHeight, DWORD dwStyle = 0, DWORD dwExStyle = 0);

		/**
		* @brief	Reposition a window based on the leftX, topY and window width, window height.
		* @param	int leftX		 : The left position of the window.
		* @param	int topY		 : The top position of the window.
		* @param	int windowWidth	 : Width of the window.
		* @param	int windowHeight : Height of the window.
		*/
		void RepositionWindow(int leftX, int topY, int windowWidth, int windowHeight, UINT uFlags = 0) const noexcept;

		/**
		* @brief	Register the window to receive messages from devices that supply raw input.
		* @param	std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices : HID usages are organized into pages of related controls (usage ids).
		* @param	DWORD dwFlags : Mode flag that specifies how to interpret the information provided by usUsagePage and usUsage.
		*/
		void RegisterRawInput(std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices, DWORD dwFlags = 0);

		/**
		* @brief	Returns a reference to the startup info struct.
		*/
		const STARTUPINFO& StartupInfo() const noexcept { return startupInfo_; }

		/**
		* @brief	Returns the static Window WNDCLASSEX structure.
		* @return	const WNDCLASSEX& sWindowClass_
		*/
		static const WNDCLASSEX& GetWindowClass() noexcept { return sWindowClass_; }

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
		void SetRawInputDevices(const std::vector<RAWINPUTDEVICE>& devices) noexcept { rawInputDevices_ = devices; }

		/**
		* @brief	Returns the width of the Window.
		* @return	LONG windowWidth_
		*/
		LONG GetWindowWidth() const noexcept { return windowWidth_; }

		/**
		* @brief	Sets the width the Window.
		* @param	LONG width : The width of the Window in pixels.
		*/
		void SetWindowWidth(LONG width) noexcept { windowWidth_ = width; }

		/**
		* @brief	Returns the height of the window.
		* @return	LONG windowHeight_
		*/
		LONG GetWindowHeight() const noexcept { return windowHeight_; }

		/**
		* @brief	Sets the height of the Window.
		* @param	LONG height : The height of the Window in pixels.
		*/
		void SetWindowHeight(LONG height) noexcept { windowHeight_ = height; }

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