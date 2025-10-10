#include "nprintf.hpp"

#include "core/global.hpp"

static size_t gCurrentLine = 0;

void U::NPrintf::Add(const char *fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buf, fmt, args);
	va_end(args);

	G::Engine.Con_NPrintf(gCurrentLine++, buf);
}

void U::NPrintf::Add()
{
	gCurrentLine++;
}

void U::NPrintf::Reset()
{
	gCurrentLine = 0;
}