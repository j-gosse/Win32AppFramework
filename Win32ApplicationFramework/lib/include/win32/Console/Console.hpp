/*!
lib\include\win32\Console\Console.hpp
Created: October 5, 2025
Updated: November 27, 2025
Copyright (c) 2025, Jacob Gosse

Console header file.
*/

#pragma once

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <win32/framework.h>
#include <win32/resource.h>
#include <span>

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
		bool isConsoleAllocated_;
		HWND hConsoleWindow_;

		HANDLE OutputHandle() const;
		HANDLE InputHandle() const;
		HANDLE ErrorHandle() const;
		HANDLE hConsoleOutput_;
		HANDLE hConsoleInput_;
		HANDLE hConsoleError_;

		HINSTANCE hInstance_;
		std::wstring consoleTitle_;

		static constexpr SHORT CONSOLE_COLUMNS = 120;
		static constexpr SHORT CONSOLE_ROWS = 30;
		static constexpr SHORT BUFFER_HEIGHT = 9001;
		SHORT consoleColumns_;
		SHORT maxConsoleColumns_;
		SHORT consoleRows_;
		SHORT maxConsoleRows_;

		bool isCleaned_ = false;

		void InitConsole(SHORT consoleWidth, SHORT consoleHeight);
		void InitBuffer(std::vector<CHAR_INFO>& buffer, const std::wstring& text) const noexcept;

		void WriteOutput(const std::span<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const;
		void ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const;
		std::uint32_t ReadInput(std::vector<INPUT_RECORD>& inputEvents, std::size_t maxEvents = 128) const;

		ConsoleWriteRegion CreateWriteRegion(const std::wstring& text, COORD writePos, WORD attribute) const noexcept;
		void WriteLineChunks(const std::wstring& line, COORD& cursorPos, WORD attribute, SHORT bufferWidth) const;
		void RedirectStdIO() const noexcept;

		void Cleanup() noexcept;

	public:
		Console(const std::wstring& consoleTitle = L"", SHORT consoleWidth = CONSOLE_COLUMNS, SHORT consoleHeight = CONSOLE_ROWS);
		Console(HINSTANCE hInstance, const std::wstring& consoleTitle = L"", SHORT consoleWidth = CONSOLE_COLUMNS, SHORT consoleHeight = CONSOLE_ROWS);
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;
		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;
		~Console();

		void WriteText(const std::wstring& text, WORD attribute = static_cast<WORD>(0x07)) const;
		void ResizeConsoleBuffer(SHORT bufferWidth = CONSOLE_COLUMNS, SHORT bufferHeight = BUFFER_HEIGHT) const;
		void ResizeConsole(SHORT consoleWidth = CONSOLE_COLUMNS, SHORT consoleHeight = CONSOLE_ROWS) const;
		void RepositionConsole(int leftX, int topY, int consoleWidth, int consoleHeight, UINT uFlags = 0) const noexcept;

		HWND GetConsole() const noexcept { return hConsoleWindow_; }
		bool IsConsoleAllocated() const noexcept { return isConsoleAllocated_; }

		HANDLE GetOutputHandle() const noexcept { return hConsoleOutput_; }
		void SetOutputHandle(HANDLE handle) noexcept { hConsoleOutput_ = handle; }
		HANDLE GetInputHandle() const noexcept { return hConsoleInput_; }
		void SetInputHandle(HANDLE handle) noexcept { hConsoleInput_ = handle; }
		HANDLE GetErrorHandle() const noexcept { return hConsoleError_; }
		void SetErrorHandle(HANDLE handle) noexcept { hConsoleError_ = handle; }

		CONSOLE_SCREEN_BUFFER_INFOEX GetScreenBufferInfo() const;
		SHORT GetScreenBufferWidth() const { return this->GetScreenBufferInfo().dwSize.X; }
		SHORT GetScreenBufferHeight() const { return this->GetScreenBufferInfo().dwSize.Y; }

		SHORT GetConsoleColumns() const noexcept { return consoleColumns_; }
		SHORT GetMaxConsoleColumns() const noexcept { return maxConsoleColumns_; }
		void SetConsoleColumns(SHORT columns) noexcept { consoleColumns_ = columns > maxConsoleColumns_ ? maxConsoleColumns_ : columns; }

		SHORT GetConsoleRows() const noexcept { return consoleRows_; }
		SHORT GetMaxConsoleRows() const noexcept { return maxConsoleRows_; }
		void SetConsoleRows(SHORT rows) noexcept { consoleRows_ = rows > maxConsoleRows_ ? maxConsoleRows_ : rows; }

		std::uint32_t GetInputCodePage() const noexcept { return GetConsoleCP(); }
		std::uint32_t GetOutputCodePage() const noexcept { return GetConsoleOutputCP(); }
		void SetInputCodePage(std::uint32_t wCodePageID) const noexcept { SetConsoleCP(wCodePageID); }
		void SetOutputCodePage(std::uint32_t wCodePageID) const noexcept { SetConsoleOutputCP(wCodePageID); }

		CONSOLE_CURSOR_INFO GetCursorInfo() const;
		void SetCursorInfo(const CONSOLE_CURSOR_INFO& cursorInfo) const noexcept { SetConsoleCursorInfo(hConsoleOutput_, &cursorInfo); }

		COORD GetCursorPosition() const { return this->GetScreenBufferInfo().dwCursorPosition; }
		void SetCursorPosition(COORD cursorPos) const noexcept { SetConsoleCursorPosition(hConsoleOutput_, cursorPos); }

		WORD GetTextAttributes() const { return this->GetScreenBufferInfo().wAttributes; }
		void SetTextAttributes(std::vector<CHAR_INFO>& buffer, WORD attributes) const noexcept { for (CHAR_INFO& ch : buffer) ch.Attributes = attributes; }

		const std::wstring& GetTitle() const noexcept { return consoleTitle_; }
		void SetTitle(const std::wstring& title = L"") noexcept;

		bool IsCleaned() const noexcept { return isCleaned_; }
	};
}; // end of namespace winxframe

#endif