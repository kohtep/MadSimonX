#pragma once

#include "memoria_common.hpp"

#include <vadefs.h>

MEMORIA_BEGIN

// vsprintf_s
extern int FormatBufSafeV(char *lpBuffer, size_t dwMaxSize, const char *lpFormat, va_list args);

// sprintf_s
extern int FormatBufSafe(char *lpBuffer, size_t dwMaxSize, const char *lpFormat, ...);

extern char *VFormatBufSafe(const char *lpFormat, ...);

MEMORIA_END

// sprintf
#define FormatBuf(buffer, fmt, ...) Memoria::FormatBufSafe(buffer, static_cast<size_t>(-1), fmt, __VA_ARGS__)

#define VFormatBuf(fmt, ...) Memoria::VFormatBufSafe(fmt, __VA_ARGS__)