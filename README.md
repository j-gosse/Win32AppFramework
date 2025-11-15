# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.0  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 15, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, message loops, and error handling. 
Includes a suite for unit testing and various helper utilities. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update

### November 15, 2025
#### <u>PRE-ALPHA v2.0</u>
##### &emsp;Additions:
- Window class WNDCLASSEX member variable and window class name are now managed statically. Added static variables to keep track of class registration and window count. Windows are registered with the same static WNDCLASSEX object.  
- Added public static method UnregisterWindowClass(). Window class WNDCLASSEX static member variable is unregistered after all windows have been closed and destroyed.  
- Added virtual Update and Render methods to the Window class. Update method currently accepts a parameter of nanoseconds delta time.  
- Delta time is now being calculated alongside elapsed time in the wWinMain entry (win32_platform) message loop. Framerate is being limited properly instead of using Sleep().  
- ResizeConsoleBuffer, ResizeConsole, and RepositionConsole methods added to Console Class.  
- Added member variables to Console class for column and row size, and max column and row size. Used in determining the console size. Getters/Setters included.  
- Added std::mutex member variable and MsgBoxSync/MsgBoxAsync methods to the Error class. The purpose of these methods is to allow for thread-safe message box handling when an error is caught. MsgBoxSync shows the message box and 
pauses program execution until the message box is dismissed. MsgBoxAsync shows the message box without blocking program execution. Both methods use a mutex to ensure only one message box appears at a time, even if multiple errors 
occur concurrently.  
- Added try/catch blocks to Window class constructors, Console class constructors and TestRegistry/TestCase constructors.  
##### &emsp;Changes:
- Cleaned up Console, Window, Error, TestSuite classes a bit more.  
- Window class has been made polymorphic, and windows are now constructed through derived classes. When a window is closed, cleanup occurs and the window's internal resources are released. In the WM_NCDESTROY message handler, 
the WindowLongPtr and window handle are set to nullptr. This allows the main message loop to detect that the window has been destroyed, after which it is removed from the windows vector. Since each window is managed by a unique_ptr, 
removing it from the vector automatically destroys the unique_ptr, which in turn calls the window’s destructor, ensuring proper cleanup.  
- Windows created with the Window class can now run simultaneously, utilizing virtual Update and Render methods in order to override them in the derived class for varied update/render logic per window if necessary.  
- Window class destructor is now virtual.  
- The wWinMain entry point (win32_platform) now stores a vector of the windows created. The main message loop runs as long as there are active windows in the vector. In each iteration, window messages are processed for each window, 
closed windows are detected and removed from the vector (which automatically destroys them via unique_ptr), and the update/render logic occurs for each remaining active window. Sorting logic may be added later to ensure the first 
window processed is the currently active window, or the last active window if none are currently active.  
- Window class ProcessMessages() method reconfigured to simply be a void function and run the message processing, rather than returning boolean value false when WM_QUIT is received. Cleanup and window destruction occur in the 
HandleMessage() method.  
- Window class default and HINSTANCE constructors now accept a title, window width, and window height. Size defaults at 800x600 if no parameters are assigned.  
- Changed Window class window width and window height member variables to type LONG. Getters/setters modified accordingly.  
- Added UINT uFlags parameter to Window class RepositionWindow method.  
- Console class constructors now accept a title, and a width and height for sizing the console window. Console is prevented from being sized larger than the values returned by GetLargestConsoleWindowSize.  
- Renamed local variable in Error class LOG_ERROR macros from 'error' to be a little more unique to prevent local variable naming conflicts.  
- Restructured error handling in the wWinMain entry (win32_platform) to centralize exception management and preserve the cause chain. Entire constructor bodies, including initializer lists, can be wrapped in try/catch blocks that perform 
local cleanup and rethrow exceptions thrown during construction. These rethrown exceptions are then caught by the outermost wWinMain catch block, which displays a message box and logs the full error and cause chain to the console window and 
debug output, ensuring consistent and maintainable exception handling. Any objects not created directly in wWinMain that may throw would require their own local try/catch handling. May also be necessary to implement RAII resource management 
in cases where resources cannot be properly cleaned and/or released when the object goes out of scope to due an exception.  

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  