/*!
lib\include\win32\framework.h
Created: October 5, 2025
Updated: October 27, 2025
Copyright (c) 2025, Jacob Gosse

Framework header file. Include file for standard system include files, or project specific include files.
*/

#pragma once

#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#define NOMINMAX			// Exclude windows.h min/max macro definitions

// Windows Headers
#include <windows.h>
#include <io.h>
#include <conio.h>			// _getch()
#include <lmcons.h>			// UNLEN and MAX_COMPUTERNAME_LENGTH

// C RunTime Headers
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C Standard Library Headers
#include <stdio.h>
#include <fcntl.h>

// C++ Standard Library Headers
#include <string>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ostream>
#include <fstream>
#include <filesystem>
#include <type_traits>

#include <stdexcept>
#include <cstdio>
#include <thread>

#include <chrono>
#include <map>
#include <unordered_map>
#include <vector>
#include <array>

#endif
