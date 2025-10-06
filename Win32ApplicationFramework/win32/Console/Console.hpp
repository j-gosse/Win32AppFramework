/*!
win32\Console\Console.hpp
Created: October 5, 2025
Updated: October 5, 2025
Copyright (c) 2025, Jacob Gosse

Console header file.
*/

#pragma once

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <win32/framework.h>
#include <win32/resource.h>

class Console
{
private:
	bool m_consoleAllocated;
	HWND m_hConsoleWindow;
	HANDLE m_hConsoleOutput;
	HANDLE m_hConsoleInput;
	HINSTANCE m_hInstance;

	static constexpr const int MAX_LOADSTRING = 100;
	WCHAR m_consoleTitle[MAX_LOADSTRING];

	static constexpr const LONG CONSOLE_WIDTH = 800;
	static constexpr const LONG CONSOLE_HEIGHT = 600;

	void InitConsole();
	void CreateOutputHandle();
	void CreateInputHandle();
	void RedirectStdIO();
	void Cleanup();

public:
	Console();
	Console(HINSTANCE hInstance);
	virtual ~Console();
};

#endif