/*!
lib\include\win32\Window\MessagePumpMode.hpp
Created: November 16, 2025
Updated: November 16, 2025
Copyright (c) 2025, Jacob Gosse

Message Pump Mode header file.
*/

#pragma once

#ifndef MESSAGEPUMPMODE_HPP_
#define MESSAGEPUMPMODE_HPP_

namespace winxframe
{
    enum struct MessagePumpMode
    {
        RealTime,   // PeekMessage real-time loop
        EventDriven // GetMessage event-driven loop
    };
}; // end of namespace winxframe

#endif