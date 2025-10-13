/*!
win32\Error\errormacros.hpp
Created: October 11, 2025
Updated: October 11, 2025
Copyright (c) 2025, Jacob Gosse

Error Macros header file.
*/

#pragma once

#ifndef ERRORMACROS_HPP_
#define ERRORMACROS_HPP_

#include "Error.hpp"

#define LOG_ERROR() \
	do {\
		Error error = Error(__FILE__, __func__, __LINE__);\
		error.Log();\
	} while(0)
#define LOG_ERROR_CTX(context) \
	do {\
		Error error = Error(__FILE__, __func__, __LINE__, std::wstring(context));\
		error.Log();\
	} while(0)
#define LOG_IF_ERROR(result) \
	do {\
		if (!(result)) {\
			Error error = Error(__FILE__, __func__, __LINE__);\
			error.Log();\
		}\
	} while(0)
#define LOG_IF_ERROR_CTX(result, context) \
	do {\
		if (!(result)) {\
			Error error = Error(__FILE__, __func__, __LINE__, std::wstring(context));\
			error.Log();\
		}\
	} while(0)

#define THROW_ERROR() \
	do { throw Error(__FILE__, __func__, __LINE__); } while(0)
#define THROW_ERROR_CTX(context) \
	do { throw Error(__FILE__, __func__, __LINE__, std::wstring(context)); } while(0)
#define THROW_IF_ERROR(result) \
	do { if (!(result)) throw Error(__FILE__, __func__, __LINE__); } while(0)
#define THROW_IF_ERROR_CTX(result, context) \
	do { if (!(result)) throw Error(__FILE__, __func__, __LINE__, std::wstring(context)); } while(0)
#define RETHROW_ERROR() \
	do { throw Error(__FILE__, __func__, __LINE__, std::current_exception()); } while(0)
#define RETHROW_ERROR_CTX(context) \
	do { throw Error(__FILE__, __func__, __LINE__, std::current_exception(), std::wstring(context)); } while(0)

#endif