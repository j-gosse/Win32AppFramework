/*!
lib\include\win32\Error\Error.hpp
Created: October 9, 2025
Updated: November 2, 2025
Copyright (c) 2025, Jacob Gosse

Error header file.
*/

#pragma once

#ifndef ERROR_HPP_
#define ERROR_HPP_

#include "ErrorLevel.hpp"
#include <win32/framework.h>
#include <win32/debug.h>

namespace winxframe
{
	class Error : public std::exception
	{
	private:
		DWORD errorCode_;
		ErrorLevel errorLevel_;
		std::wstring file_;
		std::wstring func_;
		int line_;
		std::wstring context_;
		std::exception_ptr cause_;
		mutable std::string what_;
		mutable std::wstring wwhat_;

		std::wstring Message() const;
		std::wstring BuildErrorMessage(DWORD errorCode) const;
		std::string BuildWhat() const;
		std::wstring BuildWWhat() const;
		ErrorLevel AssignErrorLevel() const;
		const wchar_t* ErrorLevelToString(ErrorLevel errorLevel) const;

	public:
		Error(const char* file, const char* func, int line);
		Error(const std::wstring& context, const char* file, const char* func, int line);
		Error(std::exception_ptr cause, const char* file, const char* func, int line);
		Error(const std::wstring& context, std::exception_ptr cause, const char* file, const char* func, int line);
		virtual ~Error();

		void Log() const;
		int MsgBox() const;
		const char* what() const noexcept override;
		const wchar_t* wwhat() const noexcept;
		std::wstring LogCauseChain() const;

		DWORD GetErrorCode() const { return errorCode_; }
		ErrorLevel GetErrorLevel() const { return errorLevel_; }
		std::wstring GetFile() const { return file_; }
		std::wstring GetFunc() const { return func_; }
		int GetLine() const { return line_; }
		const std::exception_ptr& GetCause() const noexcept { return cause_; }
		std::string GetWhat() const { return what_; }
		const std::wstring& GetContext() const { return context_; }
	};
}; // end of namespace winxframe

#endif