/*! \project    Win32ApplicationFramework
    \file       win32\win32_platform.cpp
    \version    PRE-ALPHA 1.2
    \desc	    A lightweight C++ framework that encapsulates core Win32 API functionality
                for window creation, message loops, error handling, and optional console 
                output. Ideal as a foundation for GUI apps, game engines, debug utils, etc.
    \author     Jacob Gosse
    \date       October 5, 2025
    \updated    October 9, 2025

    \MSVC       /std:c++20
    \GNUC       -m64 -std=c++20

    Copyright (c) 2025, Jacob Gosse

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at:

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <win32/Console/Console.hpp>
#include <win32/Error/Error.hpp>
#include <win32/Window/Window.hpp>
#include <win32/debug.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        #if defined(_DEBUG) && defined(_WIN32)
        ENABLE_CRT_LEAK_CHECKING;
        ENABLE_CRT_DELAY_FREE_MEM;
        #endif

        // init console
        std::unique_ptr<Console, std::default_delete<Console>> console = std::make_unique<Console>(hInstance);

        // console test
        console->WriteText(L"This is a really really really really really really really really long line that if necessary will be written across multiple lines based on the width of the console buffer. Does not wrap words.");
        console->WriteText(L"gggg", static_cast<WORD>(FOREGROUND_RED));
        console->WriteText(L"1111");
        console->WriteText(L"gggg");
        console->WriteText(L"gggg\n\n\n\n");
        console->WriteText(L"gggg", static_cast<WORD>(0x0003));
        console->WriteText(L"gggg");
        console->WriteText(L"1111\n");
        console->WriteText(L"gggg\n");

        // error test
        //SetLastError(ERROR_ACCESS_DENIED);
        //throw Error(__FILE__, __func__, __LINE__);
        //throw Error(__FILE__, __func__, __LINE__, L"Simulated test error");

        // init window
        std::unique_ptr<Window, std::default_delete<Window>> window = std::make_unique<Window>(hInstance);

        // main loop
        UINT wMsgFilterMin = 0;
        UINT wMsgFilterMax = 0;
        while (window->ProcessMessages(wMsgFilterMin, wMsgFilterMax))
        {
            //std::wcout << window->GetElapsed() << L'\n';

            /*
            This is where the main loop logic or call to a update/render loop occurs.
            For example:
                obj->Update(window->GetWindow(), elapsedTime);
                obj->Render(window->GetWindow());
            */

            Sleep(16);  // simulate ~60 FPS
        }

        window.reset();
        window = nullptr;

        std::wcout << L"\nProgram finished. Press any key to continue..." << std::endl;
        _getch(); // waits for a single character input

        console.reset();
        console = nullptr;

        OutputDebugStringW(L"Program finished.\n");

        return 0;
    }
    catch (const Error& e)
    {
        CRT_DUMP_MEMORY_LEAKS;

        e.MsgBox();
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        CRT_DUMP_MEMORY_LEAKS;

        std::cerr << "Error (wWinMain) caught std::exception: " << e.what() << std::endl;
        OutputDebugStringA(e.what());
        MessageBoxExA(nullptr, e.what(), "Fatal Error!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
        return EXIT_FAILURE;
    }
}