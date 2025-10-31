/*!
lib\include\utils\stream_utils.hpp
Created: October 30, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

Stream Utilities header file.
*/

#pragma once

#ifndef STREAM_UTILS_HPP_
#define STREAM_UTILS_HPP_

#include <utils/math_utils.hpp>
#include <ios>

namespace winxframe
{
	namespace stream_utils
	{
    #pragma region Max Stream Size
        [[nodiscard]] constexpr std::streamsize MaxStreamSize(size_t currentValue, std::streamsize maxValue) noexcept
        {
            std::streamsize streamSize = math_utils::Max(maxValue, static_cast<std::streamsize>(currentValue));
            return streamSize;
        }

        [[nodiscard]] constexpr std::streamsize MaxStreamSize(double currentValue, std::streamsize maxValue, int precision = 1) noexcept
        {
            double absValue = math_utils::Abs(currentValue);
            int integerDigits = 1;
            double v = absValue;
            while (v >= 10.0)
            {
                v /= 10.0;
                ++integerDigits;
            }

            std::streamsize value = integerDigits + (1 + static_cast<std::streamsize>(precision)) + ((currentValue < 0.0) ? 1 : 0);
            std::streamsize streamSize = math_utils::Max(maxValue, value);

            return streamSize;
        }
    #pragma endregion
	}; // end of namespace stream_utils
}; // end of namespace

#endif