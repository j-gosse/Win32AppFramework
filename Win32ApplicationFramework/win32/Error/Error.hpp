/*!
win32\Console\Error.hpp
Created: October 9, 2025
Updated: October 9, 2025
Copyright (c) 2025, Jacob Gosse

Error header file.
*/

#include <win32/framework.h>

#pragma once

#ifndef ERROR_HPP_
#define ERROR_HPP_

#define THROW_IF_ERROR(result) do { if (!(result)) throw Error(__FILE__, __func__, __LINE__); } while(0)
#define THROW_IF_ERROR_CTX(result, context) do { if (!(result)) throw Error(__FILE__, __func__, __LINE__, context); } while(0)

class Error : public std::exception
{
private:
	DWORD m_errorCode;
	const char* m_file;
	const char* m_func;
	int m_line;
	std::wstring m_context;
	mutable std::string m_what;

	std::wstring FormErrorMsg(DWORD msgId) const;

public:
	Error(const char* file, const char* func, int line);
	Error(const char* file, const char* func, int line, const std::wstring& context);
	virtual ~Error();

	std::wstring Msg() const;
	int MsgBox() const;
	const char* what() const noexcept override;

	DWORD GetCode() const { return m_errorCode; }
	const char* GetFile() const { return m_file; }
	const char* GetFunc() const { return m_func; }
	int GetLine() const { return m_line; }
	const std::wstring& GetContext() const { return m_context; }
	std::string GetWhat() const { return m_what; }
};

#endif