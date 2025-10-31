/*!
lib\include\utils\time_utils.hpp
Created: October 27, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

Time Utilities header file.
*/

#pragma once

#ifndef TIME_UTILS_HPP_
#define TIME_UTILS_HPP_

#include <chrono>
#include <string>

namespace winxframe
{
    namespace time_utils
    {
    #pragma region Time Conversions
        struct double_time
        {
            double time;
            std::string unit;
        };

        [[nodiscard]] constexpr double_time ConvertToDouble(std::chrono::nanoseconds elapsed) noexcept
        {
            const char* units[] = { "ns", "us", "ms", "s", "min" };
            double time = static_cast<double>(elapsed.count());
            int unitIndex = 0;

            while (time >= 1000.0 && unitIndex < 4)
            {
                time /= 1000.0;
                ++unitIndex;

                if (unitIndex == 3 && time >= 60.0)
                {
                    time /= 60.0;
                    ++unitIndex;
                    break;
                }
            }
            return { time, units[unitIndex] };
        }
    #pragma endregion
    }; // end of namespace time_utils
}; // end of namespace winxframe

#endif