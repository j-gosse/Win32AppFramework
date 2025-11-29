/*!
lib\include\win32\Window\WindowClassRegistry.hpp
Created: November 28, 2025
Updated: November 29, 2025
Copyright (c) 2025, Jacob Gosse

Window Class Registry header file.
*/

#pragma once

#ifndef WINDOWCLASSREGISTRY_HPP_
#define WINDOWCLASSREGISTRY_HPP_

#include <win32/resource.h>
#include <windows.h>
#include <unordered_map>
#include <string>
#include <mutex>

namespace winxframe
{
    class WindowClassRegistry
    {
    private:
        std::unordered_map<std::wstring, ATOM> registeredClasses_;
        mutable std::mutex mutex_;

    public:
        /**
        * @brief	Default WindowClassRegistry Constructor.
        */
        WindowClassRegistry();

        /**
        * @brief	WindowClassRegistry Destructor.
        */
        ~WindowClassRegistry() noexcept;

        /**
        * @brief    Create the WNDCLASSEX structure.
        * @param	HINSTANCE hInstance             : A handle to the window instance module.
        * @param    WNDPROC wndProc                 : Window Procedure static callback function pointer.
        * @param    const std::wstring& className   : Name by which the registered class is referenced.
        * @param    int extraClassBytes             : Number of extra bytes to allocate following the window-class structure.
        * @param    int extraWindowBytes            : Number of extra bytes to allocate following the window instance.
        */
        WNDCLASSEX CreateWindowClass(HINSTANCE hInstance, WNDPROC wndProc, const std::wstring& className, int extraClassBytes = 0, int extraWindowBytes = 0);

        /**
        * @brief	Register the WNDCLASSEX structure referenced by the parameter.
        * @param    WNDCLASSEX& wcex : Reference to the WNDCLASSEX structure.
        * @return	ATOM (type unsigned short) serves as a unique identifier for a string stored in an internal system hash table. Returns 0 on failure, 1 if class already registered.
        */
        ATOM RegisterWindowClass(WNDCLASSEX& wcex);

        /**
        * @brief	Unregister the specified window class.
        * @param	HINSTANCE hInstance             : A handle to the window instance module.
        * @param    const std::wstring& className   : Name by which the registered class is referenced when stored in the registeredClasses_ unordered map.
        */
        void Unregister(HINSTANCE hInstance, const std::wstring& className);

        /**
        * @brief	Unregister all window classes stored in the registeredClasses_ unordered map.
        * @param	HINSTANCE hInstance : A handle to the window instance module.
        */
        void UnregisterAll(HINSTANCE hInstance);
    };
}; // end of namespace winxframe
#endif