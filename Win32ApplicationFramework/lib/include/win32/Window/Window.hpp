/*!
lib\include\win32\Window\Window.hpp
Created: October 5, 2025
Updated: November 20, 2025
Copyright (c) 2025, Jacob Gosse

Window header file.
*/

#pragma once

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <win32/framework.h>
#include <win32/resource.h>
#include <win32/Window/window_manager.hpp>
#include "MessagePumpMode.hpp"
#include "HidUsage.hpp"

namespace winxframe
{
	/**
	* @brief	Create a GUI Window for an application.
	*/
	class Window
	{
	private:
		HWND hWindow_ = nullptr;
		HINSTANCE hInstance_;
		HACCEL hAccelTable_ = nullptr;

		HDC     hMemoryDC_ = nullptr;
		HBITMAP hMemoryBitmap_ = nullptr;
		HBITMAP hOldMemoryBitmap_ = nullptr;

		std::vector<RAWINPUTDEVICE> rawInputDevices_;
		STARTUPINFO startupInfo_{};
		PROCESS_INFORMATION processInfo_{};
		SYSTEM_INFO systemInfo_{};
		MessagePumpMode pumpMode_;

		std::wstring windowTitle_;
		static std::wstring sWindowClassName_;
		static WNDCLASSEX sWindowClass_;
		static bool sIsClassRegistered_;

		static unsigned int sRealTimeWindowCount_;
		static unsigned int sEventDrivenWindowCount_;

		LONG windowWidth_;
		LONG windowHeight_;
		ULONG desktopWidth_ = GetSystemMetrics(SM_CXSCREEN);
		ULONG desktopHeight_ = GetSystemMetrics(SM_CYSCREEN);

		std::chrono::nanoseconds elapsedTime_;
		double fps_ = 0.0;

		bool isCleaned_ = false;
		bool isWindowCleaned_ = false;

		/**
		* @brief	Obtain system information, register the window class and initialize window startup properties, build the window, 
		*			create the memory bitmap buffer, register raw input devices, center the window position, show and update the window, 
		*			and increment window counter.
		*/
		void InitWindow();

		/**
		* @brief	Register the WNDCLASSEX structure for the window.
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
		* @brief	Set the STARTUPINFO structure.
		* @param	const STARTUPINFO& si : The startup info structure to be applied to the member variable.
		*/
		void SetStartupInfo(const STARTUPINFO& si) noexcept { startupInfo_ = si; }

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
		* @brief	Create the memory bitmap buffer. Checks for existing memory bitmap and destroys, sets the memory bitmap device context handle, 
		*			creates the memory bitmap buffer and assigns the initialized bitmap object to the old bitmap member variable.
		*/
		void CreateMemoryBitmap();

		/**
		 * @brief	Clear the memory bitmap buffer to a specified color (default black).
		 * @param	COLORREF color : The fill color for the buffer. Defaults to black (RGB(0,0,0)).
		 */
		void ClearMemoryBitmapBuffer(COLORREF color = RGB(0, 0, 0)) const;

		/**
		* @brief	Register the window to receive messages from devices that supply raw input.
		* @param	std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices : HID usages are organized into pages of related controls (usage ids).
		* @param	DWORD dwFlags : Mode flag that specifies how to interpret the information provided by usUsagePage and usUsage.
		*/
		void RegisterRawInput(std::initializer_list<std::pair<HidUsagePage, HidUsageId>> devices, DWORD dwFlags = 0);

		/**
		* @brief	Increment the real-time or event-driven static window count based on current window pump mode.
		*/
		void IncrementWindowCount() const;

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
		* @brief	Release and delete the memory bitmap and memory bitmap device context handle.
		*/
		void DestroyMemoryBitmap();

		/**
		* @brief	Unregister the raw input devices and clean raw input device storage.
		*/
		void CleanupRawDevices();

		/**
		* @brief	Clean and release any internal resources associated with the window handle. Decrement real-time or event-driven static window 
		*			count based on current window pump mode.
		*/
		void CleanupWindowResources();

		/**
		* @brief	Clean and release any other internal resources not associated with the window handle.
		*/
		void Cleanup();

		/**
		* @brief	Decrement real-time or event-driven static window count based on current window pump mode.
		*/
		void DecrementWindowCount() const;

	public:
		/**
		* @brief	Default Window Constructor.
		* @param	const std::wstring& title	: Title of the window.
		* @param	LONG windowWidth			: Width of the window in pixels.
		* @param	LONG windowHeight			: Height of the window in pixels.
		* @param	MessageLoopMode mode		: Determines if the window is in real time or event driven mode.
		*/
		Window(
			const std::wstring& windowTitle = L"", 
			LONG windowWidth = WINDOW_WIDTH, 
			LONG windowHeight = WINDOW_HEIGHT, 
			MessagePumpMode mode = MessagePumpMode::RealTime
		);

		/**
		* @brief	HINSTANCE Window Constructor.
		* @param	HINSTANCE hInstance			: A handle to the window instance module.
		* @param	const std::wstring& title	: Title of the window.
		* @param	LONG windowWidth			: Width of the window in pixels.
		* @param	LONG windowHeight			: Height of the window in pixels.
		* @param	MessageLoopMode mode		: Determines if the window is in real time or event driven mode.
		*/
		Window(
			HINSTANCE hInstance, 
			const std::wstring& windowTitle = L"", 
			LONG windowWidth = WINDOW_WIDTH, 
			LONG windowHeight = WINDOW_HEIGHT,
			MessagePumpMode mode = MessagePumpMode::RealTime
		);

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
		* @brief	Dispatch incoming nonqueued messages, check the thread message queue for a posted message, and retrieve the message.
		*			If wMsgFilterMin and wMsgFilterMax are both zero, returns all available messages.
		* @param	UINT wMsgFilterMin : The value of the first message in the range of messages to be examined.
		* @param	UINT wMsgFilterMax : The value of the last message in the range of messages to be examined.
		*/
		void PeekMessages(UINT wMsgFilterMin = 0, UINT wMsgFilterMax = 0) const noexcept;

		/**
		* @brief	Retrieve a message from the calling thread's message queue. Dispatch incoming sent messages until a posted message is available for retrieval.
		*			If wMsgFilterMin and wMsgFilterMax are both zero, returns all available messages.
		* @param	UINT wMsgFilterMin : The value of the first message in the range of messages to be examined.
		* @param	UINT wMsgFilterMax : The value of the last message in the range of messages to be examined.
		* @return	Return value can be nonzero, zero, or -1.
		*/
		int GetMessages(UINT wMsgFilterMin = 0, UINT wMsgFilterMax = 0) const noexcept;

		/**
		* @brief	Unregister the static WNDCLASSEX structure for the window.
		*/
		static void UnregisterWindowClass();

		/**
		* @brief	Return the window handle.
		* @return	HWND hWindow_
		*/
		HWND GetWindow() const noexcept { return hWindow_; }

		/**
		* @brief	Return the memory device context that has the backbuffer bitmap selected into it.
		* @return	HDC hMemoryDC_
		*/
		HDC GetMemoryDC() const noexcept { return hMemoryDC_; }

		/**
		* @brief	Return the current message pump mode for the window.
		*/
		MessagePumpMode GetPumpMode() const noexcept { return pumpMode_; }

		/**
		* @brief	Return the name of the window class WNDCLASSEX structure.
		* @return	std::wstring& windowClassName_
		*/
		static const std::wstring& GetWindowClassName() noexcept { return sWindowClassName_; }

		/**
		* @brief	Set the name of the window class WNDCLASSEX structure.
		* @param	const std::wstring& className : The name of the Window class WNDCLASSEX structure.
		*/
		void SetWindowClassName(const std::wstring& className = L"") const noexcept;

		/**
		* @brief	Return the window title.
		* @return	const std::wstring& windowTitle_
		*/
		const std::wstring& GetWindowTitle() const noexcept { return windowTitle_; }

		/**
		* @brief	Set the window title.
		* @param	const std::wstring& title : The title of the window.
		*/
		void SetWindowTitle(const std::wstring& title = L"") noexcept;

		/**
		* @brief	Return the number of nanoseconds elapsed since the initialization of the application.
		* @return	std::chrono::nanoseconds elapsedTime_
		*/
		std::chrono::nanoseconds GetElapsed() const noexcept { return elapsedTime_; }

		/**
		* @brief	Set the number of nanoseconds elapsed since the initialization of the application.
		* @param	std::chrono::nanoseconds elapsed : The elapsed time in nanoseconds.
		*/
		void SetElapsed(std::chrono::nanoseconds elapsed) noexcept { elapsedTime_ = elapsed; }

		/**
		* @brief	Return the current frames per second value.
		* @return	double fps_
		*/
		double GetFPS() const noexcept { return fps_; }

		/**
		* @brief	Set the current frames per second value.
		* @param	double fps : frames per second
		*/
		void SetFPS(double fps) noexcept { fps_ = fps; }

		/**
		* @brief	Return the static real-time window counter.
		* @return	static unsigned int sRealTimeWindowCount_
		*/
		static unsigned int GetRealTimeWindowCount() noexcept { return Window::sRealTimeWindowCount_; }

		/**
		* @brief	Return the static event-driven window counter.
		* @return	static unsigned int sEventDrivenWindowCount_
		*/
		static unsigned int GetEventDrivenWindowCount() noexcept { return Window::sEventDrivenWindowCount_; }

	protected:
		/**
		* @brief	Reposition a window based on the leftX, topY and window width, window height.
		* @param	int leftX		 : The left position of the window.
		* @param	int topY		 : The top position of the window.
		* @param	int windowWidth	 : Width of the window.
		* @param	int windowHeight : Height of the window.
		*/
		void RepositionWindow(int leftX, int topY, int windowWidth, int windowHeight, UINT uFlags = 0) const noexcept;

		/**
		 * @brief	Prepare the memory bitmap buffer for a new frame.
		 * @param	COLORREF clearColor : Clears the memory bitmap buffer to the default or specified color.
		 */
		void BeginFrame(COLORREF clearColor = RGB(0, 0, 0));

		/**
		 * @brief	Copy the memory bitmap buffer to the window's device context.
		 */
		void Present() const;

		/**
		* @brief	Return a reference to the startup info struct.
		* @return	STARTUPINFO& startupInfo_
		*/
		const STARTUPINFO& StartupInfo() const noexcept { return startupInfo_; }

		/**
		* @brief	Return the static Window WNDCLASSEX structure.
		* @return	const WNDCLASSEX& sWindowClass_
		*/
		static const WNDCLASSEX& GetWindowClass() noexcept { return sWindowClass_; }

		/**
		* @brief	Return the handle for the memory bitmap buffer.
		* @return	HBITMAP hMemoryBitmap_
		*/
		HBITMAP GetMemoryBitmap() const noexcept { return hMemoryBitmap_; }

		/**
		* @brief	Return a vector of the registered raw input devices.
		* @return	const std::vector<RAWINPUTDEVICE>& registeredRawInputDevices_
		*/
		const std::vector<RAWINPUTDEVICE>& GetRawInputDevices() const noexcept { return rawInputDevices_; }

		/**
		* @brief	Set the registered raw input devices vector.
		*			If an rvalue (temporary vector) is passed, the contents will be moved. If an lvalue is passed, the contents will be copied.
		* @tparam	T Type of the vector argument : lvalue or rvalue reference.
		* @param	T&& devices : The vector of RAWINPUTDEVICE objects to set.
		*/
		template<typename T>
		void SetRawInputDevices(T&& devices) noexcept { rawInputDevices_ = std::forward<T>(devices); }

		/**
		* @brief	Return the width of the window.
		* @return	LONG windowWidth_
		*/
		LONG GetWindowWidth() const noexcept { return windowWidth_; }

		/**
		* @brief	Set the width the Window.
		* @param	LONG width : The width of the window in pixels.
		*/
		void SetWindowWidth(LONG width) noexcept { windowWidth_ = width; }

		/**
		* @brief	Return the height of the window.
		* @return	LONG windowHeight_
		*/
		LONG GetWindowHeight() const noexcept { return windowHeight_; }

		/**
		* @brief	Set the height of the Window.
		* @param	LONG height : The height of the window in pixels.
		*/
		void SetWindowHeight(LONG height) noexcept { windowHeight_ = height; }

		/**
		* @brief	Set the width and height of the window.
		* @param	LONG width : The width of the window in pixels.
		* @param	LONG height : The height of the window in pixels.
		*/
		void SetWindowSize(LONG width, LONG height) noexcept { windowWidth_ = width; windowHeight_ = height; }

		/**
		* @brief	Return the width of the Desktop screen.
		* @return	ULONG desktopWidth_
		*/
		ULONG GetDesktopWidth() const noexcept { return desktopWidth_; }

		/**
		* @brief	Return the height of the Desktop screen.
		* @return	ULONG desktopHeight_
		*/
		ULONG GetDesktopHeight() const noexcept { return desktopHeight_; }

		/**
		* @brief	Return the boolean value checking if general cleanup has been performed.
		* @return	bool isCleaned_
		*/
		bool IsCleaned() const noexcept { return isCleaned_; }

		/**
		* @brief	Return the boolean value checking if window cleanup has been performed.
		* @return	bool isWindowCleaned_
		*/
		bool IsWindowCleaned() const noexcept { return isWindowCleaned_; }
	};
}; // end of namespace winxframe

#endif