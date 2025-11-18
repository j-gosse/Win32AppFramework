/*!
lib\include\win32\Window\MessageLoopMode.hpp
Created: November 16, 2025
Updated: November 16, 2025
Copyright (c) 2025, Jacob Gosse

Message Loop Mode header file.
*/

#pragma once

#ifndef MESSAGELOOPMODE_HPP_
#define MESSAGELOOPMODE_HPP_

namespace winxframe
{
    enum struct MessageLoopMode
    {
        RealTime,   // PeekMessage-based engine loop
        EventDriven // GetMessage/WM_QUIT loop
    };
}; // end of namespace winxframe

#endif