/*!
lib\source\win32\Console\Console.cpp
Created: October 5, 2025
Updated: November 27, 2025
Copyright (c) 2025, Jacob Gosse

Console source file.
*/

#include <win32/Console/Console.hpp>
#include <win32/Console/ConsoleColor.hpp>
#include <win32/Error/error_macros.hpp>

namespace winxframe
{
    /* CONSTRUCTORS */

    Console::Console(const std::wstring& consoleTitle, SHORT consoleWidth, SHORT consoleHeight)
        try :
        isConsoleAllocated_(AllocConsole()),
        hConsoleWindow_(GetConsoleWindow()),
        hConsoleOutput_(this->OutputHandle()),
        hConsoleInput_(this->InputHandle()),
        hConsoleError_(this->ErrorHandle()),
        hInstance_(GetModuleHandle(nullptr)),
        consoleTitle_(consoleTitle)
    {
        OutputDebugString(L"CONSTRUCTOR: Console(const std::wstring& title, SHORT consoleWidth, SHORT consoleHeight)\n");
        if (!isConsoleAllocated_)
            OutputDebugString(L"Failed to allocate console, will attempt to attach later.");
        this->InitConsole(consoleWidth, consoleHeight);
    }
    catch (const Error&)
    {
        RETHROW_ERROR_CTX(L"Rethrowing Console constructor error!");
    }

    Console::Console(HINSTANCE hInstance, const std::wstring& consoleTitle, SHORT consoleWidth, SHORT consoleHeight)
        try :
        isConsoleAllocated_(AllocConsole()),
        hConsoleWindow_(GetConsoleWindow()),
        hConsoleOutput_(this->OutputHandle()),
        hConsoleInput_(this->InputHandle()),
        hConsoleError_(this->ErrorHandle()),
        hInstance_(hInstance),
        consoleTitle_(consoleTitle)
    {
        OutputDebugString(L"CONSTRUCTOR: Console(HINSTANCE hInstance, const std::wstring& title, SHORT consoleWidth, SHORT consoleHeight)\n");
        if (!isConsoleAllocated_)
            OutputDebugString(L"Failed to allocate console, will attempt to attach later.");
        this->InitConsole(consoleWidth, consoleHeight);
    }
    catch (const Error&)
    {
        RETHROW_ERROR_CTX(L"Rethrowing Console constructor error!");
    }

    /* DESTRUCTOR */

    Console::~Console()
    {
        OutputDebugString(L"DESTRUCTOR: ~Console()\n");
        this->Cleanup();
    }

    /* FUNCTION DEFINITIONS */

    HANDLE Console::OutputHandle() const
    {
        HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

        hOutput = CreateFile
        (
            L"CONOUT$",
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        THROW_IF_ERROR_CTX(!hOutput, L"Invalid handle value for output handle!");

        DWORD mode;
        THROW_IF_ERROR_CTX(!GetConsoleMode(hOutput, &mode), L"GetConsoleMode failed!");

        return hOutput;
    }

    HANDLE Console::InputHandle() const
    {
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

        hInput = CreateFile
        (
            L"CONIN$",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        THROW_IF_ERROR_CTX(!hInput, L"Invalid handle value for input handle!");

        DWORD mode;
        THROW_IF_ERROR_CTX(!GetConsoleMode(hInput, &mode), L"GetConsoleMode failed!");

        return hInput;
    }

    HANDLE Console::ErrorHandle() const
    {
        HANDLE hError = GetStdHandle(STD_ERROR_HANDLE);

        hError = CreateFile
        (
            L"CONOUT$",
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        THROW_IF_ERROR_CTX(!hError, L"Invalid handle value for error handle!");

        DWORD mode;
        THROW_IF_ERROR_CTX(!GetConsoleMode(hError, &mode), L"GetConsoleMode failed!");

        return hError;
    }

    void Console::InitConsole(SHORT consoleWidth, SHORT consoleHeight)
    {
        if (!isConsoleAllocated_)
        {
            OutputDebugString(L"Failed to allocate console!");
            OutputDebugString(L"Attempting to attach console from parent process...");
            THROW_IF_ERROR_CTX(!AttachConsole(ATTACH_PARENT_PROCESS), L"Failed to attach console from parent process!");
        }

        THROW_IF_ERROR_CTX(!hConsoleWindow_, L"Console window handle is invalid!");

        // redirect standard input/output streams
        this->RedirectStdIO();

        // set console title
        this->SetTitle(this->GetTitle());

        // assign console sizes
        COORD largest = GetLargestConsoleWindowSize(hConsoleOutput_);
        maxConsoleColumns_ = largest.X;
        maxConsoleRows_ = largest.Y;
        this->SetConsoleColumns(consoleWidth);
        this->SetConsoleRows(consoleHeight);

        // set console size (buffer size must not be smaller than window size)
        this->ResizeConsoleBuffer(consoleColumns_, this->GetScreenBufferHeight());
        this->ResizeConsole(consoleColumns_, consoleRows_);

        // reposition console
        UINT uFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE;
        this->RepositionConsole(0, 0, 0, 0, uFlags);
    }

    void Console::InitBuffer(std::vector<CHAR_INFO>& buffer, const std::wstring& text) const noexcept
    {
        buffer.resize(text.size());
        for (size_t i = 0; i < text.size(); ++i)
            buffer[i] = CHAR_INFO{ .Char = {.UnicodeChar = text[i] }, .Attributes = 0 };
    }

    void Console::WriteOutput(const std::span<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& writeRegion) const
    {
        THROW_IF_ERROR_CTX(!WriteConsoleOutput(hConsoleOutput_, buffer.data(), bufferSize, bufferCoord, &writeRegion), L"WriteConsoleOutputW failed!");
    }

    void Console::ReadOutput(std::vector<CHAR_INFO>& buffer, COORD bufferSize, COORD bufferCoord, SMALL_RECT& readRegion) const
    {
        std::size_t newBufferSize = static_cast<std::size_t>(bufferSize.X) * bufferSize.Y;
        if (buffer.size() < newBufferSize)
            buffer.resize(newBufferSize);

        THROW_IF_ERROR_CTX(!ReadConsoleOutput(hConsoleOutput_, buffer.data(), bufferSize, bufferCoord, &readRegion), L"ReadConsoleOutputW failed!");
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
        THROW_IF_ERROR_CTX(!ReadConsoleInput(hConsoleOutput_, inputEvents.data(), static_cast<DWORD>(inputEvents.size()), &numEventsRead), L"ReadConsoleInputW failed!");

        inputEvents.resize(numEventsRead);
        return numEventsRead;
    }

    ConsoleWriteRegion Console::CreateWriteRegion(const std::wstring& text, COORD writePos, WORD attribute) const noexcept
    {
        ConsoleWriteRegion writeRegion;
        this->InitBuffer(writeRegion.buffer, text);
        this->SetTextAttributes(writeRegion.buffer, attribute);

        writeRegion.bufferSize = { static_cast<SHORT>(writeRegion.buffer.size()), 1 };
        writeRegion.bufferCoord = { 0, 0 };
        writeRegion.writeRegion = { writePos.X, writePos.Y, static_cast<SHORT>(writePos.X + writeRegion.bufferSize.X - 1), writePos.Y };
        return writeRegion;
    }

    void Console::WriteLineChunks(const std::wstring& line, COORD& cursorPos, WORD attribute, SHORT bufferWidth) const
    {
        size_t chunkStartIndex = 0;
        size_t lineLength = line.size();

        while (chunkStartIndex < lineLength)
        {
            SHORT columnsRemaining = bufferWidth - cursorPos.X;
            size_t chunkLength = std::min<size_t>(lineLength - chunkStartIndex, columnsRemaining);
            std::wstring currentChunk = line.substr(chunkStartIndex, chunkLength);

            ConsoleWriteRegion writeRegion = this->CreateWriteRegion(currentChunk, cursorPos, attribute);
            this->WriteOutput(writeRegion.buffer, writeRegion.bufferSize, writeRegion.bufferCoord, writeRegion.writeRegion);

            chunkStartIndex += chunkLength;
            cursorPos.X += static_cast<SHORT>(chunkLength);

            if (cursorPos.X >= bufferWidth)
            {
                cursorPos.X = 0;
                ++cursorPos.Y;
            }
        }
    }

    void Console::WriteText(const std::wstring& text, WORD attribute) const
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

    void Console::RepositionConsole(int leftX, int topY, int consoleWidth, int consoleHeight, UINT uFlags) const noexcept
    {
        SetWindowPos
        (
            hConsoleWindow_,
            HWND_TOP,
            leftX,
            topY,
            consoleWidth,
            consoleHeight,
            uFlags
        );
    }

    CONSOLE_SCREEN_BUFFER_INFOEX Console::GetScreenBufferInfo() const
    {
        CONSOLE_SCREEN_BUFFER_INFOEX csbix{};
        csbix.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        THROW_IF_ERROR_CTX(!GetConsoleScreenBufferInfoEx(hConsoleOutput_, &csbix), L"GetConsoleScreenBufferInfo failed!");
        return csbix;
    }

    CONSOLE_CURSOR_INFO Console::GetCursorInfo() const
    {
        CONSOLE_CURSOR_INFO cursorInfo{};
        THROW_IF_ERROR_CTX(!GetConsoleCursorInfo(hConsoleOutput_, &cursorInfo), L"GetConsoleCursorInfo failed!");
        return cursorInfo;
    }

    void Console::SetTitle(const std::wstring& title) noexcept
    {
        if (!title.empty())
            consoleTitle_ = title;
        else
        {
            WCHAR titleBuffer[MAX_LOADSTRING]{};
            LoadString(hInstance_, IDS_CONSOLE_TITLE, titleBuffer, MAX_LOADSTRING);
            consoleTitle_ = titleBuffer;
        }
        SetConsoleTitle(this->GetTitle().c_str());
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

    void Console::ResizeConsoleBuffer(SHORT bufferWidth, SHORT bufferHeight) const
    {
        assert(bufferWidth > 0 && bufferHeight > 0 && "Console::ResizeConsoleBuffer: Console Buffer width and height must be greater than 0.");
        if (bufferWidth < consoleColumns_)
            bufferWidth = consoleColumns_;
        if (bufferHeight < consoleRows_)
            bufferHeight = consoleRows_;

        COORD bufferSize = { bufferWidth, bufferHeight };
        if (!SetConsoleScreenBufferSize(hConsoleOutput_, bufferSize))
        {
            DWORD error = GetLastError();
            OutputDebugString((L"SetConsoleScreenBufferSize failed: " + std::to_wstring(error)).c_str());
            THROW_ERROR_CTX(L"SetConsoleScreenBufferSize failed!");
        }
    }

    void Console::ResizeConsole(SHORT consoleWidth, SHORT consoleHeight) const
    {
        assert(consoleWidth > 0 && consoleHeight > 0 && "Console::ResizeConsole: Console width and height must be greater than 0.");
        if (consoleWidth > maxConsoleColumns_)
            consoleWidth = maxConsoleColumns_;
        if (consoleHeight > maxConsoleRows_)
            consoleHeight = maxConsoleRows_;

        SMALL_RECT windowRect = { 0, 0, consoleWidth - 1, consoleHeight - 1 }; // subtract from width and height to ensure window sized less than buffer
        if (!SetConsoleWindowInfo(hConsoleOutput_, TRUE, &windowRect))
        {
            DWORD error = GetLastError();
            OutputDebugString((L"SetConsoleWindowInfo failed: " + std::to_wstring(error)).c_str());
            THROW_ERROR_CTX(L"SetConsoleWindowInfo failed!");
        }
    }

    void Console::Cleanup() noexcept
    {
        if (isCleaned_) return;

        if (hConsoleOutput_)
        {
            CloseHandle(hConsoleOutput_);
            hConsoleOutput_ = INVALID_HANDLE_VALUE;
        }

        if (hConsoleInput_)
        {
            CloseHandle(hConsoleInput_);
            hConsoleInput_ = INVALID_HANDLE_VALUE;
        }

        if (hConsoleError_)
        {
            CloseHandle(hConsoleError_);
            hConsoleError_ = INVALID_HANDLE_VALUE;
        }

        if (hConsoleWindow_)
        {
            DestroyWindow(hConsoleWindow_);
            hConsoleWindow_ = nullptr;
        }

        if (isConsoleAllocated_)
            FreeConsole();

        isCleaned_ = true;
    }
}; // end of namespace winxframe