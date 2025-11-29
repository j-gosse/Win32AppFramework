# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.5  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 29, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, window management, message loops, and error 
handling. Includes a suite for unit testing and various helper utils. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update

### November 29, 2025
#### <u>PRE-ALPHA v2.5</u>
##### &emsp;Additions:
- Added WindowClassRegistry header and source files. This class is responsible for creating, registering, storing, and unregistering WNDCLASSEX structures. References to the created classes are stored in an unordered map of type std\::wstring for key 
and type ATOM for value. An ATOM (type unsigned short) serves as a unique identifier for a string stored in an internal system hash table (also known as "atom table"). Included public methods CreateWindowClass(), RegisterWindowClass(), Unregister(), 
and UnregisterAll(). Window class registry should be created upon an application's initialization, and only after ALL of the application's windows have been destroyed would all registered window classes then be unregistered and the registry object 
destroyed. Default window class creation and registration is currently being performed in IWindow's Create() method, setting a default class name if the referenced class name passed to the method is empty. Included a std\::mutex variable to ensure 
thread-safe class registration in methods that write to or read from the unordered map registeredClasses_ member variable.  
- Added WindowCounter header file. This class declares the counter variables as inline static std\::atomic\<std\::uint32_t\> to ensure that window counters are global and shared across all window instances. It provides static getter methods for public 
access while keeping instance-level increment/decrement methods tied to the window lifecycle. The class supports multi-window concurrency and properly isolates the counter logic within its own class. WindowCounter is not inherited by IWindow, but rather 
a derived window class (in this case Window).  
##### &emsp;Changes:
- Modified Window class constructor to instead accept a reference to a WindowClassRegistry object, rather than a WNDCLASSEX struct. Applied the same change to IWindow class Create() method.  
- Removed RegisterWindowClass() method from IWindow. No longer necessary now that this is handled by WindowClassRegistry.  
- Changed Windows functions in win32 classes to use their macro counter-parts, instead of specifying "W" (Wide/Unicode) or "A" (Ansi). This is better for general use of either Unicode or Ansi preprocessor directives, allowing the macros to utilize 
the correct functions depending on project settings.  
- Removed IncrementWindowCount(), DecrementWindowCount(), GetRealTimeWindowCount(), GetEventDrivenWindowCount(), HasRealTimeWindow(), and HasEventDrivenWindow() from IWindow. No longer necessary now that this is handled by WindowCounter class.  
- Window class now uses WindowCounter's IncrementWindowCount() and DecrementWindowCount() methods in OnCreate and OnDestroy respectively. ManageWindows function in window_manager has also been adjusted to check the static HasRealTimeWindow() or 
HasEventDrivenWindow() methods instead.  
- Cleaned up various classes a bit more; refined existing doxygen descriptions.  
- Moved NOTICE.txt up one level in the folder structure.  

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  