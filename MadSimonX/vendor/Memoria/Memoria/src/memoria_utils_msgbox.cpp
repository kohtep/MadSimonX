#include "memoria_utils_msgbox.hpp"

#include <Windows.h>

#include "memoria_utils_format.hpp"
#include "memoria_utils_secure.hpp"

#ifdef MEMORIA_USE_LAZYIMPORT
	#define MessageBoxA    LI_FN(MessageBoxA)
#endif

MEMORIA_BEGIN

void MsgAlert(const char *fmt, ...)
{
	char msgBuf[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(msgBuf, sizeof(msgBuf), fmt, args);
	va_end(args);

	MessageBoxA(HWND_DESKTOP, msgBuf, STRING("Alert"), MB_ICONWARNING | MB_SYSTEMMODAL);
}

void MsgError(const char *fmt, ...)
{
	char msgBuf[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(msgBuf, sizeof(msgBuf), fmt, args);
	va_end(args);

	MessageBoxA(HWND_DESKTOP, msgBuf, STRING("Error"), MB_ICONERROR | MB_SYSTEMMODAL);
}

void MsgInfo(const char *fmt, ...)
{
	char msgBuf[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(msgBuf, sizeof(msgBuf), fmt, args);
	va_end(args);

	MessageBoxA(HWND_DESKTOP, msgBuf, STRING("Info"), MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

MEMORIA_END