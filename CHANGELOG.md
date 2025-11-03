# Win32 Application Framework
VERSION:&emsp;&emsp;&emsp;PRE-ALPHA 1.8  
DATE CREATED:&ensp;October 5, 2025  
DATE UPDATED:&nbsp;November 2, 2025  
AUTHOR:&emsp;&emsp;&emsp;&nbsp;Jacob Gosse  

## CHANGELOG

### November 2, 2025
#### <u>PRE-ALPHA v1.8</u>
##### &emsp;Additions:
- Console Color class header added. Implemented console colors.  
- Console Color initializes a fixed (static) palette of 8 basic colors and their 8 brighter counter-parts (16 colors in total that the console is able to use). These can be utilized in writes directly to the console.  
- In order to utilize colors when writing to an output stream such as std::cout, Console Color is used to construct an object with a foreground and background color (type unsigned char) that can then be called during the write to an output stream or a stringstream.  
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
- Inclusion of std::exception_ptr member variable and constructors accepting std::current_exception for error exception chaining.  
- Exception chaining is a pattern where one exception (often a higher-level or wrapping exception) stores a reference to another exception (the "cause") that triggered it. The outer exception tells where the problem was handled or detected. The inner exception (cause) tells what originally went wrong.  
- Macros modified to account for throwing an error, throwing IF an error, and rethrowing an error. Can perform them with or without context.  
- Removed LogError method from Window class.  

### October 9, 2025
#### <u>PRE-ALPHA v1.2</u>
##### &emsp;Additions:
- Error class header added. Implemented custom error handling.  
- Error class overrides what() function allowing for alternative error catching with std::exception.  
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