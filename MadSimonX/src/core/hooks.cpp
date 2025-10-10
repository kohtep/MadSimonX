#include "hooks.hpp"

#include "core/global.hpp"

#include "common/simon.h"
#include "common/concmds.hpp"

#include "utils/rendering.hpp"
#include "utils/app_utils.hpp"
#include "utils/nprintf.hpp"

#include "common/precache.hpp"

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

static P::Internal::ViewPunch_t orgCBasePlayer_ViewPunch;
static void __fastcall hkCBasePlayer_ViewPunch(CBasePlayer *that, int, float p, float y, float r)
{
	if (C::mp_noviewpunch && C::mp_noviewpunch->value > 0.0)
		return;

	orgCBasePlayer_ViewPunch(that, 0, p, y, r);
}

static void __fastcall hkWorldPrecache(void *self)
{
	Precache();
	P::WorldPrecache(self);
}

void InitHooks()
{
	Memoria::WritePointer(&P::Client->pHudFrame, hkHUD_Frame);
	Memoria::WritePointer(&P::Client->pHudRedrawFunc, hkHUD_Redraw);
	Memoria::WritePointer(&P::EntityInterface->pfnServerDeactivate, hkServerDeactivate);

	Memoria::Hook32(P::ViewPunch, hkCBasePlayer_ViewPunch, &orgCBasePlayer_ViewPunch, Memoria::eInvokeMethod::JumpRel);

	Memoria::FillNops(P::BunnyHopSpeedUpBlockPtr, 5);

	Memoria::WriteU8(P::NPrintfDelayFix1, 0xD8); // double -> float
	Memoria::WritePointer(P::NPrintfDelayFix2, &C::nprintf_time->value);

	// Remove CBasePlayer::PostThink FL_GODMODE flag reset for 'god' cmd
	Memoria::FillNops(P::UnlockGodmodeFunc, 6);

	// Remove CBasePlayer::PostThink MOVETYPE_NOCLIP reset for 'noclip' cmd
	Memoria::FillNops(P::UnlockNoclipFunc, 10);

	Memoria::WriteHook32(P::WorldPrecacheCall, hkWorldPrecache, Memoria::eInvokeMethod::CallRel);
}