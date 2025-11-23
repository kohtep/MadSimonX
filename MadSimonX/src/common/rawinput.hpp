#pragma once

#include <Windows.h>

extern bool ToggleRawInputForWindow(HWND hWnd, bool enable);

extern void ProcessRawInput(UINT msg, WPARAM wParam, LPARAM lParam);