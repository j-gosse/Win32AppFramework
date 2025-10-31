/*!
lib\include\utils\container_utils.hpp
Created: October 27, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

Container Utilities header file.
*/

#pragma once

#ifndef CONTAINER_UTILS_HPP_
#define CONTAINER_UTILS_HPP_

#include <map>
#include <unordered_map>
#include <vector>

namespace winxframe
{
    namespace container_utils
    {
    #pragma region Sorting
        template<typename T>
        [[nodiscard]] constexpr std::vector<typename T::key_type> SortedMapKeys(const T& map) noexcept
        {
            std::vector<typename T::key_type> keys;
            keys.reserve(map.size());
            for (const auto& [key, value] : map)
                keys.push_back(key);
            std::sort(keys.begin(), keys.end());
            return keys;
        }
    #pragma endregion

    #pragma region Map to Vector
        template<typename T>
        [[nodiscard]] constexpr auto FlattenMap(const T& map) noexcept
        {
            std::vector<typename T::mapped_type> result;
            for (const auto& [key, value] : map)
                result.push_back(value);
            return result;
        }

        template<typename T>
        [[nodiscard]] constexpr auto FlattenNestedMap(const T& outerMap) noexcept
        {
            std::vector<typename T::mapped_type::mapped_type::value_type> result;

            for (const auto& [outerKey, innerMap] : outerMap)
                for (const auto& [innerKey, vec] : innerMap)
                    result.insert(result.end(), vec.begin(), vec.end());

            return result;
        }
    #pragma endregion
    }; // end of namespace container_utils
}; // end of namespace winxframe

#endif