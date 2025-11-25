/*!
lib\include\win32\Window\Window.hpp
Created: October 5, 2025
Updated: November 25, 2025
Copyright (c) 2025, Jacob Gosse

Window header file.
*/

#pragma once

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "IWindow.hpp"
#include "HidUsage.hpp"

namespace winxframe
{
	/**
	* @brief	Create a GUI Window for an application.
	*/
	class Window : public IWindow
	{
	private:
		int showCmd_ = SW_SHOWDEFAULT;

		HDC hMemoryDC_ = nullptr;
		HBITMAP hMemoryBitmap_ = nullptr;
		HBITMAP hOldMemoryBitmap_ = nullptr;

		std::vector<RAWINPUTDEVICE> rawInputDevices_;

		std::wstring windowTitle_;
		LONG screenWidth_;
		LONG screenHeight_;

		std::chrono::nanoseconds elapsedTime_;
		double fps_ = 0.0;

		bool isWindowCleaned_ = false;

		/**
		* @brief	Obtain system information, register the window class and initialize window startup properties, build the window, 
		*			create the memory bitmap buffer, register raw input devices, center the window position, show and update the window, 
		*			and increment window counter.
		* @param    WNDCLASSEX& wcex : Reference to a WNDCLASSEX structure.
		*/
		void InitWindow(WNDCLASSEX& wcex);

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
		* @brief	Callback function to generate the About dialog box.
		* @param	HWND hDlg		: A handle to the window dialog box that is receiving the message.
		* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
		* @param	WPARAM wParam	: Provides additional message-specific information.
		* @param	LPARAM lParam	: Provides additional message-specific information.
		* @return	TRUE when switch case or FALSE otherwise
		*/
		static INT_PTR CALLBACK About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

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

	protected:
		/**
		* @brief	Virtual method called in WM_CREATE. Encapsulate any necessary window creation logic necessary for the base window class. Override in a derived class.
		* @return	0 on success, nonzero or -1 may indicate error or failure.
		*/
		virtual LRESULT OnCreate();

		/**
		* @brief	Virtual method called in WM_DESTROY. Encapsulate any necessary window destruction logic necessary for the base window class. Override in a derived class.
		* @return	0 on success, nonzero or -1 may indicate error or failure.
		*/
		virtual LRESULT OnDestroy();

		/**
		* @brief	Handle a message sent to the window on a switch-case basis.
		* @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
		* @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
		* @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
		* @return	DefWindowProcW(hWindow_, uMsg, wParam, lParam) when default switch case or return 0 when switch case successful
		*/
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		/**
		* @brief	Sets the window's show state. Updates the client area of the specified window by sending a
		*			WM_PAINT message to the window if the window's update region is not empty.
		* @param	The show command, may be passed differently from window creation. Otherwise uses showCmd_ member variable.
		*/
		void ShowAndUpdateWindow(int nCmdShow = -1) const noexcept;

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
		* @brief	Set the width of the window client area.
		* @param	LONG width : The width of the window in pixels.
		*/
		void SetScreenWidth(LONG width) noexcept { screenWidth_ = width; }

		/**
		* @brief	Set the height of the window client area.
		* @param	LONG height : The height of the window in pixels.
		*/
		void SetScreenHeight(LONG height) noexcept { screenHeight_ = height; }

		/**
		* @brief	Set the width and height of the window client area.
		* @param	LONG width : The width of the window in pixels.
		* @param	LONG height : The height of the window in pixels.
		*/
		void SetScreenSize(LONG width, LONG height) noexcept { screenWidth_ = width; screenHeight_ = height; }

		/**
		* @brief	Return the boolean value checking if window cleanup has been performed.
		* @return	bool isWindowCleaned_
		*/
		bool IsWindowCleaned() const noexcept { return isWindowCleaned_; }

	public:
		/**
		* @brief	Default Window Constructor.
		* @param    WNDCLASSEX& wcex            : Reference to the WNDCLASSEX structure.
		* @param	const std::wstring& title	: Title of the window.
		* @param	LONG screenWidth			: Width of the window client area in pixels.
		* @param	LONG screenHeight			: Height of the window client area in pixels.
		* @param	MessagePumpMode mode		: Real-time or event-driven mode.
		*/
		Window(
			WNDCLASSEX& wcex, 
			const std::wstring& windowTitle = L"", 
			LONG screenWidth = DEFAULT_SCREEN_WIDTH, 
			LONG screenHeight = DEFAULT_SCREEN_HEIGHT, 
			MessagePumpMode mode = MessagePumpMode::RealTime,
			int nCmdShow = SW_SHOWDEFAULT
		);

		/**
		* @brief	HINSTANCE Window Constructor.
		* @param	HINSTANCE hInstance			: A handle to the window instance module.
		* @param    WNDCLASSEX& wcex            : Reference to the WNDCLASSEX structure.
		* @param	const std::wstring& title	: Title of the window.
		* @param	LONG screenWidth			: Width of the window client area in pixels.
		* @param	LONG screenHeight			: Height of the window client area in pixels.
		* @param	MessagePumpMode mode		: Real-time or event-driven mode.
		*/
		Window(
			HINSTANCE hInstance, 
			WNDCLASSEX& wcex, 
			const std::wstring& windowTitle = L"", 
			LONG screenWidth = DEFAULT_SCREEN_WIDTH,
			LONG screenHeight = DEFAULT_SCREEN_HEIGHT,
			MessagePumpMode mode = MessagePumpMode::RealTime,
			int nCmdShow = SW_SHOWDEFAULT
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
		virtual ~Window() noexcept override;

		/**
		* @brief	Default window client area width in pixels.
		*/
		static constexpr WORD DEFAULT_SCREEN_WIDTH = 800;

		/**
		* @brief	Default window client area height in pixels.
		*/
		static constexpr WORD DEFAULT_SCREEN_HEIGHT = 600;

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
		* @brief	Return the memory device context that has the backbuffer bitmap selected into it.
		* @return	Handle to a device context for memory bitmap.
		*/
		HDC GetMemoryDC() const noexcept { return hMemoryDC_; }

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
		* @brief	Return the width of the window client area.
		* @return	LONG screenWidth_
		*/
		LONG GetScreenWidth() const noexcept { return screenWidth_; }

		/**
		* @brief	Return the height of the window client area.
		* @return	LONG screenHeight_
		*/
		LONG GetScreenHeight() const noexcept { return screenHeight_; }

		/**
		* @brief	Return the width of the entire window.
		* @return	LONG window width
		*/
		LONG GetWindowWidth() const noexcept { RECT rect; GetWindowRect(this->GetWindow(), &rect); return rect.right - rect.left; }

		/**
		* @brief	Return the height of the entire window.
		* @return	LONG window height
		*/
		LONG GetWindowHeight() const noexcept { RECT rect; GetWindowRect(this->GetWindow(), &rect); return rect.bottom - rect.top; }

		/**
		* @brief	Retrieve the window width and window height inclusive of non-client area and assign the values to the referenced outWidth and outHeight parameters.
		* @param	LONG& outWidth	: Reference to the outer-scope width variable.
		* @param	LONG& outHeight : Reference to the outer-scope height variable.
		* @param	DWORD dwStyle	: Window styles specifier.
		* @param	BOOL hasMenu	: Indicates whether the window has a menu.
		* @param	DWORD dwExStyle : Extended window styles specifier.
		*/
		void GetWindowSize(LONG& outWidth, LONG& outHeight, DWORD dwStyle = WS_OVERLAPPEDWINDOW, BOOL hasMenu = TRUE, DWORD dwExStyle = 0) const noexcept;

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
	};
}; // end of namespace winxframe

#endif