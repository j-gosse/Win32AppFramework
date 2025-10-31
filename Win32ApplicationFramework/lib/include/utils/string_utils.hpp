/*!
lib\include\utils\string_utils.hpp
Created: October 12, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

String Utilities header file.
*/

#pragma once

#ifndef STRING_UTILS_HPP_
#define STRING_UTILS_HPP_

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>

namespace winxframe
{
    namespace string_utils
    {
    #pragma region String Manipulation
        [[nodiscard]] constexpr std::string CenterText(const std::string& text, int width, char fill = ' ') noexcept
        {
            int padTotal = width - static_cast<int>(text.size());
            if (padTotal <= 0) return text;
            int padLeft = padTotal / 2;
            int padRight = padTotal - padLeft;
            return std::string(padLeft, fill) + text + std::string(padRight, fill);
        }
    #pragma endregion

    #pragma region String Conversions
        [[nodiscard]] constexpr std::wstring ToWide(std::string_view string) noexcept
        {
            if (string.empty()) return L"NULL";
            const int wideLength = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
            if (wideLength <= 0) return L"conversion to utf16 char failed";
            std::wstring wideString(static_cast<std::size_t>(wideLength), L'\0');
            MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), wideString.data(), wideLength);
            return wideString;
        }
        [[nodiscard]] constexpr std::wstring ToWide(const char* string) noexcept
        {
            return ToWide(std::string_view(string));
        }
        [[nodiscard]] constexpr std::wstring ToWide(const std::string& string) noexcept
        {
            return ToWide(std::string_view(string));
        }

        [[nodiscard]] constexpr std::string ToNarrow(std::wstring_view wideString) noexcept
        {
            if (wideString.empty()) return "NULL";
            const int narrowLength = WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), nullptr, 0, nullptr, nullptr);
            if (narrowLength <= 0) return "conversion to utf8 char failed";
            std::string narrowString(static_cast<std::size_t>(narrowLength), '\0');
            WideCharToMultiByte(CP_UTF8, 0, wideString.data(), static_cast<int>(wideString.size()), narrowString.data(), narrowLength, nullptr, nullptr);
            return narrowString;
        }
        [[nodiscard]] constexpr std::string ToNarrow(const wchar_t* wideString) noexcept
        {
            return ToNarrow(std::wstring_view(wideString));
        }
        [[nodiscard]] constexpr std::string ToNarrow(const std::wstring& wideString) noexcept
        {
            return ToNarrow(std::wstring_view(wideString));
        }
    #pragma endregion
    }; // end of namespace string_utils
}; // end of namespace winxframe

#endif