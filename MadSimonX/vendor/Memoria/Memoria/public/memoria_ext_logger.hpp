//
// memoria_ext_logger.hpp
//
// Lightweight system for event registration to log application behavior.
//
// The system is designed for registering custom handlers via `RegisterLogger`,
// but also provides a quick way to deploy familiar logging systems,
// such as the system console (`InitializeConsoleLogging`).
//

#pragma once

#include "memoria_common.hpp"

#include <Windows.h>

MEMORIA_BEGIN

using LoggerCallback_t = void(*)(const char *text);

extern bool RegisterLogger(LoggerCallback_t cb);
extern bool UnregisterLogger(LoggerCallback_t cb);

extern size_t GetLoggerCount();

enum class ConsoleFlag
{
	None          = 0,
	NoLineBreak   = 1 << 0,
	EscapeSupport = 1 << 1,
};

DEFINE_ENUM_FLAG_OPERATORS(ConsoleFlag);

extern bool InitializeConsoleLogging(ConsoleFlag flags = ConsoleFlag::None);
extern bool FinalizeConsoleLogging();

extern void DispatchLog(const char *fmt, ...);

MEMORIA_END

#ifndef MEMORIA_LOG_PREFIX
#define MEMORIA_LOG_PREFIX ""
#endif

#ifndef MEMORIA_LOG_WRAP
#define MEMORIA_LOG_WRAP(msg) msg
#endif

#define MEMORIA_LOG_FORMAT(fmt) MEMORIA_LOG_WRAP(MEMORIA_LOG_PREFIX fmt)

#ifdef _DEBUG

#define LOG_DBG(fmt, ...) Memoria::DispatchLog(MEMORIA_LOG_FORMAT(fmt), __VA_ARGS__)
#define LOG(fmt, ...)     Memoria::DispatchLog(MEMORIA_LOG_FORMAT(fmt), __VA_ARGS__)

#else

#define LOG_DBG(fmt, ...) ((void)0)
#define LOG(fmt, ...)     Memoria::DispatchLog(MEMORIA_LOG_FORMAT(fmt), __VA_ARGS__)

#endif

//
// ANSI Colors (ANSI X3.64)
//

#define FORE_BLACK         "\033[30m"
#define FORE_RED           "\033[31m"
#define FORE_GREEN         "\033[32m"
#define FORE_YELLOW        "\033[33m"
#define FORE_BLUE          "\033[34m"
#define FORE_MAGENTA       "\033[35m"
#define FORE_CYAN          "\033[36m"
#define FORE_WHITE         "\033[37m"
#define FORE_RESET         "\033[39m"

#define FORE_LIGHTBLACK    "\033[90m"
#define FORE_LIGHTRED      "\033[91m"
#define FORE_LIGHTGREEN    "\033[92m"
#define FORE_LIGHTYELLOW   "\033[93m"
#define FORE_LIGHTBLUE     "\033[94m"
#define FORE_LIGHTMAGENTA  "\033[95m"
#define FORE_LIGHTCYAN     "\033[96m"
#define FORE_LIGHTWHITE    "\033[97m"

#define BACK_BLACK         "\033[40m"
#define BACK_RED           "\033[41m"
#define BACK_GREEN         "\033[42m"
#define BACK_YELLOW        "\033[43m"
#define BACK_BLUE          "\033[44m"
#define BACK_MAGENTA       "\033[45m"
#define BACK_CYAN          "\033[46m"
#define BACK_WHITE         "\033[47m"
#define BACK_RESET         "\033[49m"

#define BACK_LIGHTBLACK    "\033[100m"
#define BACK_LIGHTRED      "\033[101m"
#define BACK_LIGHTGREEN    "\033[102m"
#define BACK_LIGHTYELLOW   "\033[103m"
#define BACK_LIGHTBLUE     "\033[104m"
#define BACK_LIGHTMAGENTA  "\033[105m"
#define BACK_LIGHTCYAN     "\033[106m"
#define BACK_LIGHTWHITE    "\033[107m"

#define STYLE_RESET        "\033[0m"
#define STYLE_BRIGHT       "\033[1m"
#define STYLE_DIM          "\033[2m"
#define STYLE_NORMAL       "\033[22m"

// ANSI Colors (Standart Deviations)

#define FORE_RAINBOW       "\033[300m"