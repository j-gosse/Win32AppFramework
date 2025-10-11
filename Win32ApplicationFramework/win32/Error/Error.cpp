/*!
win32\Error\Error.cpp
Created: October 9, 2025
Updated: October 10, 2025
Copyright (c) 2025, Jacob Gosse

Error source file.
*/

#include "Error.hpp"

/* CONSTRUCTORS */

Error::Error(const char* file, const char* func, int line) :
	m_errorCode(GetLastError()),
	m_file(std::move(file)),
	m_func(std::move(func)),
	m_line(line)
{
	std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line)\n";
	Error::BuildWhat();
}

Error::Error(const char* file, const char* func, int line, const std::wstring& context) :
	m_errorCode(GetLastError()),
	m_file(std::move(file)),
	m_func(std::move(func)),
	m_line(line),
	m_context(context)
{
	std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, const std::wstring& context)\n";
	Error::BuildWhat();
}

Error::Error(const char* file, const char* func, int line, std::exception_ptr cause) :
	m_errorCode(GetLastError()),
	m_file(std::move(file)),
	m_func(std::move(func)),
	m_line(line),
	m_cause(std::move(cause))
{
	std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, std::exception_ptr cause)\n";

	try
	{
		if (m_cause)
		{
			std::rethrow_exception(m_cause);
		}
	}
	catch (const Error& nestedError)
	{
		m_errorCode = nestedError.GetCode();
		if (m_context.empty()) m_context = nestedError.GetContext();
		else if (!nestedError.GetContext().empty()) m_context = nestedError.GetContext() + L" | " + m_context;

		Error::BuildWhat();
		return;
	}
	catch (...)
	{
		// fall through
	}

	m_errorCode = GetLastError();
	Error::BuildWhat();
}

Error::Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context) :
	m_errorCode(GetLastError()),
	m_file(std::move(file)),
	m_func(std::move(func)),
	m_line(line),
	m_cause(std::move(cause)),
	m_context(context)
{
	std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context)\n";

	try
	{
		if (m_cause)
		{
			std::rethrow_exception(m_cause);
		}
	}
	catch (const Error& nestedError)
	{
		m_errorCode = nestedError.GetCode();
		if (m_context.empty()) m_context = nestedError.GetContext();
		else if (!nestedError.GetContext().empty()) m_context = nestedError.GetContext() + L" | " + m_context;

		Error::BuildWhat();
		return;
	}
	catch (...)
	{
		// fall through
	}

	m_errorCode = GetLastError();
	Error::BuildWhat();
}

/* DESTRUCTOR */

Error::~Error()
{
	std::wcout << L"DESTRUCTOR: Error()\n";
	OutputDebugStringW(L"DESTRUCTOR: Error()\n");
}

/* FUNCTION DEFINITIONS */

std::wstring Error::FormErrorMsg(DWORD msgId) const
{
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER 
					| FORMAT_MESSAGE_FROM_SYSTEM
					| FORMAT_MESSAGE_IGNORE_INSERTS
					| FORMAT_MESSAGE_MAX_WIDTH_MASK;

	LPWSTR msgBuffer{};

	DWORD charCount = FormatMessageW
	(
		dwFlags,
		NULL,
		msgId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msgBuffer),
		0,
		NULL
	);

	std::wstring msg = (charCount == 0 || msgBuffer == nullptr) ? L"unknown error message" : msgBuffer;
	LocalFree(msgBuffer);
	return msg;
}

std::wstring Error::Msg() const
{
	std::wostringstream woss;
	woss << L"Code: " << GetCode() << L'\n';
	if (!GetContext().empty()) woss << L"Context: " << GetContext() << L"\n";
	woss << L"Msg: " << FormErrorMsg(GetCode()) << L'\n';
	woss << L"File: " << GetFile() << L'\n';
	woss << L"Func: " << GetFunc() << L'\n';
	woss << L"Line: " << GetLine() << L'\n';
	return woss.str();
}

void Error::BuildWhat()
{
	std::wstring wideMsg = Msg();
	int size = WideCharToMultiByte(CP_UTF8, 0, wideMsg.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (size > 0)
	{
		m_what.resize(size);
		WideCharToMultiByte(CP_UTF8, 0, wideMsg.c_str(), -1, m_what.data(), size, nullptr, nullptr);
	}
	else m_what = "Failed to convert wide string to UTF-8";

}

int Error::MsgBox() const
{
	return MessageBoxExW(nullptr, Msg().c_str(), L"Fatal Error!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
}

const char* Error::what() const noexcept
{
	return m_what.c_str();
}

void Error::PrintCauseChain() const
{
	if (GetCause())
	{
		try
		{
			std::rethrow_exception(GetCause());
		}
		catch (const Error& nested)
		{
			std::cerr << "Caused by Error class: " << (nested.what() ? nested.what() : "NULL") << "\n";
			nested.PrintCauseChain();
		}
		catch (const std::exception& nestedStd)
		{
			std::cerr << "Caused by std::exception: " << (nestedStd.what() ? nestedStd.what() : "NULL") << "\n";
		}
		catch (const char* nestedMsg)
		{
			std::cerr << "Caused by C-string exception: " << (nestedMsg ? nestedMsg : "NULL") << "\n";
		}
		catch (...)
		{
			std::cerr << "Caused by unknown exception.\n";
		}
	}
}