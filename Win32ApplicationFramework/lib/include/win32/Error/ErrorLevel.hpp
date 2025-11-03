/*!
lib\include\win32\Error\ErrorLevel.hpp
Created: October 11, 2025
Updated: November 1, 2025
Copyright (c) 2025, Jacob Gosse

Error Level header file.
*/

#pragma once

#ifndef ERRORLEVEL_HPP_
#define ERRORLEVEL_HPP_

namespace winxframe
{
	enum struct ErrorLevel
	{
		Info,
		Warning,
		General,
		Critical,
		Fatal,
		Unknown
	};
}; // end of namespace winxframe

#endif