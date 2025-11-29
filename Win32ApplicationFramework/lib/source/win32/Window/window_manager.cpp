/*!
lib\source\win32\Window\window_manager.cpp
Created: November 17, 2025
Updated: November 29, 2025
Copyright (c) 2025, Jacob Gosse

Window Manager source file.
*/

#include <win32/Window/window_manager.hpp>
#include <win32/Window/Window.hpp>
#include <win32/utils/win32_utils.hpp>

namespace winxframe
{
    bool ManageWindows(std::vector<std::unique_ptr<Window>>& windows)
    {
        if (windows.empty())
            return false;

        static std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
        static std::chrono::high_resolution_clock::time_point previousTime = startTime;
        std::chrono::high_resolution_clock::time_point currentTime;

        static std::chrono::nanoseconds elapsedTime{};
        std::chrono::nanoseconds deltaTime{}, frameDuration{}, sleepTime{};
        const std::chrono::nanoseconds targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS

        double fps = 0.0;
        static bool firstFrame = true; // skip FPS calculation on first frame, persists across calls

        if (WindowCounter::HasRealTimeWindow())
        {
            std::chrono::high_resolution_clock::time_point frameStart = std::chrono::high_resolution_clock::now();

            deltaTime = frameStart - previousTime;
            previousTime = frameStart;
            elapsedTime = frameStart - startTime;

            if (!firstFrame && deltaTime.count() > 0)
                fps = 1.0 / (deltaTime.count() * 1e-9);
            firstFrame = false;

            //std::cout << "deltaTime ns: " << deltaTime.count() << " | FPS: " << fps << "\n";

            // pump messages for all windows
            for (auto window = windows.begin(); window != windows.end();)
            {
                (*window)->PeekMessages(); // non-blocking

                // remove destroyed windows
                if (!(*window)->IsWindow())
                    window = windows.erase(window);
                else
                {
                    // update/render only real-time windows
                    if ((*window)->GetPumpMode() == MessagePumpMode::RealTime)
                    {
                        (*window)->SetElapsed(elapsedTime);
                        (*window)->SetFPS(fps);
                        (*window)->Update(deltaTime);
                        (*window)->Render();
                    }
                    ++window;
                }
            }

            // limit frame rate
            //win32_utils::HybridVSyncFrameLimiter(targetFrameTime, frameStart);
            win32_utils::HighPrecisionFrameLimiter(targetFrameTime);
        }
        else if (WindowCounter::HasEventDrivenWindow())
        {
            // pump messages for all event-driven windows with call to the first
            if (windows.front()->GetMessages()) // blocking
            {
                std::cout << "Destroying windows...\n";
            }

            // remove destroyed windows
            for (auto window = windows.begin(); window != windows.end();)
            {
                if (!(*window)->IsWindow())
                    window = windows.erase(window);
                else
                    ++window;
            }
        }

        return !windows.empty(); // return true if windows remain
    }
}; // end of namespace winxframe