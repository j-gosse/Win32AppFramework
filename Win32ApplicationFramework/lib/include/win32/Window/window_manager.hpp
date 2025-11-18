/*!
lib\include\win32\Window\window_manager.hpp
Created: November 17, 2025
Updated: November 18, 2025
Copyright (c) 2025, Jacob Gosse

Window Manager header file.
*/

#pragma once

#ifndef WINDOW_MANAGER_HPP_
#define WINDOW_MANAGER_HPP_

#include <memory>
#include <vector>

namespace winxframe
{
    class Window;

    bool ManageWindows(std::vector<std::unique_ptr<Window>>& windows);

}; // end of namespace winxframe

#endif