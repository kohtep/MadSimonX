#include "rendering.hpp"

#include "core/global.hpp"

void U::Draw::Area(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	G::Engine.pfnFillRGBABlend(x, y, w, h, r, g, b, a);
}

void U::Draw::SimpleBox(int x, int y, int w, int h, int linewidth, BYTE r, BYTE g, BYTE b, byte a)
{
	Area(x, y, w, linewidth, r, g, b, 1);
	Area(x + w - linewidth, y + linewidth, linewidth, h - linewidth, r, g, b, a);
	Area(x, y + linewidth, linewidth, h - linewidth, r, g, b, a);
	Area(x + linewidth, y + h - linewidth, w - linewidth * 2, linewidth, r, g, b, a);

	Area(x, y, w, linewidth, r, g, b, a); // fix bad drawing
}

void U::Draw::HealthBar(int x, int y, float health, float maxhealth)
{
	int r, g, b;

	health = health < 0.0f ? 0.0f : health;

	int colorMod = (health / maxhealth) * 255.0f;
	r = 255 - colorMod;
	g = colorMod;
	b = 0;

	health = health / maxhealth * 100;

	G::Engine.pfnFillRGBABlend(x + 2, y + 2, health - 2, 6, r, g, b, 255);

	SimpleBox(x, y, 100, 10, 2, 0, 0, 0, 255);
}