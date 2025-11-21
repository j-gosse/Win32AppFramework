# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 2.2  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 20, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

A lightweight C++ framework that encapsulates core Win32 API functionality 
for console and window creation, message loops, and error handling. 
Includes a suite for unit testing and various helper utilities. 
Ideal as a foundation for GUI apps, game engines, debug utils, etc.

## Latest Update

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
	
	This implementation serves as a temporary solution for testing and will likely be replaced in the future with a GPU-accelerated rendering backend (e.g., DirectX, Vulkan, OpenGL, or a modern graphics framework).  
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

[CLICK HERE](CHANGELOG.md) to view the full change log.

Copyright (c) 2025  