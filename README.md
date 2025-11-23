# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.3  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 22, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, window management, message loops, and error 
handling. Includes a suite for unit testing and various helper utils. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update

### November 22, 2025
#### <u>PRE-ALPHA v2.3</u>
##### &emsp;Additions:
- Added int showCmd_ member variable to the Window class. Utilized in setting a window's startup properties.  
- Added ResolveShowCmd() utility function to win32_utils. Determines the effective show command for the window, based on if system startup information has set STARTF_USESHOWWINDOW flag or nCmdShow equal to SW_SHOWDEFAULT. Otherwise, 
it returns the provided nCmdShow parameter.  
- Added ShowAndUpdateWindow() protected method to the Window class. Sets the window's show state. Updates the client area of the specified window by sending a WM_PAINT message to the window if the window's update region is not empty.  
- Added virtual methods OnCreate() and OnDestroy() to the window class. These functions are intended to encapsulate any necessary window creation or destruction logic that applies to all windows, and to allow for derived classes to 
override them in order to include their own creation/destruction logic that runs separately from the base class. For example, a derived class might use OnCreate() to create child windows, modal dialogs, buttons, etc., that are specific 
to the window being created with the derived class, but should not be included with every window created by the base class.  
- Added GetWindowWidth() and GetWindowHeight() public methods to the Window class which return the full width and height of the window inclusive of non-client area.  
- Added GetWindowSize() public method to the Window class. Retrieves the window width and window height inclusive of non-client area and assigns the values to the referenced outWidth and outHeight parameters.  
- Window class ShowAndUpdateWindow() method now has a parameter that accepts a show command (nCmdShow) to allow for updating the window display after creation if necessary. Defaults to showCmd_ member variable if parameter unassigned.  
##### &emsp;Changes:
- Removed startupInfo_ member variable, InitStartupInfo(), SetStartupInfo(), and StartupInfo() from the Window class. Unnecessary as this information should be obtained prior to the class finalizing construction of a window object.  
- Removed processInfo_ and systemInfo_ member variables from the Window class. Neither is relevant to the window itself, and can be queried at startup or in the case of PROCESS_INFO, managed elsewhere.  
- Removed SysInfo() and ProcessInfo() methods from the Window class. Moved to win32_utils header file.  
- Modified Window constructors to include a parameter for passing nCmdShow.  
- Window class HandleMessage() method is now a virtual protected method to allow derived classes to override message handling with their own logic.  
- Added WM_CREATE case to Window class HandleMessage() method. WM_CREATE calls (and returns) OnCreate() virtual method. OnDestroy() virtual method is now called within (and acts as the return value for) WM_DESTROY, and handles the 
HWND-dependent cleanup and decrementing the static real-time/event-driven window counters.  
- Removed desktopWidth_ and desktopHeight_ member variables and associated getters from the Window class. These values can be obtained at any point through the use of such Windows functions as GetSystemMetrics().  
- Moved assignment of the window class name to the RegisterWindowClass() method.  
- In the Window class InitWindow() method, logic that occurs up to and including the creation of the window remains in this method while the rest has been moved to the OnCreate() method. Other than SetWindowTitle() which now occurs 
after the window has been created instead of before and thus is also moved to OnCreate().  
- Renamed windowWidth_ and windowHeight_ Window class member variables to screenWidth_ and screenHeight_. Clarifies that the values represent the window client drawing area, and not the entire window size. Renamed associated getters/
setters as well.  
- Renamed Window class WINDOW_WIDTH and WINDOW_HEIGHT constants to DEFAULT_SCREEN_WIDTH and DEFAULT_SCREEN_HEIGHT.  

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  