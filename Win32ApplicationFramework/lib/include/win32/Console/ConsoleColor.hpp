/*!
lib\include\win32\Console\ConsoleColor.hpp
Created: November 1, 2025
Updated: November 5, 2025
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
    namespace console_color
    {
        // console color palette
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

        class ConsoleColor
        {
        private:
            uint8_t foreground_;
            uint8_t background_;

        public:
            constexpr ConsoleColor(uint8_t fg = WHITE, uint8_t bg = BLACK)
                : foreground_(fg), background_(bg) {}

            constexpr WORD ToWord() const { return (WORD)(foreground_ | (background_ << 4)); }
        };

        // constructed colors
        constexpr ConsoleColor Default(WHITE, BLACK);
        constexpr ConsoleColor BrightWhite(BRIGHT_WHITE, BLACK);

        constexpr ConsoleColor WhiteOnRed(BRIGHT_WHITE, RED);
        constexpr WORD WHITE_ON_RED = WhiteOnRed.ToWord();

        constexpr ConsoleColor WhiteOnGreen(BRIGHT_WHITE, GREEN);
        constexpr WORD WHITE_ON_GREEN = WhiteOnGreen.ToWord();
    }

    // output operator
    inline std::ostream& operator<<(std::ostream& os, const console_color::ConsoleColor& color)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color.ToWord());
        return os;
    }

}; // end of namespace winxframe

#endif