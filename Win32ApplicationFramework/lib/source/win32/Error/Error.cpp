/*!
lib\source\win32\Error\Error.cpp
Created: October 9, 2025
Updated: November 22, 2025
Copyright (c) 2025, Jacob Gosse

Error source file.
*/

#include <win32/Error/Error.hpp>
#include <win32/utils/win32_utils.hpp>
#include <utils/string_utils.hpp>
#include <thread>

namespace winxframe
{
	/* CONSTRUCTORS */

	Error::Error(const char* file, const char* func, int line) :
		errorCode_(GetLastError()),
		errorLevel_(this->AssignErrorLevel()),
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line)\n";
	}

	Error::Error(const std::wstring& context, const char* file, const char* func, int line) :
		errorCode_(GetLastError()),
		errorLevel_(this->AssignErrorLevel()),
		context_(context),
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, const std::wstring& context)\n";
	}

	Error::Error(std::exception_ptr cause, const char* file, const char* func, int line) :
		cause_(std::move(cause)),
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, std::exception_ptr cause)\n";

		try
		{
			if (this->GetCause())
			{
				std::rethrow_exception(this->GetCause());
			}
		}
		catch (const Error& nestedError)
		{
			errorCode_ = nestedError.GetErrorCode();
			errorLevel_ = nestedError.AssignErrorLevel();
			if (this->GetContext().empty())
				context_ = nestedError.GetContext();
			else if (!nestedError.GetContext().empty())
				context_ = nestedError.GetContext() + L" | " + this->GetContext();
			return;
		}
		catch (...)
		{
			// fall through
		}

		errorCode_ = GetLastError();
		errorLevel_ = this->AssignErrorLevel();
	}

	Error::Error(const std::wstring& context, std::exception_ptr cause, const char* file, const char* func, int line) :
		context_(context),
		cause_(std::move(cause)),
		file_(string_utils::ToWide(file)),
		func_(string_utils::ToWide(func)),
		line_(line)
	{
		std::wcout << L"CONSTRUCTOR: Error(const char* file, const char* func, int line, std::exception_ptr cause, const std::wstring& context)\n";

		try
		{
			if (this->GetCause())
			{
				std::rethrow_exception(this->GetCause());
			}
		}
		catch (const Error& nestedError)
		{
			errorCode_ = nestedError.GetErrorCode();
			errorLevel_ = nestedError.AssignErrorLevel();
			if (this->GetContext().empty())
				context_ = nestedError.GetContext();
			else if (!nestedError.GetContext().empty())
				context_ = nestedError.GetContext() + L" | " + this->GetContext();
			return;
		}
		catch (...)
		{
			// fall through
		}

		errorCode_ = GetLastError();
		errorLevel_ = this->AssignErrorLevel();
	}

	/* DESTRUCTOR */

	Error::~Error()
	{
		std::wcout << L"DESTRUCTOR: Error()\n";
		OutputDebugStringW(L"DESTRUCTOR: Error()\n");
	}

	/* STATIC DEFINITIONS */

	std::mutex Error::msgBoxMutex_;

	/* FUNCTION DEFINITIONS */

	std::wstring Error::Message() const
	{
		std::wostringstream woss;
		if (!this->GetContext().empty())
			woss << L"Context: " << this->GetContext() << L'\n';
		woss << L"Code: " << this->GetErrorCode() << L'\n';
		woss << L"Level: " << this->ErrorLevelToString(this->GetErrorLevel()) << L'\n';
		woss << L"Error: " << this->BuildErrorMessage(this->GetErrorCode()) << L'\n';
		woss << L"File: " << this->GetFile() << L'\n';
		woss << L"Func: " << this->GetFunc() << L'\n';
		woss << L"Line: " << this->GetLine() << L'\n';
		return woss.str();
	}

	std::wstring Error::BuildErrorMessage(DWORD errorCode) const
	{
		std::wstring errorMessage = win32_utils::FormatSysMessageW(errorCode);
		return errorMessage;
	}

	std::string Error::BuildWhat() const
	{
		return string_utils::ToNarrow(this->Message());
	}

	std::wstring Error::BuildWWhat() const
	{
		return this->Message();
	}

	ErrorLevel Error::AssignErrorLevel() const
	{
		switch (errorCode_)
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
		case ERROR_INVALID_WINDOW_HANDLE:
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
			return L"Info";
		case ErrorLevel::Warning:
			return L"Warning";
		case ErrorLevel::General:
			return L"General";
		case ErrorLevel::Critical:
			return L"Critical";
		case ErrorLevel::Fatal:
			return L"Fatal";
		default:
			return L"Unknown";
		}
	}

	void Error::Log() const
	{
		std::wostringstream woss;
		woss << L"LOGGED ERROR:\n" << this->Message() << L'\n';
		std::wcout << woss.str();
		OutputDebugStringW(woss.str().c_str());
		woss.str(L"");
		woss.clear();

		std::wstring causeChain = this->LogCauseChain();
		woss << L"CAUSE CHAIN:\n" << std::wstring(causeChain.begin(), causeChain.end());
		if (!causeChain.empty())
		{
			std::wcout << woss.str();
			OutputDebugStringW(woss.str().c_str());
			woss.str(L"");
			woss.clear();
		}
	}

	int Error::MsgBox() const
	{
		return MessageBoxExW
		(
			nullptr,
			this->Message().c_str(),
			this->ErrorLevelToString(this->GetErrorLevel()),
			MB_OK | MB_ICONERROR,
			LANG_USER_DEFAULT
		);
	}

	void Error::MsgBoxSync() const
	{
		std::thread t([this]()
			{
				std::lock_guard<std::mutex> lock(msgBoxMutex_);
				this->MsgBox();
			}
		);
		t.join(); // pauses thread execution until dismissed
	}

	void Error::MsgBoxAsync() const
	{
		std::thread t([this]()
			{
				std::lock_guard<std::mutex> lock(msgBoxMutex_);
				this->MsgBox();
			}
		);
		t.detach(); // does NOT block thread execution
	}

	const char* Error::what() const noexcept
	{
		if (this->GetWhat().empty())
			what_ = this->BuildWhat();
		return this->GetWhat().c_str();
	}

	const wchar_t* Error::wwhat() const noexcept
	{
		if (this->GetWWhat().empty())
			wwhat_ = this->BuildWWhat();
		return this->GetWWhat().c_str();
	}

	std::wstring Error::LogCauseChain() const
	{
		std::wostringstream woss;
		if (this->GetCause())
		{
			try
			{
				std::rethrow_exception(this->GetCause());
			}
			catch (const Error& nestedError)
			{
				woss << L"Caused by Error class:\n" << nestedError.wwhat() << L'\n';
				woss << nestedError.LogCauseChain(); // recursive call
			}
			catch (const std::exception& nestedException)
			{
				woss << L"Caused by std::exception:\n" << string_utils::ToWide(nestedException.what()) << L'\n';
			}
			catch (const char* nestedMsg)
			{
				woss << L"Caused by C-string exception:\n" << string_utils::ToWide(nestedMsg) << L'\n';
			}
			catch (const wchar_t* nestedWideMsg)
			{
				woss << L"Caused by wide string exception:\n" << (nestedWideMsg ? nestedWideMsg : L"NULL") << L'\n';
			}
			catch (...)
			{
				woss << L"Caused by unknown exception.\n";
			}
		}
		return woss.str();
	}
}; // end of namespace winxframe