/*!
lib\source\win32\Error\Error.cpp
Created: October 9, 2025
Updated: October 28, 2025
Copyright (c) 2025, Jacob Gosse

Error source file.
*/

#include <win32/Error/Error.hpp>
#include <win32/utils/win32_utils.hpp>

namespace winxframe
{
	/* CONSTRUCTORS */

	Error::Error(const char* file, const char* func, int line) :
		errorCode_(GetLastError()),
		errorLevel_(Error::AssignErrorLevel()),
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line)\n";
	}

	Error::Error(const char* file, const char* func, int line, const std::wstring& context) :
		errorCode_(GetLastError()),
		errorLevel_(Error::AssignErrorLevel()),
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line),
		context_(context)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, const std::wstring& context)\n";
	}

	Error::Error(const char* file, const char* func, int line, std::exception_ptr cause) :
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line),
		cause_(std::move(cause))
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, std::exception_ptr cause)\n";

		try
		{
			if (cause_)
			{
				std::rethrow_exception(cause_);
			}
		}
		catch (const Error& nestedError)
		{
			errorCode_ = nestedError.GetErrorCode();
			errorLevel_ = nestedError.AssignErrorLevel();
			if (context_.empty()) context_ = nestedError.GetContext();
			else if (!nestedError.GetContext().empty()) context_ = nestedError.GetContext() + L" | " + context_;
			return;
		}
		catch (...)
		{
			// fall through
		}

		errorCode_ = GetLastError();
		errorLevel_ = Error::AssignErrorLevel();
	}

	Error::Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context) :
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line),
		cause_(std::move(cause)),
		context_(context)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context)\n";

		try
		{
			if (cause_)
			{
				std::rethrow_exception(cause_);
			}
		}
		catch (const Error& nestedError)
		{
			errorCode_ = nestedError.GetErrorCode();
			errorLevel_ = nestedError.AssignErrorLevel();
			if (context_.empty()) context_ = nestedError.GetContext();
			else if (!nestedError.GetContext().empty()) context_ = nestedError.GetContext() + L" | " + context_;
			return;
		}
		catch (...)
		{
			// fall through
		}

		errorCode_ = GetLastError();
		errorLevel_ = Error::AssignErrorLevel();
	}

	/* DESTRUCTOR */

	Error::~Error()
	{
		std::wcout << L"DESTRUCTOR: Error()\n";
		OutputDebugStringW(L"DESTRUCTOR: Error()\n");
	}

	/* FUNCTION DEFINITIONS */

	std::wstring Error::Message() const
	{
		std::wostringstream woss;
		woss << L"Code: " << Error::GetErrorCode() << L'\n';
		woss << L"Level: " << Error::ErrorLevelToString(Error::GetErrorLevel()) << L'\n';
		if (!Error::GetContext().empty()) woss << L"Context: " << Error::GetContext() << L'\n';
		woss << L"Error: " << Error::BuildErrorMessage(Error::GetErrorCode()) << L'\n';
		woss << L"File: " << Error::GetFile() << L'\n';
		woss << L"Func: " << Error::GetFunc() << L'\n';
		woss << L"Line: " << Error::GetLine() << L'\n';
		return woss.str();
	}

	std::wstring Error::BuildErrorMessage(DWORD errorCode) const
	{
		std::wstring errorMessage = win32_utils::FormatSysMessageW(errorCode);
		return errorMessage;
	}

	std::string Error::BuildWhat() const
	{
		return string_utils::ToNarrow(Error::Message());
	}

	std::wstring Error::BuildWWhat() const
	{
		return Error::Message();
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

	const wchar_t* Error::ErrorLevelToString(ErrorLevel errorLevel) const
	{
		switch (errorLevel)
		{
		case ErrorLevel::Info:
			return L"Info Error";
		case ErrorLevel::Warning:
			return L"Warning Error";
		case ErrorLevel::General:
			return L"General Error";
		case ErrorLevel::Critical:
			return L"Critical Error";
		case ErrorLevel::Fatal:
			return L"Fatal Error";
		default:
			return L"Unknown Error";
		}
	}

	void Error::Log() const
	{
		std::wcout << L"LOGGED ERROR:\n" << Error::Message() << L'\n';
		std::wstring causeChain = Error::LogCauseChain();
		if (!causeChain.empty())
		{
			std::wcout << L"CAUSE CHAIN:\n" << std::wstring(causeChain.begin(), causeChain.end());
		}
	}

	int Error::MsgBox() const
	{
		return MessageBoxExW
		(
			nullptr,
			Error::Message().c_str(),
			Error::ErrorLevelToString(Error::GetErrorLevel()),
			MB_OK | MB_ICONERROR,
			LANG_USER_DEFAULT
		);
	}

	const char* Error::what() const noexcept
	{
		if (what_.empty()) what_ = Error::BuildWhat();
		return what_.c_str();
	}

	const wchar_t* Error::wwhat() const noexcept
	{
		if (wwhat_.empty()) wwhat_ = Error::BuildWWhat();
		return wwhat_.c_str();
	}

	std::wstring Error::LogCauseChain() const
	{
		std::wostringstream woss;
		if (Error::GetCause())
		{
			try
			{
				std::rethrow_exception(Error::GetCause());
			}
			catch (const Error& nestedError)
			{
				woss << L"Caused by Error class: " << nestedError.wwhat() << L'\n';
				woss << nestedError.LogCauseChain(); // recursive call
			}
			catch (const std::exception& nestedException)
			{
				woss << L"Caused by std::exception: " << string_utils::ToWide(nestedException.what()) << L'\n';
			}
			catch (const char* nestedMsg)
			{
				woss << L"Caused by C-string exception: " << string_utils::ToWide(nestedMsg) << L'\n';
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
}; // end of namespace winxframe