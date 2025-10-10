#include "memoria_utils_assert.hpp"

#include "memoria_utils_format.hpp"
#include "memoria_ext_logger.hpp"

#include <Windows.h>
#include <stdio.h>

bool AssertImpl(bool exprResult, const char *exprStr, const char *file, int line, const char *fmt, ...)
{
	if (!exprResult)
	{
		char buffer[1024];

		if (fmt)
		{
			char msgBuf[512];
			va_list args;
			va_start(args, fmt);
			vsprintf_s(msgBuf, sizeof(msgBuf), fmt, args);
			va_end(args);

			FormatBuf(buffer,
				"Assertion failed!\n\n"
				"Expression: %s\n"
				"File: %s\n"
				"Line: %d\n\n"
				"Message: %s\n",
				exprStr, file, line, msgBuf);
		}
		else
		{
			FormatBuf(buffer,
				"Assertion failed!\n\n"
				"Expression: %s\n"
				"File: %s\n"
				"Line: %d\n\n",
				exprStr, file, line);
		}

		LOG_DBG("Assert: %s, File: %s, Line: %d", exprStr, file, line);

		int result = MessageBoxA(HWND_DESKTOP, buffer, "Assert", MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_SYSTEMMODAL);

		if (result == IDABORT)
		{
			ExitProcess(EXIT_FAILURE);
		}
		else if (result == IDRETRY)
		{
			__debugbreak();
		}
	}

	return exprResult;
}