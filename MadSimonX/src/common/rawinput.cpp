#include "rawinput.hpp"

#include <intrin.h>
#include <cmath>

#include "utils/mem_utils.hpp"

#include "core/global.hpp"

#include "common/concmds.hpp"
#include "utils/nprintf.hpp"

static const int CVAR_PREC = 3;

float g_rawDeltaX = 0.0f;
float g_rawDeltaY = 0.0f;

float mx_frac = 0.0f;
float my_frac = 0.0f;

bool EnableRawInputForWindow(HWND hWnd)
{
	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = hWnd;

	return RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE;
}

bool DisableRawInputForWindow(HWND hWnd)
{
	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = RIDEV_REMOVE;
	rid.hwndTarget = nullptr;

	return RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE;
}

void AccumulateAxis(float delta, float &accumFloat)
{
	accumFloat += delta;
}

int ConsumeAxisInt(float &accumFloat, float &accumFrac)
{
	float value = accumFloat + accumFrac;

	const int whole = static_cast<int>(std::trunc(value));

	accumFrac = value - static_cast<float>(whole);
	accumFloat = 0.0f;

	return whole;
}

float FloatPrecision(float f, int places)
{
	float n = std::pow(10.0f, places);
	return std::round(f * n) / n;
}

float GetYawMove(int precision)
{
	if (!C::sensitivity || !C::m_yaw)
		return 0.0f;

	float ret = C::m_yaw->value * C::sensitivity->value;

	if (precision > -1)
		ret = FloatPrecision(ret, precision);

	return ret;
}

float GetPitchMove(int precision)
{
	if (!C::sensitivity || !C::m_pitch)
		return 0.0f;

	float ret = C::m_pitch->value * C::sensitivity->value;

	if (precision > -1)
		ret = FloatPrecision(ret, precision);

	return ret;
}

void ProcessRawInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg != WM_INPUT)
		return;

	if (!C::m_rawinput || C::m_rawinput->value <= 0.0f)
		return;

	if (P::gameui->IsGameUIActive())
		return;

	BYTE buffer[sizeof(RAWINPUT)];
	UINT size = sizeof(buffer);

	const UINT received = GetRawInputData(
		reinterpret_cast<HRAWINPUT>(lParam),
		RID_INPUT,
		buffer,
		&size,
		sizeof(RAWINPUTHEADER));

	if (received == static_cast<UINT>(-1) || received > sizeof(buffer))
		return;

	const RAWINPUT *raw = reinterpret_cast<const RAWINPUT *>(buffer);

	if (raw->header.dwType != RIM_TYPEMOUSE)
		return;

	const LONG dx = raw->data.mouse.lLastX;
	const LONG dy = raw->data.mouse.lLastY;

	*P::mx_accum += dx;
	*P::my_accum += dy;

	//const float yaw = GetYawMove(CVAR_PREC) * static_cast<float>(dx);
	//const float pitch = GetPitchMove(CVAR_PREC) * static_cast<float>(dy);

	//AccumulateAxis(yaw, g_rawDeltaX);
	//AccumulateAxis(pitch, g_rawDeltaY);
}