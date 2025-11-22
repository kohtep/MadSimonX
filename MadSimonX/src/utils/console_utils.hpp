#pragma once

#include "core/global.hpp"

namespace U::Console
{
	void SetColor(Color clr);
	void ResetColor();

	void Print(Color clr, const char *fmt, ...);
	void Print(const char *fmt, ...);
}