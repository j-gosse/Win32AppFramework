# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.1  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 18, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, message loops, and error handling. 
Includes a suite for unit testing and various helper utilities. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update

### November 18, 2025
#### <u>PRE-ALPHA v2.1</u>
##### &emsp;Additions:
- Added SetWindowSize method to Window class. Sets the window width and the window height.  
- Added GetMessages method to Window class. Runs event driven message processing. This is in contrast to PeekMessages which is used for real-time event processing.  
- Removed static window count. Added static event-driven and static real-time window counts with getters.  
- Added FPS member variable to Window class. Included public getter/setter methods.  
- Added Window Manager header and source file. Implemented the ManageWindows() function. Returns a boolean value; true if windows vector has active windows, false if the vector is empty. Parameter accepts a reference to a vector of 
unique pointers to windows. This function begins by ensuring the vector is not empty, initializes time values, checks if any real-time windows exist, and if so runs the real-time window loop, otherwise runs an event-driven loop. The 
elapsed time, delta time, and frame rate are being calculated in the real-time windows loop, event-driven loop only performs message pumping and window removal. In both loops, the windows are essentially managed using the same process. 
Messages are pumped from all windows and each window is checked to see if it has been destroyed. If the window handle is invalid, it is removed from the vector and unique_ptr automatically calls the destructor for that window. The real-time 
loop then sets elapsed time, current fps, and performs update/render methods for each real-time window. Finally, the frame rate is appropriately limited based on a target fps. For the purpose of simplifying message processing in the real-time 
loop, event-driven windows are treated as real-time and utilize PeekMessages() instead of GetMessages() for message retrieval. Only when no real-time windows exist do event-driven run solely using the event-driven loop. Returning a boolean 
allows for running a while loop in wWinMain entry based on whether any active windows remain, could then perform further tasks here.  
- Included a Frame Limiter section in win32_utils header file. Added HybridVSyncFrameLimiter() and HighPrecisionFrameLimiter() functions. As the name implies, HybridVSyncFrameLimiter combines sleeping and busy-waiting to achieve a precise 
target frame rate. This technique aims to balance the CPU efficiency of sleeping with the timing accuracy of busy-waiting, effectively mitigating the imprecision of std\::this_thread\::sleep_for. This loop is CPU-intensive but provides the 
highest possible precision, as the thread remains active and reacts immediately when the target time is reached. HighPrecisionFrameLimiter is a more complex implementation that uses a static absolute time tracker to prevent frame-time drift 
over long periods. For its waiting mechanism, it uses cooperative yielding (std\::this_thread\::yield()) for the final milliseconds instead of busy-waiting. This is less precise than active waiting but more efficient in terms of overall CPU 
utilization, as it avoids burning CPU cycles in a tight loop.  
- "Winmm.lib" added to project additional dependencies (Linker -> Input -> Additional Dependencies). Necessary for the use of Windows functions timeBeginPeriod() and timeEndPeriod(). The timeBeginPeriod function requests a minimum resolution 
for periodic timers, timeEndPeriod clears the previously set resolution. Must match each call to timeBeginPeriod with a call to timeEndPeriod, specifying the same minimum resolution in both calls.  
- Added hwndFPSLabel_ and hwndFPSValue_ member variables to the Window class. Temporarily rendering the updated fps value here. Created in WM_CREATE.  
##### &emsp;Changes:
- Refactored Window class cleanup process further. Window (HWND) related resources are cleaned and released in the message handler during the process of destroying the window, while the destructor handles cleanup of the remaining resources 
that do not depend on the window handle being valid. Should an exception occur during the construction process, the catch block begins the process of destroying the window and performing general cleanup instead. The destructor will also start 
this process if for some reason the window handle is still valid by the time the destructor is called, after which general cleanup is performed. Under normal circumstances the cleanup pattern that occurs is the following:  
	1. User closes the window by means of WM_CLOSE (X button) or IDM_EXIT (File -> Exit) and if confirmed, DestroyWindow() is called on the window handle to begin the process of destroying the window.  
	2. Calling DestroyWindow() sets off the window destroy process, which first sends the message WM_DESTROY in which the window handle is still valid. This is typically the place to clean dynamically allocated resources and those that 
	depend on having a valid window handle. HWND-dependent resources are cleaned and released. End message handling for WM_DESTROY with PostQuitMessage(0) only when the LAST window is destroyed.  
	3. WM_NCDESTROY is sent following WM_DESTROY, after any child windows have been destroyed. This is the final place to do any internal window cleanup. Usually the only things left to do here are to clear the data in WindowLongPtr 
	(SetWindowLongPtrW(hWindow_, GWLP_USERDATA, 0) function) and set the window handle to nullptr.  
	4. When the window handle is eventually set to null, the message loop would detect that the handle is no longer valid for the window, and then it is removed (erased) from the vector calling the window's destructor. Of course, 
	this depends on the fact that the windows are being stored in the vector as unique pointers, which will automatically call the destructor for the window when the unique pointer is removed from the vector.  

	This pattern mostly complies with the standard RAII protocol, slightly adapted for the cleanup handling of Windows API internal resources.  
- Renamed Window class method ProcessMessages to PeekMessages to more accurately reflects its usage.  

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  