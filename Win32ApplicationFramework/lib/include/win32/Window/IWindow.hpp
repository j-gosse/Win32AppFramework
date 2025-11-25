/*!
lib\include\win32\Window\IWindow.hpp
Created: November 24, 2025
Updated: November 25, 2025
Copyright (c) 2025, Jacob Gosse

IWindow header file.
*/

#pragma once

#ifndef IWINDOW_HPP_
#define IWINDOW_HPP_

#include <win32/framework.h>
#include <win32/resource.h>
#include "MessagePumpMode.hpp"

namespace winxframe
{
    /**
    * @brief	Create a base Window for an application.
    */
    class IWindow
    {
    private:
        HINSTANCE hInstance_;
        HWND hWindow_ = nullptr;
        HACCEL hAccelTable_ = nullptr;
        MessagePumpMode pumpMode_;
        static unsigned int sRealTimeWindowCount_;
        static unsigned int sEventDrivenWindowCount_;
        bool isCreated_ = false;
        bool isCleaned_ = false;

        /**
        * @brief	Callback function to process event messages sent to the window.
        * @param	HWND hWnd		: A handle to the window that is receiving messages.
        * @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
        * @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
        * @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
        * @return	DefWindowProcW(hWnd, uMsg, wParam, lParam) or HandleMessage(uMsg, wParam, lParam)
        */
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
        * @brief	Register the WNDCLASSEX structure for the window.
        * @param    WNDCLASSEX& wcex : Reference to the WNDCLASSEX structure.
        * @return	ATOM result (0 or 1)
        */
        ATOM RegisterWindowClass(WNDCLASSEX& wcex) const;

        /**
        * @brief	Clean and release any other internal resources not associated with the window handle.
        */
        void Cleanup();

    protected:
        /**
        * @brief	Create a window using the specified values.
        * @param    WNDCLASSEX& wcex                : Reference to the WNDCLASSEX structure.
        * @param	const std::wstring& windowTitle : Title of the window.
        * @param	int leftX						: The initial horizontal position of the window in client coordinates.
        * @param	int topY						: The initial vertical position of the window in client coordinates.
        * @param	LONG windowWidth				: The width in device units of the window.
        * @param	LONG windowHeight				: The height in device units of the window.
        * @param	DWORD dwStyle					: Window styles specifier.
        * @param	DWORD dwExStyle					: Extended window styles specifier.
        * @return	TRUE if window not equal to nullptr, FALSE otherwise.
        */
        bool Create(
            WNDCLASSEX& wcex,
            const std::wstring& title = L"",
            int leftX = CW_USEDEFAULT,
            int topY = CW_USEDEFAULT,
            LONG windowWidth = CW_USEDEFAULT,
            LONG windowHeight = CW_USEDEFAULT,
            HWND parent = nullptr,
            HMENU menu = nullptr,
            DWORD dwStyle = WS_OVERLAPPEDWINDOW,
            DWORD dwExStyle = 0
        );

        /**
        * @brief	Handle a message sent to the window on a switch-case basis. Override in derived classes to customize message handling.
        * @param	UINT uMsg		: The message identifier. This parameter specifies which message is being sent to the window.
        * @param	WPARAM wParam	: Provides additional message-specific information. Indicates whether the window was minimized, maximized, or resized.
        * @param	LPARAM lParam	: Provides additional message-specific information. Contains the new width and height of the window.
        * @return	DefWindowProcW(hWindow_, uMsg, wParam, lParam) when default switch case or return 0 when switch case successful
        */
        virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
        * @brief	Increment the real-time or event-driven static window count based on current window pump mode.
        */
        void IncrementWindowCount() const noexcept;

        /**
        * @brief	Decrement real-time or event-driven static window count based on current window pump mode.
        */
        void DecrementWindowCount() const noexcept;

    public:
        /**
        * @brief	Default IWindow Constructor.
        * @param	MessagePumpMode mode : Real-time or event-driven mode.
        */
        IWindow(MessagePumpMode mode = MessagePumpMode::RealTime);

        /**
        * @brief	HINSTANCE IWindow Constructor.
        * @param	HINSTANCE hInstance  : A handle to the window instance module.
        * @param	MessagePumpMode mode : Real-time or event-driven mode.
        */
        IWindow(HINSTANCE hInstance, MessagePumpMode mode = MessagePumpMode::RealTime);

        /**
        * @brief	IWindow Destructor.
        */
        virtual ~IWindow() noexcept;

        /**
        * @brief    Return the instance handle.
        * @return   HINSTANCE hInstance_
        */
        HINSTANCE GetInstance() const noexcept { return hInstance_; }

        /**
        * @brief	Return the window handle.
        * @return	HWND hWindow_
        */
        HWND GetWindow() const noexcept { return hWindow_; }

        /**
        * @brief    Return a boolean value based on if the window handle is not equal to nullptr.
        * @return   TRUE if hWindow_ != nullptr, false otherwise.
        */
        bool IsWindow() const noexcept { return hWindow_ != nullptr; }

        /**
        * @brief    Return the handle to the accelerator table.
        * @return   HACCEL hAccelTable_
        */
        HACCEL GetAccelTable() const noexcept { return hAccelTable_; }

        /**
        * @brief	Return the message pump mode for the window.
        */
        MessagePumpMode GetPumpMode() const noexcept { return pumpMode_; }

        /**
        * @brief	Return the static real-time window counter.
        * @return	unsigned int sRealTimeWindowCount_
        */
        static unsigned int GetRealTimeWindowCount() noexcept { return IWindow::sRealTimeWindowCount_; }

        /**
        * @brief	Return the static event-driven window counter.
        * @return	unsigned int sEventDrivenWindowCount_
        */
        static unsigned int GetEventDrivenWindowCount() noexcept { return IWindow::sEventDrivenWindowCount_; }

        /**
        * @brief    Return a boolean value based on if any real-time windows exist.
        * @return   TRUE if real-time window count greater than 0, false otherwise.
        */
        static bool HasRealTimeWindow() noexcept { return IWindow::sRealTimeWindowCount_ > 0; }

        /**
        * @brief    Return a boolean value based on if any event-driven windows exist.
        * @return   TRUE if event-driven window count greater than 0, false otherwise.
        */
        static bool HasEventDrivenWindow() noexcept { return IWindow::sEventDrivenWindowCount_ > 0; }
    };
}; // end of namespace winxframe
#endif