/*!
win32\Error\Error.hpp
Created: October 9, 2025
Updated: October 11, 2025
Copyright (c) 2025, Jacob Gosse

Error header file.
*/

#include <win32/framework.h>
#include <win32/debug.h>
#include "ErrorLevel.hpp"

#pragma once

#ifndef ERROR_HPP_
#define ERROR_HPP_

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

class Error : public std::exception
{
private:
	DWORD m_errorCode;
	ErrorLevel m_errorLevel;
	const char* m_file;
	const char* m_func;
	int m_line;
	std::exception_ptr m_cause;
	mutable std::string m_what;
	std::wstring m_context;

	std::wstring FormErrorMsg(DWORD msgId) const;
	std::wstring Msg() const;
	void BuildWhat();

public:
	Error(const char* file, const char* func, int line);
	Error(const char* file, const char* func, int line, const std::wstring& context);
	Error(const char* file, const char* func, int line, std::exception_ptr cause);
	Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context);
	virtual ~Error();

	int MsgBox() const;
	const char* what() const noexcept override;
	void PrintCauseChain() const;

	ErrorLevel AssignErrorLevel() const;
	const wchar_t* ErrorLevelToString(ErrorLevel level) const;

	DWORD GetErrorCode() const { return m_errorCode; }
	ErrorLevel GetErrorLevel() const { return m_errorLevel; }
	const char* GetFile() const { return m_file; }
	const char* GetFunc() const { return m_func; }
	int GetLine() const { return m_line; }
	const std::exception_ptr& GetCause() const noexcept { return m_cause; }
	std::string GetWhat() const { return m_what; }
	const std::wstring& GetContext() const { return m_context; }
};

#endif