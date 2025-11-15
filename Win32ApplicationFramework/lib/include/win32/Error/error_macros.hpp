/*!
lib\include\win32\Error\error_macros.hpp
Created: October 11, 2025
Updated: November 13, 2025
Copyright (c) 2025, Jacob Gosse

Error Macros header file.
*/

#pragma once

#ifndef ERROR_MACROS_HPP_
#define ERROR_MACROS_HPP_

#include "Error.hpp"

namespace winxframe
{
	#define LOG_ERROR()\
		do {\
			Error errorRecord = Error(__FILE__, __func__, __LINE__);\
			errorRecord.Log();\
		} while(0)

	#define LOG_ERROR_CTX(context)\
		do {\
			Error errorRecord = Error(std::wstring(context), __FILE__, __func__, __LINE__);\
			errorRecord.Log();\
		} while(0)

	#define LOG_IF_ERROR(result)\
		do {\
			if (result) {\
				Error errorRecord = Error(__FILE__, __func__, __LINE__);\
				errorRecord.Log();\
			}\
		} while(0)

	#define LOG_IF_ERROR_CTX(result, context)\
		do {\
			if (result) {\
				Error errorRecord = Error(std::wstring(context), __FILE__, __func__, __LINE__);\
				errorRecord.Log();\
			}\
		} while(0)
	
	#define THROW_ERROR() throw Error(__FILE__, __func__, __LINE__);

	#define THROW_ERROR_CTX(context) throw Error(std::wstring(context), __FILE__, __func__, __LINE__);

	#define THROW_IF_ERROR(result)\
		do {\
			if (result)\
				throw Error(__FILE__, __func__, __LINE__);\
		} while(0)

	#define THROW_IF_ERROR_CTX(result, context)\
		do {\
			if (result)\
				throw Error(std::wstring(context), __FILE__, __func__, __LINE__);\
		} while(0)

	#define RETHROW_ERROR() throw Error(std::current_exception(), __FILE__, __func__, __LINE__);

	#define RETHROW_ERROR_CTX(context) throw Error(std::wstring(context), std::current_exception(), __FILE__, __func__, __LINE__);
}; // end of namespace winxframe

#endif