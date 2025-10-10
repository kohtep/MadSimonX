#include "memoria_ext_logger.hpp"

#include "memoria_utils_format.hpp"
#include "memoria_utils_vector.hpp"
#include "memoria_utils_optional.hpp"
#include "memoria_utils_assert.hpp"

#include <stdarg.h>
#include <Windows.h>

#include "memoria_utils_secure.hpp"

#ifdef MEMORIA_USE_LAZYIMPORT
	#define AllocConsole       LI_FN(AllocConsole)
	#define FreeConsole        LI_FN(FreeConsole)
	#define GetConsoleWindow   LI_FN(GetConsoleWindow)
	#define GetStdHandle       LI_FN(GetStdHandle)
	#define WriteConsoleA      LI_FN(WriteConsoleA)
#endif

MEMORIA_BEGIN

static Memoria::FixedVector<LoggerCallback_t, 32> LoggerFns;

static bool IsConsoleAvailable()
{
	return GetConsoleWindow() != NULL;
}

static Memoria::Optional<bool> gbIsConsoleWasCreated;
static HANDLE ghConsoleHandle = INVALID_HANDLE_VALUE;

static ConsoleFlag gConsoleFlags = ConsoleFlag::None;
static WORD gInitialTextAttributes = 0;

static bool gbRainbowMode = false;

static WORD GetRainbowColor(size_t i)
{
	static const WORD rainbow[] =
	{
		FOREGROUND_RED,
		FOREGROUND_RED | FOREGROUND_GREEN,
		FOREGROUND_GREEN,
		FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_BLUE,
		FOREGROUND_BLUE | FOREGROUND_RED,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	return rainbow[i % _countof(rainbow)];
}

static bool ContainsEscapeSequence(const char *text)
{
	return (text && strchr(text, '\033') != nullptr);
}

static void ApplyAnsiAttributes(const char *code)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	WORD attributes;

	if (!GetConsoleScreenBufferInfo(ghConsoleHandle, &info))
		return;

	attributes = info.wAttributes;

	char buffer[64];
	strncpy_s(buffer, code, sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	char *context = nullptr;
	char *token = strtok_s(buffer, ";", &context);
	while (token)
	{
		int val = atoi(token);

		if (val == 300)
		{
			gbRainbowMode = true;
			token = strtok_s(nullptr, ";", &context);
			continue;
		}

		switch (val)
		{
		case 0:
			attributes = gInitialTextAttributes;
			gbRainbowMode = false;
			break;
		case 1:
			attributes |= FOREGROUND_INTENSITY;
			break;
		case 2:
			attributes &= ~FOREGROUND_INTENSITY;
			break;
		case 22:
			attributes &= ~FOREGROUND_INTENSITY;
			break;

		// Foreground basic
		case 30: attributes = (attributes & 0xFFF0) | 0; break;
		case 31: attributes = (attributes & 0xFFF0) | FOREGROUND_RED; break;
		case 32: attributes = (attributes & 0xFFF0) | FOREGROUND_GREEN; break;
		case 33: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_GREEN); break;
		case 34: attributes = (attributes & 0xFFF0) | FOREGROUND_BLUE; break;
		case 35: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_BLUE); break;
		case 36: attributes = (attributes & 0xFFF0) | (FOREGROUND_GREEN | FOREGROUND_BLUE); break;
		case 37: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); break;
		case 39: attributes = (attributes & 0xFFF0) | (gInitialTextAttributes & 0x000F); gbRainbowMode = false; break;

		// Foreground bright
		case 90: attributes = (attributes & 0xFFF0) | FOREGROUND_INTENSITY; break;
		case 91: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_INTENSITY); break;
		case 92: attributes = (attributes & 0xFFF0) | (FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		case 93: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		case 94: attributes = (attributes & 0xFFF0) | (FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
		case 95: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
		case 96: attributes = (attributes & 0xFFF0) | (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
		case 97: attributes = (attributes & 0xFFF0) | (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;

		// Background basic
		case 40: attributes = (attributes & 0xFF0F) | 0; break;
		case 41: attributes = (attributes & 0xFF0F) | BACKGROUND_RED; break;
		case 42: attributes = (attributes & 0xFF0F) | BACKGROUND_GREEN; break;
		case 43: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_GREEN); break;
		case 44: attributes = (attributes & 0xFF0F) | BACKGROUND_BLUE; break;
		case 45: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_BLUE); break;
		case 46: attributes = (attributes & 0xFF0F) | (BACKGROUND_GREEN | BACKGROUND_BLUE); break;
		case 47: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE); break;
		case 49: attributes = (attributes & 0xFF0F) | (gInitialTextAttributes & 0x00F0); break;

		// Background bright
		case 100: attributes = (attributes & 0xFF0F) | BACKGROUND_INTENSITY; break;
		case 101: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_INTENSITY); break;
		case 102: attributes = (attributes & 0xFF0F) | (BACKGROUND_GREEN | BACKGROUND_INTENSITY); break;
		case 103: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY); break;
		case 104: attributes = (attributes & 0xFF0F) | (BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
		case 105: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
		case 106: attributes = (attributes & 0xFF0F) | (BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
		case 107: attributes = (attributes & 0xFF0F) | (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;

		default:
			break;
		}

		token = strtok_s(nullptr, ";", &context);
	}

	SetConsoleTextAttribute(ghConsoleHandle, attributes);
}

static void WriteColoredText(const char *text, size_t length)
{
	DWORD written = 0;

	if (gbRainbowMode)
	{
		for (size_t i = 0; i < length; ++i)
		{
			SetConsoleTextAttribute(ghConsoleHandle, GetRainbowColor(i));
			WriteConsoleA(ghConsoleHandle, &text[i], 1, &written, nullptr);
		}
	}
	else
	{
		WriteConsoleA(ghConsoleHandle, text, (DWORD)length, &written, nullptr);
	}
}

void LogToConsoleWithEsc(const char *text)
{
	const char *ptr = text;
	const char *segment_start = text;

	while (*ptr)
	{
		if (*ptr == '\033' && *(ptr + 1) == '[')
		{
			if (ptr > segment_start)
			{
				size_t len = ptr - segment_start;
				WriteColoredText(segment_start, len);
			}

			const char *code_start = ptr + 2;
			const char *code_end = code_start;

			while (*code_end && ((*code_end >= '0' && *code_end <= '9') || *code_end == ';'))
				++code_end;

			if (*code_end == 'm')
			{
				char code_buffer[64] = { 0 };
				size_t code_len = code_end - code_start;
				if (code_len < sizeof(code_buffer))
				{
					memcpy(code_buffer, code_start, code_len);
					code_buffer[code_len] = '\0';
					ApplyAnsiAttributes(code_buffer);
				}

				ptr = code_end + 1;
				segment_start = ptr;
				continue;
			}
		}

		++ptr;
	}

	if (*segment_start)
	{
		size_t len = strlen(segment_start);
		WriteColoredText(segment_start, len);
	}
}

static void LogToConsole(const char *text)
{
	if (AssertIf(ghConsoleHandle != INVALID_HANDLE_VALUE))
	{
		const bool is_linebreak_required = !((gConsoleFlags & ConsoleFlag::NoLineBreak) == ConsoleFlag::NoLineBreak);
		const bool is_esc_supported = ((gConsoleFlags & ConsoleFlag::EscapeSupport) == ConsoleFlag::EscapeSupport);

		if (is_esc_supported && ContainsEscapeSequence(text))
		{
			LogToConsoleWithEsc(text);
		}
		else
		{
			DWORD written = 0;
			WriteConsoleA(ghConsoleHandle, text, (DWORD)strlen(text), &written, nullptr);
		}

		if (is_linebreak_required)
		{
			DWORD written = 0;
			char newline = '\n';
			WriteConsoleA(ghConsoleHandle, &newline, (DWORD)sizeof(newline), &written, nullptr);
		}
	}
}

bool RegisterLogger(LoggerCallback_t cb)
{
	if (LoggerFns.full())
		return false;

	LoggerFns.emplace_back(cb);
	return true;
}

bool UnregisterLogger(LoggerCallback_t cb)
{
	if (auto it = LoggerFns.find(cb); it != LoggerFns.end())
	{
		LoggerFns.erase(it);
		return true;
	}

	return false;
}

size_t GetLoggerCount()
{
	return LoggerFns.size();
}

bool InitializeConsoleLogging(ConsoleFlag flags)
{
	if (gbIsConsoleWasCreated.has_value())
	{
		AssertMsg(false, "The console has already been initialized. Terminate the previous session.");
		return false;
	}

	if (!IsConsoleAvailable())
	{
		if (!AllocConsole())
		{
			AssertMsg(false, "Failed to initialize the console. Error code '%d'.", GetLastError());
			return false;
		}

		gbIsConsoleWasCreated = true;
	}
	else
	{
		gbIsConsoleWasCreated = false;
	}

	ghConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	gConsoleFlags = flags;

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(ghConsoleHandle, &info))
		gInitialTextAttributes = info.wAttributes;

	RegisterLogger(LogToConsole);

	return true;
}

bool FinalizeConsoleLogging()
{
	if (!gbIsConsoleWasCreated.has_value())
	{
		AssertMsg(false, "The console has not yet been initialized for logging.");
		return false;
	}

	if (gbIsConsoleWasCreated.value())
	{
		if (!FreeConsole())
		{
			AssertMsg(false, "Failed to free the console. Error code '%d'.", GetLastError());
			return false;
		}
	}

	gbIsConsoleWasCreated.reset();
	ghConsoleHandle = INVALID_HANDLE_VALUE;

	UnregisterLogger(LogToConsole);
	return true;
}

struct LogFragment_t
{

};

void DispatchLog(const char *fmt, ...)
{
	Assert(fmt);

	if (LoggerFns.empty())
		return;

	if (!fmt || !*fmt)
		return;

	char msgBuf[2048];
	va_list args;
	va_start(args, fmt);
	Memoria::FormatBufSafeV(msgBuf, sizeof(msgBuf), fmt, args);
	va_end(args);

	for (auto fn : LoggerFns)
		fn(msgBuf);
}

MEMORIA_END