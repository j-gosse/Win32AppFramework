#pragma once

#ifndef ERRORLEVEL_H_
#define ERRORLEVEL_H_

enum class ErrorLevel
{
	Info,
	Warning,
	General,
	Critical,
	Fatal,
	Unknown
};

#endif