/*!
win32\Console\Console.hpp
Created: October 5, 2025
Updated: October 7, 2025
Copyright (c) 2025, Jacob Gosse

Console header file.
*/

#pragma once

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <win32/framework.h>
#include <win32/resource.h>
#include <span> // requires C++20
#include <optional>

struct ConsoleWriteRegion
{
	std::vector<CHAR_INFO> buffer{};
	COORD bufferSize = { 0, 1 };
	COORD bufferCoord = { 0, 0 };
	SMALL_RECT writeRegion = { 0, 0, 0, 0 };
};

class Console
{
private:
	HINSTANCE m_hInstance;

	bool m_consoleAllocated = AllocConsole();
	HWND m_hConsoleWindow = GetConsoleWindow();

	const HANDLE OutputHandle() const;
	const HANDLE InputHandle() const;
	const HANDLE ErrorHandle() const;
	HANDLE m_hConsoleOutput = OutputHandle();
	HANDLE m_hConsoleInput = InputHandle();
	HANDLE m_hConsoleError = ErrorHandle();

	static constexpr const size_t MAX_LOADSTRING = 100;
	static constexpr const LONG CONSOLE_WIDTH = 800;
	static constexpr const LONG CONSOLE_HEIGHT = 600;

	void RedirectStdIO() const;
	void Cleanup();

public:
	Console();
	Console(HINSTANCE hInstance);
	virtual ~Console();

	void WriteOutput(std::span<CHAR_INFO> const& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const;
	void ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const;
	std::uint32_t ReadInput(std::vector<INPUT_RECORD>& inputEvents, std::size_t maxEvents = 128) const;

	void GetScreenBufferInfoEx(CONSOLE_SCREEN_BUFFER_INFOEX& csbix) const;
	CONSOLE_SCREEN_BUFFER_INFOEX GetScreenBufferInfoEx() const;

	const SHORT GetScreenBufferWidth() const;
	const SHORT GetScreenBufferHeight() const;

	void SetTitle(const wchar_t* title = nullptr) const;

	void SetCursorPosition(COORD cursorPos) const;
	COORD GetCursorPosition() const;

	void SetTextAttributes(std::vector<CHAR_INFO>& buffer, WORD attribs) const;
	void GetTextAttributes(WORD& attribs) const;
	WORD GetTextAttributes() const;

	void InitBuffer(std::vector<CHAR_INFO>& buffer, std::wstring const& text) const;
	ConsoleWriteRegion CreateWriteRegion(std::wstring const& text, COORD writePos, std::optional<WORD> attribs = std::nullopt) const;
	void WriteLineChunks(const std::wstring& line, COORD& cursorPos, WORD attribs, SHORT screenBufferWidth) const;
	void WriteText(std::wstring const& text, std::optional<WORD> attribs = std::nullopt) const;
};

#endif