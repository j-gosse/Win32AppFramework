/*!
lib\include\win32\Console\Console.hpp
Created: October 5, 2025
Updated: November 2, 2025
Copyright (c) 2025, Jacob Gosse

Console header file.
*/

#pragma once

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include "ConsoleColor.hpp"
#include <win32/framework.h>
#include <win32/resource.h>
#include <span>
#include <optional>

namespace winxframe
{
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
		HINSTANCE hInstance_;

		bool consoleAllocated_ = AllocConsole();
		HWND hConsoleWindow_ = GetConsoleWindow();

		const HANDLE OutputHandle() const;
		const HANDLE InputHandle() const;
		const HANDLE ErrorHandle() const;
		HANDLE hConsoleOutput_ = OutputHandle();
		HANDLE hConsoleInput_ = InputHandle();
		HANDLE hConsoleError_ = ErrorHandle();

		static constexpr const size_t MAX_LOADSTRING = 100;

		void Init() const;
		void Cleanup();
		bool isCleaned_ = false;

	public:
		Console();
		Console(HINSTANCE hInstance);
		~Console();

		LONG CONSOLE_WIDTH = 800;
		LONG CONSOLE_HEIGHT = 600;

		void WriteOutput(const std::span<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const;
		void ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const;
		std::uint32_t ReadInput(std::vector<INPUT_RECORD>& inputEvents, std::size_t maxEvents = 128) const;

		void InitBuffer(std::vector<CHAR_INFO>& buffer, const std::wstring& text) const;
		ConsoleWriteRegion CreateWriteRegion(const std::wstring& text, COORD writePos, std::optional<WORD> attribs = std::nullopt) const;
		void WriteLineChunks(const std::wstring& line, COORD& cursorPos, WORD attribs, SHORT screenBufferWidth) const;
		void WriteText(const std::wstring& text, std::optional<WORD> attribs = std::nullopt) const;
		void RedirectStdIO() const;

		HANDLE GetOutputHandle() const { return hConsoleOutput_; }
		HANDLE GetInputHandle() const { return hConsoleInput_; }
		HANDLE GetErrorHandle() const { return hConsoleError_; }

		CONSOLE_SCREEN_BUFFER_INFOEX GetScreenBufferInfo() const;

		const SHORT GetScreenBufferWidth() const { return Console::GetScreenBufferInfo().dwSize.X; }
		const SHORT GetScreenBufferHeight() const { return Console::GetScreenBufferInfo().dwSize.Y; }

		COORD GetCursorPosition() const { return Console::GetScreenBufferInfo().dwCursorPosition; }
		void SetCursorPosition(COORD cursorPos) const { SetConsoleCursorPosition(GetOutputHandle(), cursorPos); }

		WORD GetTextAttributes() const { return Console::GetScreenBufferInfo().wAttributes; }
		void SetTextAttributes(std::vector<CHAR_INFO>& buffer, WORD attribs) const { for (CHAR_INFO& ch : buffer) ch.Attributes = attribs; }

		void SetTitle(const wchar_t* title = nullptr) const;
	};

}; // end of namespace winxframe

#endif