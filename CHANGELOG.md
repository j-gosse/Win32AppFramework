# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.4  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 25, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

## CHANGELOG

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
- Modified IWindow class static WndProc method. Removed the try/catch blocks and stopped throwing here, instead returning FALSE if "pInstance" is null. The exception should be thrown on the Create() window method in order to appropriately catch 
errors caused by invalid or null "lpCreateParams" when assigned to "pInstance" in the WndProc method.  

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

### November 20, 2025
#### <u>PRE-ALPHA v2.2</u>
##### &emsp;Additions:
- Added member variables hMemoryDC_, hMemoryBitmap_, and hOldMemoryBitmap_ to the Window class.  
	1. hMemoryDC_ is the handle to the memory device context used for off-screen rendering.  
	2. hMemoryBitmap_ is the bitmap that serves as the backbuffer, created to be compatible with the window’s device context.  
	3. hOldMemoryBitmap_ stores the original bitmap selected into the memory DC before the backbuffer was selected, allowing it to be restored during cleanup.  
	
	Together, these variables implement the rendering buffer for the window (the backbuffer) using a classic Win32 double-buffering technique. This separates what is visible to the user (front buffer) 
	from what is actually being drawn (back buffer). All rendering is performed on the memory bitmap first, and then the completed frame is copied to the window in a single BitBlt call.  

	By assembling the frame off-screen and presenting it in one operation, this approach helps to avoid issues such as flickering, tearing, partially drawn frames, and incorrect draw ordering.  
	
	This implementation serves as a temporary rendering solution and will likely be adapted or replaced in the future for the use of a GPU-accelerated rendering backend (e.g., DirectX, Vulkan, OpenGL, or a modern graphics framework).  
- Window class private method CreateMemoryBitmap() added. Checks for existing memory bitmap and destroys, sets the memory bitmap device context handle, creates the memory bitmap buffer and assigns the initialized bitmap object to the 
old bitmap member variable.  
- Window class private method ClearMemoryBitmapBuffer(). Clears the memory bitmap buffer with the specified parameter color, defaults to black.  
- Window class private method DestroyMemoryBitmap() added. Releases and deletes the memory bitmap and memory bitmap device context handle.  
- Added IncrementWindowCount() and DecrementWindowCount() methods to the Window class. Performs incrementing and decrementing the static real-time and event-driven window counters.  
- Added section to win32_utils header file for Rendering.  
- Created RenderFPS() function in win32_utils header file Rendering section. Accepts the device context handle for the memory bitmap and the fps value. Utilizes basic Windows API rendering methods to draw the FPS to the top-left corner 
of a real-time window.  
- Window class protected method BeginFrame() added. Accepts a color to clear the memory bitmap buffer with; calls ClearMemoryBitmapBuffer(). Prepares the memory bitmap buffer for a new frame.  
- Window class protected method Present() added. Copies the memory bitmap buffer to the window's device context.  
- Public getter for hMemoryDC (the memory bitmap device context handle) added to the Window class.  
- Protected getter for hMemoryBitmap added to the Window class.  
##### &emsp;Changes:
- Cleaned up the Window class further. Re-organized source file in to sections using pragma region. Placed methods that serve similar purposes in the same sections, for example all methods associated with window creation are now defined 
together in the "Window Creation" section.  
- Removed window labels associated with FPS rendering.  
- Included IncrementWindowCount() at the end of the InitWindow() method.  
- Included DestroyMemoryBitmap() and DecrementWindowCount() in the CleanupWindowResources() method which occurs in WM_DESTROY message handling.  
- The static window counters for real-time and event-driven are now only ever incremented at the end of InitWindow(), and decremented at the end of CleanupWindowResources().  
- Replaced SetRawInputDevices() and its overload with a template method that instead uses std\::forward. If an rvalue (temporary vector) is passed, the contents will be moved. If an lvalue is passed, the contents will be copied.  
- Render() method in derived Window test class "TestWindow" is now making use of BeginFrame(), RenderFPS(), and Present() to render the window.  

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

### November 15, 2025
#### <u>PRE-ALPHA v2.0</u>
##### &emsp;Additions:
- Window class WNDCLASSEX member variable and window class name are now managed statically. Added static variables to keep track of class registration and window count. Windows are registered with the same static WNDCLASSEX object.  
- Added public static method UnregisterWindowClass(). Window class WNDCLASSEX static member variable is unregistered after all windows have been closed and destroyed.  
- Added virtual Update and Render methods to the Window class. Update method currently accepts a parameter of nanoseconds delta time.  
- Delta time is now being calculated alongside elapsed time in the wWinMain entry (win32_platform) message loop. Framerate is being limited properly instead of using Sleep().  
- ResizeConsoleBuffer, ResizeConsole, and RepositionConsole methods added to Console Class.  
- Added member variables to Console class for column and row size, and max column and row size. Used in determining the console size. Getters/Setters included.  
- Added std\::mutex member variable and MsgBoxSync/MsgBoxAsync methods to the Error class. The purpose of these methods is to allow for thread-safe message box handling when an error is caught. MsgBoxSync shows the message box and 
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
- Restructured error handling in the wWinMain entry (win32_platform) to centralize exception management and preserve the cause chain. Entire constructor bodies, including initializer lists, can be wrapped 
in try/catch blocks that perform local cleanup and rethrow exceptions thrown during construction. These rethrown exceptions are then caught by the outermost wWinMain catch block, which displays a message 
box and logs the full error and cause chain to the console and debug output, ensuring consistent and maintainable exception handling. Objects not created directly in wWinMain that may throw require their 
own local try/catch blocks. In cases where the destructor alone cannot reliably clean up resources, such as HWND dependent resources or other API managed handles, RAII-inspired helper methods may need to 
be implemented to ensure proper and deterministic resource release.  

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
- Modified all class method calls where necessary to call by instance (this->GetWindow()) rather than by static call (Window\::GetWindow()).  
- Changed Error class accessors to return by reference where necessary.  
- Wrapped multi-line Error class macros with "do {} while (0)" to ensure they resolve to a single statement.  
- Changed LOG_IF_ERROR and THROW_IF_ERROR Error class macros to log/throw if the result is true rather than false. Seems more logical to check results such as "THROW_IF_ERROR(hFile == INVALID_HANDLE_VALUE);" for example.  
- Switched windowTitle_, windowClassName_ (Window.hpp) and consoleTitle_ (Console.hpp) from C-style wide character arrays to std\::wstring. Modified SetClassName in Window and SetTitle in both to account for this change.  
- Elapsed time in the Window class is now handled using type std\::chrono\::nanoseconds rather than ULONGLONG.  
- RegisterRawInput in the Window class now registers mutliple raw input devices using a custom enum types initializer list and stores them in a member variable.  

### November 2, 2025
#### <u>PRE-ALPHA v1.8</u>
##### &emsp;Additions:
- Console Color class header added. Implemented console colors.  
- Console Color initializes a fixed (static) palette of 8 basic colors and their 8 brighter counter-parts (16 colors in total that the console is able to use). These can be utilized in writes directly to the console.  
- In order to utilize colors when writing to an output stream such as std\::cout, Console Color is used to construct an object with a foreground and background color (type unsigned char) that can then be called during the write to an output stream or a stringstream.  
- The foreground and background member variables allow for overlapping color outputs such as white text on a green or red background, or whichever other combinations of the 16 colors.  
- Added Changelog.  
##### &emsp;Changes:
- Modified the Check functions and test macros in TestSuite so that they output the correct file and line locations when a check fails to pass.  
- Tidied up some code in various files.  
- Updated Changelog.  
- Updated ReadMe.  

### October 30, 2025
#### <u>PRE-ALPHA v1.7</u>
##### &emsp;Additions:
- Created a testing suite for running unit tests. Structure and utilization is based on how testing is handled in frameworks such as Boost, Doctest, Catch2, etc. Includes macros for ease of usage.  
- Testing suite outputs a report summary that is dynamically formatted as a cleanly organized table with detailed results inclusive of the running time for each test case and total running time. Checks that fail to pass a test also have their own separate output relevant to the failing test.  
- Added several helper utility headers. Many utilities that have been included in the headers are now being utilized to enhance code clarity, refine mathematical conversions, and improve compilation/processing time in various aspects.  
- Math and String unit test files created with some example tests that make use of the helper utilities.  
- Implemented namespacing. Global namespace is known as "winxframe" but this is likely a temporary placeholder.  
##### &emsp;Changes:
- Re-arranged the file/folder structure to be more sensible.  

### October 13, 2025
#### <u>PRE-ALPHA v1.6</u>
##### &emsp;Additions:
- Added the string utilities header file. Contains helper functions for string formatting and string conversions.  
##### &emsp;Changes:
- Switched the naming convention for class member variables (private/protected) from m_varName to varName_ (trailing underscore).  
- Cleaned up the Error class further.  

### October 11, 2025
#### <u>PRE-ALPHA v1.5</u>
##### &emsp;Additions:
- Added Error Level header file which contains the ErrorLevel enum class. Error levels are categorized as Info, Warning, General, Critical, Fatal, Unknown.  
- Implemented Log method which will log an error to the console rather than throw. Need to add options for debug and file logging.  
- Added Error Macros header file and moved over throw/rethrow error macros and newly added log error macros.  
- WideChar method converts a c-string to wide character format. Currently in Error class but I should make a utils file and add stuff like that there instead.  
##### &emsp;Changes:
- Most Error class outputs are now correctly and safely converting to wide character format in the case that is necessary and possible.  

#### <u>PRE-ALPHA v1.4</u>
##### &emsp;Additions:
- Error Level enum struct header added. Consists of enum class for declaring various levels of errors/warnings.  
##### &emsp;Changes:
- Error Level header file extension renamed.  
- Error throws now also display the associated error level, if an error is being tracked. Otherwise the error level is unknown.  

### October 10, 2025
#### <u>PRE-ALPHA v1.3</u>
##### &emsp;Additions:
- Added a boolean check for Window and Console cleanup; won't run if cleanup already occurred.  
##### &emsp;Changes:
- Further refinement of the Error class. Constructors added/modified.  
- Inclusion of std\::exception_ptr member variable and constructors accepting std\::current_exception for error exception chaining.  
- Exception chaining is a pattern where one exception (often a higher-level or wrapping exception) stores a reference to another exception (the "cause") that triggered it. The outer exception tells where the problem was handled or detected. The inner exception (cause) tells what originally went wrong.  
- Macros modified to account for throwing an error, throwing IF an error, and rethrowing an error. Can perform them with or without context.  
- Removed LogError method from Window class.  

### October 9, 2025
#### <u>PRE-ALPHA v1.2</u>
##### &emsp;Additions:
- Error class header added. Implemented custom error handling.  
- Error class overrides what() function allowing for alternative error catching with std\::exception.  
- Defined macros for throwing errors for ease of usage.  
##### &emsp;Changes:
- Updated debug.h to include a macro for dumping memory leaks.  

### October 7, 2025
#### <u>PRE-ALPHA v1.1</u>
##### &emsp;Additions:
- Added SetWindowTitle method to Window class.  
##### &emsp;Changes:
- Overhauled the Console class.  
- Console Output/Input/Error Handles are assigned the default standard output/input/error streams.  
- WriteOutput, ReadOutput, and ReadInput write and read the raw console output/input.  
- InitBuffer initializes a buffer to hold the raw memory for console write and read regions.  
- CreateWriteRegion prepares the console buffer for output, allowing for optional text color formatting. Returns struct ConsoleWriteRegion.  
- WriteLineChunks is used in conjunction with WriteText, writing multiline output to the console buffer based on the console screen width. Does not currently wrap words.  
- WriteText is the final encapsulation of the logic for the prior methods, allowing for simple writing to the console with optional text color.  
- Several other helper methods implemented to assist in read/write procedure.  
- Updated ReadMe.  

### October 5, 2025
#### <u>PRE-ALPHA v1.0</u>
- Project created.  
- Initial version of project. Allows for creation of console and GUI window, along with the default initialization of some properties such as window height, width, and title.  

Copyright (c) 2025  