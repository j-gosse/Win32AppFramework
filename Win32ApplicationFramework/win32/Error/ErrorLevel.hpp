/*!
win32\Error\ErrorLevel.hpp
Created: October 11, 2025
Updated: October 11, 2025
Copyright (c) 2025, Jacob Gosse

ErrorLevel header file.
*/

#pragma once

#ifndef ERRORLEVEL_HPP_
#define ERRORLEVEL_HPP_

enum class ErrorLevel
{
	Info,
	Warning,
	General,
	Critical,
	Fatal,
	Unknown
};

#endif