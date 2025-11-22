#include "console_utils.hpp"

#include "mem_utils.hpp"

#define NAMESPACE_BEGIN namespace U::Console {
#define NAMESPACE_END }

NAMESPACE_BEGIN

static bool color_saved = false;
static Color default_color;

static Color *GetConsoleColor()
{
	// CGameConsoleDialog
	void *panel = U::Memory::Advance(P::console, 8, true);

	// @tag: CGameConsoleDialog::m_PrintColor
	// @sig: 81 C1 [28 01 00 00] E8 ?? ?? ?? ?? 8B 4D EC
	Color *clr_current = reinterpret_cast<Color *>(U::Memory::Advance(panel, 296));

	return clr_current;
}

static void CheckColorBackup()
{
	if (!color_saved)
	{
		color_saved = true;
		default_color = *GetConsoleColor();
	}
}

void SetColor(Color clr)
{
	CheckColorBackup();
	*GetConsoleColor() = clr;
}

void ResetColor()
{
	CheckColorBackup();
	*GetConsoleColor() = default_color;
}

void Print(Color clr, const char *fmt, ...)
{
	CheckColorBackup();

	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buf, fmt, args);
	va_end(args);

	Color *clr_current = GetConsoleColor();

	Color clr_backup = *clr_current;
	*clr_current = clr;
	G::Engine.Con_Printf(buf);
	*clr_current = clr_backup;
}

void Print(const char *fmt, ...)
{
	CheckColorBackup();

	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buf, fmt, args);
	va_end(args);

	G::Engine.Con_Printf(buf);
}

NAMESPACE_END