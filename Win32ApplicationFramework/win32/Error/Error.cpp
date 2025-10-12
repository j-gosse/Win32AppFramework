/*!
win32\Error\Error.cpp
Created: October 9, 2025
Updated: October 11, 2025
Copyright (c) 2025, Jacob Gosse

Error source file.
*/

#include "Error.hpp"

/* CONSTRUCTORS */

Error::Error(const char* file, const char* func, int line) :
	m_errorCode(GetLastError()),
	m_errorLevel(AssignErrorLevel()),
	m_file(std::move(file)),
	m_func(std::move(func)),
	m_line(line)
{
	std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line)\n";
	Error::BuildWhat();
}

Error::Error(const char* file, const char* func, int line, const std::wstring& context) :
	m_errorCode(GetLastError()),
	m_errorLevel(AssignErrorLevel()),
	m_file(std::move(file)),
	m_func(std::move(func)),
	m_line(line),
	m_context(context)
{
	std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, const std::wstring& context)\n";
	Error::BuildWhat();
}

Error::Error(const char* file, const char* func, int line, std::exception_ptr cause) :
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
		m_errorCode = nestedError.GetErrorCode();
		m_errorLevel = nestedError.AssignErrorLevel();
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
	m_errorLevel = AssignErrorLevel();
	Error::BuildWhat();
}

Error::Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context) :
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
		m_errorCode = nestedError.GetErrorCode();
		m_errorLevel = nestedError.AssignErrorLevel();
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
	m_errorLevel = AssignErrorLevel();
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

void Error::Log() const
{
	std::wcout << L"LOGGED ERROR:\n" << Error::Msg() << L'\n';
	std::wstring causeChain = GetCauseChain();
	if (!causeChain.empty())
	{
		std::wcout << L"CAUSE CHAIN:\n" << std::wstring(causeChain.begin(), causeChain.end());
	}
}

std::wstring Error::Msg() const
{
	std::wostringstream woss;
	woss << L"Code: " << GetErrorCode() << L'\n';
	woss << L"Level: " << ErrorLevelToString(GetErrorLevel()) << L'\n';
	if (!GetContext().empty()) woss << L"Context: " << GetContext() << L'\n';
	woss << L"Error: " << FormErrorMsg(GetErrorCode()) << L'\n';
	woss << L"File: " << Error::WideChar(GetFile()) << L'\n';
	woss << L"Func: " << Error::WideChar(GetFunc()) << L'\n';
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

ErrorLevel Error::AssignErrorLevel() const
{
	HRESULT errorCode = Error::GetErrorCode();

	switch (errorCode)
	{
	case S_OK:						// Operation was successful
	case S_FALSE:					// Operation was successful, but condition was false
	case ERROR_OPERATION_ABORTED:	// Operation was canceled by user/request
	case ERROR_ALREADY_EXISTS:
		return ErrorLevel::Info;

	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
	case ERROR_INSUFFICIENT_BUFFER:
	case ERROR_MORE_DATA:
	case ERROR_PARTIAL_COPY:
	case ERROR_RETRY:
		return ErrorLevel::Warning;

	case ERROR_INVALID_NAME:
	case ERROR_INVALID_DRIVE:
	case ERROR_BAD_NETPATH:
	case ERROR_DIR_NOT_EMPTY:
	case ERROR_NO_MORE_FILES:
	case ERROR_NOT_READY:			// Device not ready
	case ERROR_SHARING_VIOLATION:
	case ERROR_BUSY:
	case ERROR_DEV_NOT_EXIST:
	case ERROR_BAD_DEVICE:
	case ERROR_DLL_INIT_FAILED:
	case ERROR_INVALID_PARAMETER:
	case ERROR_BAD_FORMAT:
	case ERROR_NOT_SUPPORTED:
	case ERROR_INVALID_DATA:
	case ERROR_GEN_FAILURE:
	case E_FAIL:					// Generic COM failure
		return ErrorLevel::General;

	case ERROR_LOGON_FAILURE:
	case ERROR_NO_LOGON_SERVERS:
	case ERROR_NETWORK_ACCESS_DENIED:
	case ERROR_CANNOT_MAKE:
	case ERROR_FILE_EXISTS:
	case ERROR_LOCK_VIOLATION:
	case ERROR_ACCESS_DENIED:
	case ERROR_INVALID_HANDLE:
		return ErrorLevel::Critical;

	case ERROR_STACK_OVERFLOW:
	case ERROR_NOT_ENOUGH_QUOTA:
	case ERROR_DISK_FULL:
	case ERROR_TOO_MANY_OPEN_FILES:
	case ERROR_HANDLE_DISK_FULL:
	case ERROR_OUTOFMEMORY:
	case E_OUTOFMEMORY:
		return ErrorLevel::Fatal;

	default:
		return ErrorLevel::Unknown;
	}
}

const wchar_t* Error::ErrorLevelToString(ErrorLevel level) const
{
	switch (level)
	{
	case ErrorLevel::Info:
		return L"Info Error!";
	case ErrorLevel::Warning:
		return L"Warning Error!";
	case ErrorLevel::General:
		return L"General Error!";
	case ErrorLevel::Critical:
		return L"Critical Error!";
	case ErrorLevel::Fatal:
		return L"Fatal Error!";
	default:
		return L"Unknown Error!";
	}
}

int Error::MsgBox() const
{
	return MessageBoxExW(nullptr, Msg().c_str(), ErrorLevelToString(GetErrorLevel()), MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
}

const char* Error::what() const noexcept
{
	return m_what.c_str();
}

std::wstring Error::WideChar(const char* string) const
{
	if (!string) return L"NULL";
	int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);

	if (len <= 0) return L"conversion failed";
	std::wstring wstr(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, string, -1, &wstr[0], len);
	wstr.resize(wcslen(wstr.c_str()));
	return wstr;
}

std::wstring Error::GetCauseChain() const
{
	std::wostringstream woss;
	if (GetCause())
	{
		try
		{
			std::rethrow_exception(GetCause());
		}
		catch (const Error& nestedError)
		{
			woss << L"Caused by Error class: " << Error::WideChar(nestedError.what()) << L'\n';
			woss << nestedError.GetCauseChain(); // recursive call
		}
		catch (const std::exception& nestedException)
		{
			woss << L"Caused by std::exception: " << Error::WideChar(nestedException.what()) << L'\n';
		}
		catch (const char* nestedMsg)
		{
			woss << L"Caused by C-string exception: " << Error::WideChar(nestedMsg) << L'\n';
		}
		catch (const wchar_t* nestedWideMsg)
		{
			woss << L"Caused by wide string exception: " << (nestedWideMsg ? nestedWideMsg : L"NULL") << L'\n';
		}
		catch (...)
		{
			woss << L"Caused by unknown exception.\n";
		}
	}
	return woss.str();
}