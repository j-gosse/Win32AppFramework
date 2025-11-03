/*!
lib\include\win32\Console\ConsoleColor.hpp
Created: November 1, 2025
Updated: November 1, 2025
Copyright (c) 2025, Jacob Gosse

Console Color header file.
*/

#pragma once

#ifndef CONSOLECOLOR_HPP_
#define CONSOLECOLOR_HPP_

#include <windows.h>
#include <ostream>
#include <cstdint>

namespace winxframe
{
    class ConsoleColor
    {
    private:
        uint8_t foreground_;
        uint8_t background_;

    public:
        constexpr ConsoleColor(uint8_t fg = ConsoleColor::WHITE, uint8_t bg = ConsoleColor::BLACK) 
            : foreground_(fg), background_(bg) {}

        constexpr WORD ToWord() const { return (WORD)(foreground_ | (background_ << 4)); }

        static constexpr WORD BLACK = 0;
        static constexpr WORD BLUE = 1;
        static constexpr WORD GREEN = 2;
        static constexpr WORD CYAN = 3;
        static constexpr WORD RED = 4;
        static constexpr WORD MAGENTA = 5;
        static constexpr WORD YELLOW = 6;
        static constexpr WORD WHITE = 7;
        static constexpr WORD BRIGHT = 8;

        static constexpr WORD BRIGHT_BLACK = BLACK | BRIGHT;
        static constexpr WORD BRIGHT_BLUE = BLUE | BRIGHT;
        static constexpr WORD BRIGHT_GREEN = GREEN | BRIGHT;
        static constexpr WORD BRIGHT_CYAN = CYAN | BRIGHT;
        static constexpr WORD BRIGHT_RED = RED | BRIGHT;
        static constexpr WORD BRIGHT_MAGENTA = MAGENTA | BRIGHT;
        static constexpr WORD BRIGHT_YELLOW = YELLOW | BRIGHT;
        static constexpr WORD BRIGHT_WHITE = WHITE | BRIGHT;
    };

    namespace console_color
    {
        constexpr ConsoleColor Default(ConsoleColor::WHITE, ConsoleColor::BLACK);
        constexpr ConsoleColor BrightWhite(ConsoleColor::BRIGHT_WHITE, ConsoleColor::BLACK);

        constexpr ConsoleColor WhiteOnRed(ConsoleColor::BRIGHT_WHITE, ConsoleColor::RED);
        constexpr WORD WHITE_ON_RED = WhiteOnRed.ToWord();

        constexpr ConsoleColor WhiteOnGreen(ConsoleColor::BRIGHT_WHITE, ConsoleColor::GREEN);
        constexpr WORD WHITE_ON_GREEN = WhiteOnGreen.ToWord();
    }

    // output operator
    inline std::ostream& operator<<(std::ostream& os, const ConsoleColor& color)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color.ToWord());
        return os;
    }

}; // end of namespace winxframe

#endif