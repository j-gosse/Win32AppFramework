/*!
lib\source\win32\Console\Console.cpp
Created: October 5, 2025
Updated: November 8, 2025
Copyright (c) 2025, Jacob Gosse

Console source file.
*/

#include <win32/Console/Console.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
    /* CONSTRUCTORS */

    Console::Console() :
        hInstance_(GetModuleHandleW(nullptr)), consoleTitle_(L"")
    {
        OutputDebugStringW(L"CONSTRUCTOR: Console()\n");
        this->InitConsole();
    }

    Console::Console(HINSTANCE hInstance) :
        hInstance_(hInstance), consoleTitle_(L"")
    {
        OutputDebugStringW(L"CONSTRUCTOR: Console(HINSTANCE hInstance)\n");
        this->InitConsole();
    }

    /* DESTRUCTOR */

    Console::~Console()
    {
        OutputDebugStringW(L"DESTRUCTOR: ~Console()\n");
        this->Cleanup();
    }

    /* FUNCTION DEFINITIONS */

    HANDLE Console::OutputHandle() const
    {
        HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

        hOutput = CreateFileW
        (
            L"CONOUT$",
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        THROW_IF_ERROR(hOutput == INVALID_HANDLE_VALUE);

        DWORD mode;
        THROW_IF_ERROR_CTX(!GetConsoleMode(hOutput, &mode), L"GetConsoleMode failed!");

        return hOutput;
    }

    HANDLE Console::InputHandle() const
    {
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

        hInput = CreateFileW
        (
            L"CONIN$",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        THROW_IF_ERROR(hInput == INVALID_HANDLE_VALUE);

        DWORD mode;
        THROW_IF_ERROR_CTX(!GetConsoleMode(hInput, &mode), L"GetConsoleMode failed!");

        return hInput;
    }

    HANDLE Console::ErrorHandle() const
    {
        HANDLE hError = GetStdHandle(STD_ERROR_HANDLE);

        hError = CreateFileW
        (
            L"CONOUT$",
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        THROW_IF_ERROR(hError == INVALID_HANDLE_VALUE);

        DWORD mode;
        THROW_IF_ERROR_CTX(!GetConsoleMode(hError, &mode), L"GetConsoleMode failed!");

        return hError;
    }

    void Console::InitConsole()
    {
        if (!this->IsConsoleAllocated())
        {
            OutputDebugStringW(L"Failed to allocate console!");
            OutputDebugStringW(L"Attempting to attach console from parent process...");
            THROW_IF_ERROR_CTX(!AttachConsole(ATTACH_PARENT_PROCESS), L"Failed to attach console from parent process!");
        }

        THROW_IF_ERROR_CTX(this->GetConsole() == nullptr, L"hConsoleWindow_ == nullptr");

        // set console title
        this->SetTitle(L"CONSOLE");
        SetConsoleTitleW(this->GetTitle().c_str());

        // redirect standard input/output streams
        this->RedirectStdIO();
    }

    void Console::InitBuffer(std::vector<CHAR_INFO>& buffer, const std::wstring& text) const noexcept
    {
        buffer.resize(text.size());
        for (size_t i = 0; i < text.size(); ++i)
            buffer[i] = { {.UnicodeChar = text[i] }, 0 };
    }

    void Console::WriteOutput(const std::span<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const
    {
        THROW_IF_ERROR_CTX(!WriteConsoleOutputW(this->GetOutputHandle(), buffer.data(), bufferSize, bufferCoord, &writeRegion), L"WriteConsoleOutputW failed!");
    }

    void Console::ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const
    {
        std::size_t newBufferSize = static_cast<std::size_t>(bufferSize.X) * bufferSize.Y;
        if (buffer.size() < newBufferSize) buffer.resize(newBufferSize);

        THROW_IF_ERROR_CTX(!ReadConsoleOutputW(this->GetOutputHandle(), buffer.data(), bufferSize, bufferCoord, &readRegion), L"ReadConsoleOutputW failed!");
    }

    std::uint32_t Console::ReadInput(std::vector<INPUT_RECORD>& inputEvents, std::size_t maxEvents) const
    {
        assert(maxEvents > 0 && "Console::ReadInput: maxEvents must be > 0");
        if (inputEvents.capacity() < maxEvents)
        {
            size_t eventsReserve = 128;
            inputEvents.reserve(eventsReserve);
        }
        inputEvents.resize(maxEvents);

        DWORD numEventsRead{};
        THROW_IF_ERROR_CTX(!ReadConsoleInputW(this->GetOutputHandle(), inputEvents.data(), static_cast<DWORD>(inputEvents.size()), &numEventsRead), L"ReadConsoleInputW failed!");

        inputEvents.resize(numEventsRead);
        return numEventsRead;
    }

    ConsoleWriteRegion Console::CreateWriteRegion(const std::wstring& text, const COORD writePos, const WORD attribute) const noexcept
    {
        ConsoleWriteRegion writeRegion;
        this->InitBuffer(writeRegion.buffer, text);
        this->SetTextAttributes(writeRegion.buffer, attribute);

        writeRegion.bufferSize = { static_cast<SHORT>(writeRegion.buffer.size()), 1 };
        writeRegion.bufferCoord = { 0, 0 };
        writeRegion.writeRegion = { writePos.X, writePos.Y, static_cast<SHORT>(writePos.X + writeRegion.bufferSize.X - 1), writePos.Y };

        return writeRegion;
    }

    void Console::WriteLineChunks(const std::wstring& line, COORD& cursorPos, const WORD attribute, const SHORT screenBufferWidth) const
    {
        size_t chunkStartIndex = 0;
        size_t lineLength = line.size();

        while (chunkStartIndex < lineLength)
        {
            SHORT columnsRemaining = screenBufferWidth - cursorPos.X;
            size_t chunkLength = std::min<size_t>(lineLength - chunkStartIndex, columnsRemaining);
            std::wstring currentChunk = line.substr(chunkStartIndex, chunkLength);

            ConsoleWriteRegion writeRegion = this->CreateWriteRegion(currentChunk, cursorPos, attribute);
            this->WriteOutput(writeRegion.buffer, writeRegion.bufferSize, writeRegion.bufferCoord, writeRegion.writeRegion);

            chunkStartIndex += chunkLength;
            cursorPos.X += static_cast<SHORT>(chunkLength);

            if (cursorPos.X >= screenBufferWidth)
            {
                cursorPos.X = 0;
                ++cursorPos.Y;
            }
        }
    }

    void Console::WriteText(const std::wstring& text, const WORD attribute) const
    {
        if (text.empty()) return;

        COORD cursor = this->GetCursorPosition();
        std::wstringstream ss(text);
        std::wstring line;

        while (std::getline(ss, line, L'\n'))
        {
            this->WriteLineChunks(line, cursor, attribute, this->GetScreenBufferWidth());
            cursor.X = 0;
            ++cursor.Y;
        }
        this->SetCursorPosition(cursor);
    }

    CONSOLE_SCREEN_BUFFER_INFOEX Console::GetScreenBufferInfo() const
    {
        CONSOLE_SCREEN_BUFFER_INFOEX csbix{};
        csbix.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        THROW_IF_ERROR_CTX(!GetConsoleScreenBufferInfoEx(this->GetOutputHandle(), &csbix), L"GetConsoleScreenBufferInfo failed!");
        return csbix;
    }

    CONSOLE_CURSOR_INFO Console::GetCursorInfo() const
    {
        CONSOLE_CURSOR_INFO cursorInfo{};
        THROW_IF_ERROR_CTX(!GetConsoleCursorInfo(this->GetOutputHandle(), &cursorInfo), L"GetConsoleCursorInfo failed!");
        return cursorInfo;
    }

    void Console::SetTitle(const std::wstring& title) noexcept
    {
        if (!title.empty()) consoleTitle_ = title;
        else
        {
            WCHAR titleBuffer[MAX_LOADSTRING]{};
            LoadStringW(this->GetInstance(), IDS_CONSOLE_TITLE, titleBuffer, MAX_LOADSTRING);
            consoleTitle_ = titleBuffer;
        }
    }

    void Console::RedirectStdIO() const noexcept
    {
        FILE* dummyStream = nullptr;

        freopen_s(&dummyStream, "CONOUT$", "w", stdout);
        std::cout.clear();
        std::wcout.clear();

        freopen_s(&dummyStream, "CONOUT$", "w", stderr);
        std::cerr.clear();
        std::wcerr.clear();

        freopen_s(&dummyStream, "CONIN$", "r", stdin);
        std::cin.clear();
    }

    void Console::Cleanup() noexcept
    {
        if (this->IsCleaned()) return;

        if (this->GetOutputHandle())
        {
            CloseHandle(this->GetOutputHandle());
            this->SetOutputHandle(INVALID_HANDLE_VALUE);
        }

        if (this->GetInputHandle())
        {
            CloseHandle(this->GetInputHandle());
            this->SetInputHandle(INVALID_HANDLE_VALUE);
        }

        if (this->GetErrorHandle())
        {
            CloseHandle(this->GetErrorHandle());
            this->SetErrorHandle(INVALID_HANDLE_VALUE);
        }

        if (this->GetConsole())
        {
            DestroyWindow(hConsoleWindow_);
            this->SetConsole(nullptr);
        }

        if (this->IsConsoleAllocated())
            FreeConsole();

        this->SetIsCleaned(true);
    }
}; // end of namespace winxframe