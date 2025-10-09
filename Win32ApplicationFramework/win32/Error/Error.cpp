/*!
win32\Console\Error.cpp
Created: October 9, 2025
Updated: October 9, 2025
Copyright (c) 2025, Jacob Gosse

Error source file.
*/

#include "Error.hpp"

/* CONSTRUCTORS */

Error::Error(const char* file, const char* func, int line) :
	m_errorCode(GetLastError()),
	m_file(file),
	m_func(func),
	m_line(line)
{
	OutputDebugStringW(L"CONSTRUCTOR: Error(int line, const char* file, const char* func)\n");
}

Error::Error(const char* file, const char* func, int line, const std::wstring& context) :
	m_errorCode(GetLastError()),
	m_file(file),
	m_func(func),
	m_line(line),
	m_context(context)
{
	OutputDebugStringW(L"CONSTRUCTOR: Error(int line, const char* file, const char* func, const std::wstring& context)\n");
}

/* DESTRUCTOR */

Error::~Error()
{
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

int Error::MsgBox() const
{
	std::wcerr << Msg() << std::endl;
	OutputDebugStringW(Msg().c_str());
	return MessageBoxExW(nullptr, Msg().c_str(), L"Fatal Error!", MB_OK | MB_ICONERROR, LANG_USER_DEFAULT);
}

const char* Error::what() const noexcept
{
	std::wstring wideMsg = Msg();

	int size = WideCharToMultiByte(CP_UTF8, 0, wideMsg.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (size > 0)
	{
		m_what.resize(size);
		WideCharToMultiByte(CP_UTF8, 0, wideMsg.c_str(), -1, m_what.data(), size, nullptr, nullptr);
	}
	else
	{
		m_what = "Failed to convert wide string to UTF-8";
	}

	return m_what.c_str();
}