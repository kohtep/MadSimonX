#pragma once

#include <Windows.h>

extern float g_rawDeltaX;
extern float g_rawDeltaY;

extern float mx_frac;
extern float my_frac;

extern bool EnableRawInputForWindow(HWND hWnd);
extern bool DisableRawInputForWindow(HWND hWnd);

extern void AccumulateAxis(float delta, float &accumFloat);
extern int ConsumeAxisInt(float &accumFloat, float &accumFrac);

extern float FloatPrecision(float f, int places);

extern float GetYawMove(int precision);
extern float GetPitchMove(int precision);

extern void ProcessRawInput(UINT msg, WPARAM wParam, LPARAM lParam);