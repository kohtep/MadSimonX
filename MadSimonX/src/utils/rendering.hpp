#pragma once

#include <Windows.h>

// Utility
namespace U::Draw
{
	extern void Area(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a);
	extern void SimpleBox(int x, int y, int w, int h, int linewidth, BYTE r, BYTE g, BYTE b, byte a);
	extern void HealthBar(int x, int y, float health, float maxhealth);
}