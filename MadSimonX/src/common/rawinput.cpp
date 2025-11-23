#include "rawinput.hpp"

#include <intrin.h>
#include <cmath>

#include "utils/mem_utils.hpp"

#include "core/global.hpp"

#include "common/concmds.hpp"
#include "utils/nprintf.hpp"

bool ToggleRawInputForWindow(HWND hWnd, bool enable)
{
	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;

	if (enable)
	{
		rid.dwFlags = 0;
		rid.hwndTarget = hWnd;
	}
	else
	{
		rid.dwFlags = RIDEV_REMOVE;
		rid.hwndTarget = nullptr;
	}

	return RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE;
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