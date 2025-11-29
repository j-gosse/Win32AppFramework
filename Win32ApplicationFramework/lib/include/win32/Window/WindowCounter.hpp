/*!
lib\include\win32\Window\WindowCounter.hpp
Created: November 29, 2025
Updated: November 29, 2025
Copyright (c) 2025, Jacob Gosse

Window Counter header file.
*/

#pragma once

#ifndef WINDOWCOUNTER_HPP_
#define WINDOWCOUNTER_HPP_

#include "MessagePumpMode.hpp"
#include <atomic>
#include <cstdint>
#include <iostream>

namespace winxframe
{
    class WindowCounter
    {
    private:
        inline static std::atomic<std::uint32_t> realTimeWindowCount_;
        inline static std::atomic<std::uint32_t> eventDrivenWindowCount_;

    protected:
        /**
        * @brief	Increment the real-time or event-driven window count based on current window pump mode.
        */
        void IncrementWindowCount(MessagePumpMode mode) noexcept
        {
            if (mode == MessagePumpMode::RealTime)
                realTimeWindowCount_.fetch_add(1, std::memory_order_relaxed);
            else if (mode == MessagePumpMode::EventDriven)
                eventDrivenWindowCount_.fetch_add(1, std::memory_order_relaxed);
        }

        /**
        * @brief	Decrement real-time or event-driven window count based on current window pump mode.
        */
        void DecrementWindowCount(MessagePumpMode mode) noexcept
        {
            if (mode == MessagePumpMode::RealTime)
                realTimeWindowCount_.fetch_sub(1, std::memory_order_relaxed);
            else if (mode == MessagePumpMode::EventDriven)
                eventDrivenWindowCount_.fetch_sub(1, std::memory_order_relaxed);
        }

    public:
        WindowCounter() { std::wcout << L"CONSTRUCTOR: WindowCounter()\n"; }
        ~WindowCounter() { std::wcout << L"DESTRUCTOR: ~WindowCounter()\n"; }

        /**
        * @brief	Return the static real-time window counter.
        * @return	uint32_t sRealTimeWindowCount_
        */
        static std::uint32_t GetRealTimeWindowCount() noexcept { return realTimeWindowCount_; }

        /**
        * @brief	Return the static event-driven window counter.
        * @return	uint32_t sEventDrivenWindowCount_
        */
        static std::uint32_t GetEventDrivenWindowCount() noexcept { return eventDrivenWindowCount_; }

        /**
        * @brief    Return a static boolean value based on if any real-time windows exist.
        * @return   TRUE if real-time window count greater than 0, false otherwise.
        */
        static bool HasRealTimeWindow() noexcept { return realTimeWindowCount_.load() > 0; }

        /**
        * @brief    Return a static boolean value based on if any event-driven windows exist.
        * @return   TRUE if event-driven window count greater than 0, false otherwise.
        */
        static bool HasEventDrivenWindow() noexcept { return eventDrivenWindowCount_.load() > 0; }
    };
}; // end of namespace winxframe
#endif