/*!
win32\Console\Console.cpp
Created: October 5, 2025
Updated: October 7, 2025
Copyright (c) 2025, Jacob Gosse

Console source file.
*/

#include "Console.hpp"
#include <win32/debug.h>

/* CONSTRUCTORS */

Console::Console() :
    m_hInstance(GetModuleHandleW(nullptr))
{
    OutputDebugStringW(L"CONSTRUCTOR: Console()\n");

    if (!m_consoleAllocated)
    {
        OutputDebugStringW(L"Failed to allocate console!");
        OutputDebugStringW(L"Attempting to attach console from parent process...");
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
        {
            throw std::runtime_error("Failed to attach console!");
        }
    }

    if (m_hConsoleWindow == nullptr)
    {
        throw std::runtime_error("Failed to retrieve the console window!");
    }

    Console::SetTitle(L"CONSOLE");
    Console::RedirectStdIO();
}

Console::Console(HINSTANCE hInstance) :
    m_hInstance(hInstance)
{
    OutputDebugStringW(L"CONSTRUCTOR: Console(HINSTANCE hInstance)\n");

    if (!m_consoleAllocated)
    {
        OutputDebugStringW(L"Failed to allocate console!");
        OutputDebugStringW(L"Attempting to attach console from parent process...");
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
        {
            throw std::runtime_error("Failed to attach console!");
        }
    }

    if (m_hConsoleWindow == nullptr)
    {
        throw std::runtime_error("Failed to retrieve the console window!");
    }

    Console::SetTitle(L"CONSOLE");
    Console::RedirectStdIO();
}

/* DESTRUCTOR */

Console::~Console()
{
    OutputDebugStringW(L"DESTRUCTOR: ~Console()\n");
    Console::Cleanup();
}

/* FUNCTION DEFINITIONS */

void Console::WriteOutput(std::span<CHAR_INFO> const& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const
{
    if (!WriteConsoleOutputW(m_hConsoleOutput, buffer.data(), bufferSize, bufferCoord, &writeRegion))
    {
        throw std::runtime_error("WriteConsoleOutputW failed.");
    }
}

void Console::ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const
{
    std::size_t newBufferSize = static_cast<std::size_t>(bufferSize.X) * bufferSize.Y;
    if (buffer.size() < newBufferSize) buffer.resize(newBufferSize);

    if (!ReadConsoleOutputW(m_hConsoleOutput, buffer.data(), bufferSize, bufferCoord, &readRegion))
    {
        throw std::runtime_error("ReadConsoleOutputW failed.");
    }
}

std::uint32_t Console::ReadInput(std::vector<INPUT_RECORD>& inputEvents, std::size_t maxEvents) const
{
    if (maxEvents == 0) throw std::invalid_argument("maxEvents must be > 0");
    if (inputEvents.capacity() < maxEvents) inputEvents.reserve(128);
    inputEvents.resize(maxEvents);

    DWORD numEventsRead{};
    if (!ReadConsoleInputW(m_hConsoleInput, inputEvents.data(), static_cast<DWORD>(inputEvents.size()), &numEventsRead))
    {
        throw std::runtime_error("ReadConsoleInputW failed with error : " + std::to_string(GetLastError()));
    }
    inputEvents.resize(numEventsRead);
    return numEventsRead;
}

void Console::GetScreenBufferInfoEx(CONSOLE_SCREEN_BUFFER_INFOEX& csbix) const
{
    csbix.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(m_hConsoleOutput, &csbix))
    {
        throw std::runtime_error("GetConsoleScreenBufferInfo failed.");
    }
}
CONSOLE_SCREEN_BUFFER_INFOEX Console::GetScreenBufferInfoEx() const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbix{};
    csbix.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    Console::GetScreenBufferInfoEx(csbix);
    return csbix;
}

const SHORT Console::GetScreenBufferWidth() const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbix = Console::GetScreenBufferInfoEx();
    return csbix.dwSize.X;

}
const SHORT Console::GetScreenBufferHeight() const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbix = Console::GetScreenBufferInfoEx();
    return csbix.dwSize.Y;
}

void Console::SetTitle(const wchar_t* title) const
{
    WCHAR titleBuffer[MAX_LOADSTRING]{};
    if (title && *title) wcsncpy_s(titleBuffer, title, MAX_LOADSTRING - 1);
    else LoadStringW(m_hInstance, IDS_CONSOLE_TITLE, titleBuffer, MAX_LOADSTRING);
    if (m_hConsoleWindow) SetConsoleTitleW(titleBuffer);
}

void Console::SetCursorPosition(COORD cursorPos) const
{
    if (!SetConsoleCursorPosition(m_hConsoleOutput, cursorPos))
    {
        throw std::runtime_error("SetConsoleCursorPosition failed.");
    }
}
COORD Console::GetCursorPosition() const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbix = Console::GetScreenBufferInfoEx();
    return csbix.dwCursorPosition;
}

void Console::SetTextAttributes(std::vector<CHAR_INFO>& buffer, WORD attribs) const
{
    for (CHAR_INFO& ch : buffer)
        ch.Attributes = attribs;
}
void Console::GetTextAttributes(WORD& attribs) const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbix{ 0 };
    Console::GetScreenBufferInfoEx(csbix);
    attribs = csbix.wAttributes;
}
WORD Console::GetTextAttributes() const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbix{ 0 };
    Console::GetScreenBufferInfoEx(csbix);
    return csbix.wAttributes;
}

void Console::InitBuffer(std::vector<CHAR_INFO>& buffer, std::wstring const& text) const
{
    buffer.resize(text.size());

    for (size_t i = 0; i < text.size(); ++i)
    {
        buffer[i] = { {.UnicodeChar = text[i] }, 0 };
    }
}

ConsoleWriteRegion Console::CreateWriteRegion(std::wstring const& text, COORD writePos, std::optional<WORD> attribs) const
{
    WORD attributes = attribs.value_or(Console::GetTextAttributes());

    ConsoleWriteRegion writeRegion;
    Console::InitBuffer(writeRegion.buffer, text);
    Console::SetTextAttributes(writeRegion.buffer, attributes);

    writeRegion.bufferSize = { static_cast<SHORT>(writeRegion.buffer.size()), 1 };
    writeRegion.bufferCoord = { 0, 0 };
    writeRegion.writeRegion =
    {
        writePos.X,
        writePos.Y,
        static_cast<SHORT>(writePos.X + writeRegion.bufferSize.X - 1),
        writePos.Y
    };

    return writeRegion;
}

void Console::WriteLineChunks(const std::wstring& line, COORD& cursorPos, WORD attribs, SHORT screenBufferWidth) const
{
    size_t chunkStartIndex = 0;
    size_t lineLength = line.size();

    while (chunkStartIndex < lineLength)
    {
        SHORT columnsRemaining = screenBufferWidth - cursorPos.X;
        size_t chunkLength = std::min<size_t>(lineLength - chunkStartIndex, columnsRemaining);
        std::wstring currentChunk = line.substr(chunkStartIndex, chunkLength);

        ConsoleWriteRegion writeRegion = CreateWriteRegion(currentChunk, cursorPos, attribs);
        WriteOutput(writeRegion.buffer, writeRegion.bufferSize, writeRegion.bufferCoord, writeRegion.writeRegion);

        chunkStartIndex += chunkLength;
        cursorPos.X += static_cast<SHORT>(chunkLength);

        if (cursorPos.X >= screenBufferWidth)
        {
            cursorPos.X = 0;
            ++cursorPos.Y;
        }
    }
}

void Console::WriteText(std::wstring const& text, std::optional<WORD> attribs) const
{
    if (text.empty())
        return;

    const SHORT screenBufferWidth = GetScreenBufferWidth();
    WORD attributes = attribs.value_or(GetTextAttributes());
    COORD cursor = GetCursorPosition();

    std::wstringstream ss(text);
    std::wstring line;

    while (std::getline(ss, line, L'\n'))
    {
        Console::WriteLineChunks(line, cursor, attributes, screenBufferWidth);

        cursor.X = 0;
        ++cursor.Y;
    }

    Console::SetCursorPosition(cursor);
}

const HANDLE Console::OutputHandle() const
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

    if (hOutput == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to create output handle!");
    }

    DWORD mode;
    if (!GetConsoleMode(hOutput, &mode))
    {
        throw std::runtime_error("GetConsoleMode failed. Console output handle is not valid.");
    }

    return hOutput;
}

const HANDLE Console::InputHandle() const
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

    if (hInput == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to create input handle!");
    }

    DWORD mode;
    if (!GetConsoleMode(hInput, &mode))
    {
        throw std::runtime_error("GetConsoleMode failed. Console input handle is not valid.");
    }

    return hInput;
}

const HANDLE Console::ErrorHandle() const
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

    if (hError == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to create error handle!");
    }

    DWORD mode;
    if (!GetConsoleMode(hError, &mode))
    {
        throw std::runtime_error("GetConsoleMode failed. Console error handle is not valid.");
    }

    return hError;
}

void Console::RedirectStdIO() const
{
    FILE* dummyStream = nullptr;

    freopen_s(&dummyStream, "CONOUT$", "w", stdout);
    std::wcout.clear();
    std::cout.clear();

    freopen_s(&dummyStream, "CONOUT$", "w", stderr);
    std::cerr.clear();

    freopen_s(&dummyStream, "CONIN$", "r", stdin);
    std::cin.clear();
}

void Console::Cleanup()
{
    if (m_hConsoleOutput && m_hConsoleOutput != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hConsoleOutput);
        m_hConsoleOutput = INVALID_HANDLE_VALUE;
    }

    if (m_hConsoleInput && m_hConsoleInput != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hConsoleInput);
        m_hConsoleInput = INVALID_HANDLE_VALUE;
    }

    if (m_hConsoleWindow != nullptr)
    {
        DestroyWindow(m_hConsoleWindow);
        m_hConsoleWindow = nullptr;
    }

    if (m_consoleAllocated) FreeConsole();
}