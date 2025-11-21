#include "hooks.hpp"

#include <memory>

#include "core/global.hpp"

#include "common/simon.h"
#include "common/concmds.hpp"

#include "utils/rendering.hpp"
#include "utils/app_utils.hpp"
#include "utils/nprintf.hpp"
#include "utils/mem_utils.hpp"
#include "utils/file_utils.hpp"

#include "common/precache.hpp"
#include "common/rawinput.hpp"

extern void ApplyHooks();

static P::Internal::ViewPunch_t orgCBasePlayer_ViewPunch;
static P::Internal::GetCursorPos_t orgGetCursorPos;

// Utils

static SCREENINFO Screen;
static Vector2D ScreenCenter;

static bool IsInMainMenu()
{
	auto level = G::Engine.pfnGetLevelName();
	return level && !strncmp(level, "maps/c_game_menu", 16);
}

static bool IsPickableItem(const char *pszName)
{
	return (!_strnicmp(pszName, "item_", 5)) || (!_strnicmp(pszName, "weapon_", 7))
		|| (!_strnicmp(pszName, "ammo_", 5));
}

static bool ToScreen(float *origin, float *screen)
{
	static bool c_once = false;

	if (!c_once)
	{
		Screen.iSize = sizeof(Screen);
		G::Engine.pfnGetScreenInfo(&Screen);
		ScreenCenter.x = Screen.iWidth / 2;
		ScreenCenter.y = Screen.iHeight / 2;

		c_once = true;
	}

	if (!origin || !screen)
		return false;

	int iResult = G::Engine.pTriAPI->WorldToScreen(origin, screen);

	if (screen[0] < 1 && screen[1] < 1 && screen[0] > -1 && screen[1] > -1 && !iResult)
	{
		screen[0] = screen[0] * (Screen.iWidth / 2.0f) + (Screen.iWidth / 2.0f);
		screen[1] = -screen[1] * (Screen.iHeight / 2.0f) + (Screen.iHeight / 2.0f);

		return true;
	}

	return false;
}

static const char *MoveTypeToString(int movetype)
{
	switch (movetype)
	{
	case MOVETYPE_NONE: return "none";
	//case MOVETYPE_ANGLENOCLIP: return "none";
	//case MOVETYPE_ANGLECLIP: return "none";
	case MOVETYPE_WALK: return "walk";
	case MOVETYPE_STEP: return "step";
	case MOVETYPE_FLY: return "fly";
	case MOVETYPE_TOSS: return "toss";
	case MOVETYPE_PUSH: return "push";
	case MOVETYPE_NOCLIP: return "noclip";
	case MOVETYPE_FLYMISSILE: return "fly (missile)";
	case MOVETYPE_BOUNCE: return "bounce";
	case MOVETYPE_BOUNCEMISSILE: return "bounce (missile)";
	case MOVETYPE_FOLLOW: return "follow";
	case MOVETYPE_PUSHSTEP: return "push step";
	default: return "unknown";
	}
}

// Impl

#define MADSIMON_CONFIG "madsimon.cfg"

static void ExecuteMadSimonConfig()
{
	char path[MAX_PATH];
	const char *gameDir = G::Engine.pfnGetGameDirectory();

	if (!gameDir)
		return;

	snprintf(path, sizeof(path), "%s\\%s", gameDir, MADSIMON_CONFIG);
	path[sizeof(path) - 1] = '\0';

	if (U::File::FileExists(path))
	{
		char cmd[512];

		snprintf(cmd, sizeof(cmd), "exec \"%s\"\n", MADSIMON_CONFIG);
		cmd[sizeof(cmd) - 1] = '\0';

		G::Engine.pfnClientCmd(cmd);

		// A small hack to invoke Cbuf_Execute in order to run the above
		// 'exec' command within the current frame. This function calls only
		// Cbuf_Execute, so it is safe.
		G::Server.pfnServerExecute();
	}
}

static void hkHUD_Frame_Init(double time)
{
	if (U::Memory::IsDllLoaded("hw.dll") && U::Memory::IsDllLoaded("hl.dll") && U::Memory::IsDllLoaded("client.dll"))
	{
		InitGlobals();

		InitConCmds();
		ExecuteMadSimonConfig();

		ApplyHooks();

		G::Engine.Con_Printf("MadSimonX successfully initialized.\n");
	}
}

static void hkHUD_Frame(double time)
{
	if (*P::cstate == ca_active && !IsInMainMenu())
	{
		CSimon::Instance().Think();
	}

	U::NPrintf::Reset();

	G::Client.pHudFrame(time);
}

static int hkHUD_Redraw(float Time, int Intermission)
{
	int ret = G::Client.pHudRedrawFunc(Time, Intermission);

	if (*P::cstate != ca_active || IsInMainMenu())
		return ret;

	if (C::ent_info->value == 0.0f)
		return ret;

	for (int i = 0; i < *P::sv_numedicts; i++)
	{
		auto edict = &P::sv_edicts[i];

		if (C::ent_info_type->value < 255.0f && edict->v.movetype != C::ent_info_type->value)
			continue;

		auto name = (char *)&(*P::gGlobals)->pStringBase[edict->v.classname];

		char *model;

		if (edict->v.model)
			model = (char *)&(*P::gGlobals)->pStringBase[edict->v.model];
		else
			model = nullptr;

		if (*C::ent_info_filter->string && strncmp(name, C::ent_info_filter->string, strlen(C::ent_info_filter->string)))
			continue;

		Vector2D screen;

		if (ToScreen(edict->v.origin, &screen.x))
		{
			//screen.y -= 300;

			if (IsPickableItem(name))
				G::Engine.pfnDrawSetTextColor(0.90f, 0.90f, 0.0f);
			else
			{
				//if (!strcmp(name, "statue_puzzle_complete"))
				//	Engine.pfnDrawSetTextColor(0.9f, 0.1f, 0.1f);
				//

				if (!strncmp(name, "statue_", 7))
					G::Engine.pfnDrawSetTextColor(0.9f, 0.1f, 0.1f);
				else
					G::Engine.pfnDrawSetTextColor(0.5f, 0.5f, 0.5f);
			}

			G::Engine.pfnDrawConsoleString(screen.x, screen.y, name);

			char buf[256];
			sprintf(buf, "movetype: %s (%d)", MoveTypeToString(edict->v.movetype), edict->v.movetype);

			screen = screen + Vector2D(0.0f, 20.0f);
			G::Engine.pfnDrawSetTextColor(0.5f, 0.5f, 0.5f);
			G::Engine.pfnDrawConsoleString(screen.x, screen.y, buf);

			screen = screen + Vector2D(0.0f, 20.0f);

			if (model)
			{
				sprintf(buf, "model: %s", model);

				G::Engine.pfnDrawSetTextColor(0.5f, 0.5f, 0.5f);
				G::Engine.pfnDrawConsoleString(screen.x, screen.y, buf);
				screen = screen + Vector2D(0.0f, 20.0f);
			}

			bool healthable =
				((edict->v.movetype == MOVETYPE_STEP) || (edict->v.movetype == MOVETYPE_FLY))
				&& edict->v.max_health != 0.0f;

			if (healthable)
			{
				U::Draw::HealthBar(screen.x, screen.y, edict->v.health, edict->v.max_health);
			}
		}
	}

	return ret;
}

static void hkServerDeactivate()
{
	CSimon::Instance().Flush();

	G::EntityInterface.pfnServerDeactivate();
}

static void __fastcall hkCBasePlayer_ViewPunch(CBasePlayer *that, int, float p, float y, float r)
{
	if (C::noviewpunch && C::noviewpunch->value > 0.0)
		return;

	orgCBasePlayer_ViewPunch(that, 0, p, y, r);
}

static void __fastcall hkWorldPrecache(void *self)
{
	Precache();
	P::WorldPrecache(self);
}

static BOOL WINAPI hkGetCursorPos(LPPOINT lpPoint)
{
	if (!lpPoint)
		return FALSE;

	const bool is_client_lib = U::Memory::IsInBounds(_ReturnAddress(), M::GetClient().Handle, M::GetClient().LastByte);
	if (!is_client_lib)
		return orgGetCursorPos(lpPoint);

	if (P::gameui->IsGameUIActive())
	{
		//
		// A quick fix for a bug that causes the protagonist's camera
		// to move after alt-tabbing while the game is paused.
		//

		const int cx = G::Engine.GetWindowCenterX();
		const int cy = G::Engine.GetWindowCenterY();

		lpPoint->x = cx;
		lpPoint->y = cy;
	}
	else if (C::m_rawinput && C::m_rawinput->value > 0.0f)
	{
		//
		// Just like in the case of the quick fix, it simply replaces the result
		// of GetCursorPos with its own.
		//
		// Returning the 'GetWindowCenter' data means that a pointer to the
		// center of the window will be returned, which is equivalent to the
		// cursor not having moved.
		//
		// All core logic for raw input movement is implemented through the
		// modification of the 'mx_accum' and 'my_accum' pointers.
		//

		const int cx = G::Engine.GetWindowCenterX();
		const int cy = G::Engine.GetWindowCenterY();

		lpPoint->x = cx;
		lpPoint->y = cy;
	}
	else
	{
		return orgGetCursorPos(lpPoint);
	}

	return TRUE;
}

static DWORD CALLBACK hkWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ProcessRawInput(msg, wParam, lParam);

	return CallWindowProc(P::WndProc, hwnd, msg, wParam, lParam);
}

void BackupOriginalCode()
{
	U::Memory::CreateMemoryBackup(P::Engine, sizeof(*P::Engine));
	U::Memory::CreateMemoryBackup(P::Server, sizeof(*P::Server));
	U::Memory::CreateMemoryBackup(P::EntityInterface, sizeof(*P::EntityInterface));

	U::Memory::CreateMemoryBackup(P::BunnyHopSpeedUpBlockPtr, 5);
	U::Memory::CreateMemoryBackup(P::NPrintfDelayFix1, sizeof(uint8_t));
	U::Memory::CreateMemoryBackup(P::NPrintfDelayFix2, sizeof(float));
	U::Memory::CreateMemoryBackup(P::UnlockGodmodeFunc, 6);
	U::Memory::CreateMemoryBackup(P::UnlockNoclipFunc, 10);
	U::Memory::CreateMemoryBackup(P::WorldPrecacheCall, sizeof(void *));
}

void RestoreHooks()
{
	if (U::Memory::IsDllLoaded("hw.dll"))
	{
		// If hw.dll is unloaded, then P::game is no longer valid,
		// and there is no point in trying to modify the game window in any way.

		SetWindowLongPtr(P::game->GetMainWindow(), GWL_WNDPROC, (LONG)P::WndProc);
		DisableRawInputForWindow(P::game->GetMainWindow());
	}

	U::Memory::RestoreMemoryBackups();

	U::Memory::Unsplice(orgCBasePlayer_ViewPunch);
	U::Memory::Unsplice(orgGetCursorPos);
}

void ApplyCoreHooks()
{
	U::Memory::CreateMemoryBackup(P::Client, sizeof(*P::Client));

	U::Memory::WritePointer(&P::Client->pHudFrame, hkHUD_Frame_Init);
}

void ApplyHooks()
{
	BackupOriginalCode();

	U::Memory::WritePointer(&P::Client->pHudFrame, hkHUD_Frame);
	U::Memory::WritePointer(&P::Client->pHudRedrawFunc, hkHUD_Redraw);
	U::Memory::WritePointer(&P::EntityInterface->pfnServerDeactivate, hkServerDeactivate);

	U::Memory::Splice(P::ViewPunch, hkCBasePlayer_ViewPunch, &orgCBasePlayer_ViewPunch, false);

	U::Memory::FillNops(P::BunnyHopSpeedUpBlockPtr, 5);

	U::Memory::WriteU8(P::NPrintfDelayFix1, 0xD8); // double -> float
	U::Memory::WritePointer(P::NPrintfDelayFix2, &C::nprintf_time->value);

	// Remove CBasePlayer::PostThink FL_GODMODE flag reset for 'god' cmd
	U::Memory::FillNops(P::UnlockGodmodeFunc, 6);

	// Remove CBasePlayer::PostThink MOVETYPE_NOCLIP reset for 'noclip' cmd
	U::Memory::FillNops(P::UnlockNoclipFunc, 10);

	U::Memory::WritePointer(P::WorldPrecacheCall, hkWorldPrecache);

	// Raw Input

	P::WndProc = (WNDPROC)SetWindowLongPtr(P::game->GetMainWindow(), GWL_WNDPROC, (LONG)hkWndProc);
	U::Memory::SpliceAPI("USER32.dll", "GetCursorPos", hkGetCursorPos, &orgGetCursorPos, false);
	EnableRawInputForWindow(P::game->GetMainWindow());
}