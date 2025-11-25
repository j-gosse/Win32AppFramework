# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.4  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 25, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, window management, message loops, and error 
handling. Includes a suite for unit testing and various helper utils. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update

### November 25, 2025
#### <u>PRE-ALPHA v2.4</u>
##### &emsp;Additions:
- Implemented IWindow class. This is the base class that all windows inherit from, it stores the static WndProc() method. Moved storage of hWindow_, hInstance_, hAccelTable_, pumpMode_, sRealTimeWindowCount_, and sEventDrivenWindowCount_ from 
Window to IWindow. IWindow is now responsible for handling these variables and performing their cleanup, loading the accelerator table, registering the WNDCLASS, and window creation. However, most cleanup is still initialized by and occurs in 
derived window classes. In order to create a window, the IWindow Create() method is called in a derived window class, and accepts a reference to WNDCLASSEX struct. Adding this layer of abstraction to window creation will be helpful when making 
structural changes and additions to the window classes.  
- Added methods GetInstance(), GetAccelTable(), and IsWindow() to the IWindow class.  
- Added static functions CreateWindowClass() and UnregisterWindowClass() in win32_platform. This is a temporary means to generate WNDCLASS structs until a better method for creating, storing, and registering/unregistering WNDCLASS structs is 
implemented.  
##### &emsp;Changes:
- Window class now inherits from IWindow class.  
- Removed member variables hWindow_, hInstance_, and hAccelTable_, static member variables sRealTimeWindowCount_ and sEventDrivenWindowCount_, methods WndProc(), RegisterWindowClass(), UnregisterWindowClass(), BuildWindow(), GetWindowClassName(), 
SetWindowClassName(), GetWindowClass(), GetRealTimeWindowCount(), GetEventDrivenWindowCount(), IncrementWindowCount(), DecrementWindowCount(), GetWindow(), GetPumpMode(), and Cleanup() from the Window class. No longer necessary now that IWindow 
class handles most of this logic and WNDCLASS creation now occurs outside of any window classes.  
- Removed static member variables sWindowClassName_, sWindowClass_, and sIsClassRegistered_ from the Window class. WNDCLASS creation occurs elsewhere while class registration occurs in IWindow.  
- IWindow and Window classes now accept a parameter referencing a WNDCLASSEX structure.  

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  