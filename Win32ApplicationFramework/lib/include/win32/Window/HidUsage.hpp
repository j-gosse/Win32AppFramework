/*!
lib\include\win32\Window\HidUsage.hpp
Created: November 8, 2025
Updated: November 8, 2025
Copyright (c) 2025, Jacob Gosse

HID Usage header file.
*/

#pragma once

#ifndef HIDUSAGE_HPP_
#define HIDUSAGE_HPP_

#include <windows.h>

namespace winxframe
{
    enum struct HidUsagePage : USHORT
    {
        Undefined = 0x00,
        GenericDesktopControls = 0x01,
        SimulationControls = 0x02,
        Keyboard = 0x07,
        LEDs = 0x08,
        Button = 0x09,
        Consumer = 0x0C
    };

    enum struct HidUsageId : USHORT
    {
        Pointer = 0x01,
        Mouse = 0x02,
        Joystick = 0x04,
        GamePad = 0x05,
        Keyboard = 0x06,
        Keypad = 0x07,
        MultiAxisController = 0x08
    };
}; // end of namespace winxframe

#endif