#pragma once

#if _DEBUG
#include <cstdio>
#include <cstdarg>
#include <Windows.h>

inline void debug_message(const char * format, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, format, args);
	OutputDebugString(buffer);
	va_end(args);
}

#define DEBUG(fmt, ...) debug_message(fmt, __VA_ARGS__)
#define ERR(err) MessageBox(nullptr, std::string(err).c_str(), "ERROR", MB_OK)
#else
#define DEBUG(fmt, ...) {}
#define ERR(err) {}
#endif