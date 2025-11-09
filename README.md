# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 1.9  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 8, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, message loops, and error handling. 
Includes a suite for unit testing and various helper utilities. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update
### November 8, 2025
#### <u>PRE-ALPHA v1.9</u>
##### &emsp;Additions:
- Added several member variable getters and setters to Console, Window, and TestSuite classes.  
- Console class copy and move constructors and assignment operators are now being deleted to prevent unsafe copy and ownership transfers.  
- InitConsole method added to Console class.  
- Moved TestRegistry class destructor cleanup to a Cleanup method.  
- Added a member variable to Window class for raw input device storage. Getter and setter included as well as raw input device cleanup.  
- Added HidUsage.hpp header file. Contains HidUsage enum structs for defining HID pages and usage ids for registering raw input devices.  
##### &emsp;Changes:
- Tidied up code further in various files, also replaced error throws with Error class macro usage.  
- Modified all class method calls where necessary to call by instance (this->GetWindow()) rather than by static call (Window::GetWindow()).  
- Changed Error class accessors to return by reference where necessary.  
- Wrapped multi-line Error class macros with "do {} while (0)" to ensure they resolve to a single statement.  
- Changed LOG_IF_ERROR and THROW_IF_ERROR Error class macros to log/throw if the result is true rather than false. Seems more logical to check results such as "THROW_IF_ERROR(hFile == INVALID_HANDLE_VALUE);" for example.  
- Switched windowTitle_, windowClassName_ (Window.hpp) and consoleTitle_ (Console.hpp) from C-style wide character arrays to std::wstring. Modified SetClassName in Window and SetTitle in both to account for this change.  
- Elapsed time in the Window class is now handled using type std::chrono::nanoseconds rather than ULONGLONG.  
- RegisterRawInput in the Window class now registers mutliple raw input devices using a custom enum types initializer list and stores them in a member variable.  

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  