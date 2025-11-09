/*!
lib\include\win32\Console\Console.hpp
Created: October 5, 2025
Updated: November 8, 2025
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
		std::wstring consoleTitle_;

		bool consoleAllocated_ = AllocConsole();
		HWND hConsoleWindow_ = GetConsoleWindow();

		HANDLE hConsoleOutput_ = this->OutputHandle();
		HANDLE hConsoleInput_ = this->InputHandle();
		HANDLE hConsoleError_ = this->ErrorHandle();

		const WORD CONSOLE_WIDTH = 800;
		const WORD CONSOLE_HEIGHT = 600;

		bool isCleaned_ = false;

		HANDLE OutputHandle() const;
		HANDLE InputHandle() const;
		HANDLE ErrorHandle() const;

		void InitConsole();
		void InitBuffer(std::vector<CHAR_INFO>& buffer, const std::wstring& text) const noexcept;

		void WriteOutput(const std::span<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const;
		void ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const;
		std::uint32_t ReadInput(std::vector<INPUT_RECORD>& inputEvents, std::size_t maxEvents = 128) const;

		ConsoleWriteRegion CreateWriteRegion(const std::wstring& text, const COORD writePos, const WORD attribute) const noexcept;
		void WriteLineChunks(const std::wstring& line, COORD& cursorPos, const WORD attribute, const SHORT screenBufferWidth) const;
		void RedirectStdIO() const noexcept;

		void Cleanup() noexcept;

	public:
		Console();
		Console(HINSTANCE hInstance);
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;
		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;
		~Console();

		void WriteText(const std::wstring& text, const WORD attribute = console_color::WHITE) const;

		HWND GetConsole() const noexcept { return hConsoleWindow_; }
		void SetConsole(const HWND hWnd) noexcept { hConsoleWindow_ = hWnd; }
		HINSTANCE GetInstance() const noexcept { return hInstance_; }

		HANDLE GetOutputHandle() const noexcept { return hConsoleOutput_; }
		void SetOutputHandle(const HANDLE handle) noexcept { hConsoleOutput_ = handle; }
		HANDLE GetInputHandle() const noexcept { return hConsoleInput_; }
		void SetInputHandle(const HANDLE handle) noexcept { hConsoleInput_ = handle; }
		HANDLE GetErrorHandle() const noexcept { return hConsoleError_; }
		void SetErrorHandle(const HANDLE handle) noexcept { hConsoleError_ = handle; }

		CONSOLE_SCREEN_BUFFER_INFOEX GetScreenBufferInfo() const;

		std::uint32_t GetInputCodePage() const noexcept { return GetConsoleCP(); }
		std::uint32_t GetOutputCodePage() const noexcept { return GetConsoleOutputCP(); }
		void SetInputCodePage(std::uint32_t wCodePageID) const noexcept { SetConsoleCP(wCodePageID); }
		void SetOutputCodePage(std::uint32_t wCodePageID) const noexcept { SetConsoleOutputCP(wCodePageID); }

		SHORT GetScreenBufferWidth() const { return this->GetScreenBufferInfo().dwSize.X; }
		SHORT GetScreenBufferHeight() const { return this->GetScreenBufferInfo().dwSize.Y; }

		CONSOLE_CURSOR_INFO GetCursorInfo() const;
		void SetCursorInfo(const CONSOLE_CURSOR_INFO& cursorInfo) const noexcept { SetConsoleCursorInfo(this->GetOutputHandle(), &cursorInfo); }

		COORD GetCursorPosition() const { return this->GetScreenBufferInfo().dwCursorPosition; }
		void SetCursorPosition(const COORD& cursorPos) const noexcept { SetConsoleCursorPosition(this->GetOutputHandle(), cursorPos); }

		WORD GetTextAttributes() const { return this->GetScreenBufferInfo().wAttributes; }
		void SetTextAttributes(std::vector<CHAR_INFO>& buffer, WORD attributes) const noexcept { for (CHAR_INFO& ch : buffer) ch.Attributes = attributes; }

		const std::wstring& GetTitle() const noexcept { return consoleTitle_; }
		void SetTitle(const std::wstring& title = L"") noexcept;

		bool IsConsoleAllocated() const noexcept { return consoleAllocated_; }

		bool IsCleaned() const noexcept { return isCleaned_; }
		void SetIsCleaned(const bool isCleaned) noexcept { isCleaned_ = isCleaned; }
	};
}; // end of namespace winxframe

#endif