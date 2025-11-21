#include "concmds.hpp"

#include "core/global.hpp"

#include "common/simon.h"

#include "utils/ent_utils.hpp"
#include "utils/app_utils.hpp"

// Cut Cmds

static void pfnHost_God_f()
{
	auto ent = CSimon::Instance().Entity();
	if (!ent)
		return;

	int *flags = &ent->v.flags;

	*flags ^= FL_GODMODE;
	if (*flags & FL_GODMODE)
		G::Engine.Con_Printf("godmode ON\n");
	else
		G::Engine.Con_Printf("godmode OFF\n");
};

static void pfnHost_Notarget_f()
{
	auto ent = CSimon::Instance().Entity();
	if (!ent)
		return;

	int *flags = &ent->v.flags;

	*flags ^= FL_NOTARGET;
	if (*flags & FL_NOTARGET)
		G::Engine.Con_Printf("notarget ON\n");
	else
		G::Engine.Con_Printf("notarget OFF\n");
};

static void pfnHost_Noclip_f()
{
	auto ent = CSimon::Instance().Entity();
	if (!ent)
		return;

	if (ent->v.movetype == MOVETYPE_WALK)
	{
		ent->v.movetype = MOVETYPE_NOCLIP;
		G::Engine.Con_Printf("noclip ON\n");
	}
	else
	{
		ent->v.movetype = MOVETYPE_WALK;
		G::Engine.Con_Printf("noclip OFF\n");
	}
};

// Cmds

#define FCMD_HUD_COMMAND		(1<<0)
#define FCMD_GAME_COMMAND		(1<<1)
#define FCMD_WRAPPER_COMMAND	(1<<2)

static void Cmd_Find_f()
{
	if (G::Engine.Cmd_Argc() < 2)
	{
		auto hdr = G::Engine.Cmd_Argv(0);

		G::Engine.Con_Printf("Usage: %s <SubString>\n", hdr);
		G::Engine.Con_Printf("%s * for full listing\n", hdr);
		G::Engine.Con_Printf("h - hud; g - game; w - wrap\n");
		return;
	}

	auto cmd = reinterpret_cast<cmd_function_t *>(G::Engine.GetFirstCmdFunctionHandle());

	auto str = G::Engine.Cmd_Argv(1);
	if (*str == '*')
		str = nullptr;

	while (cmd)
	{
		if (str)
		{
			if (strstr(cmd->name, str))
			{
				G::Engine.Con_Printf("Name: %s", cmd->name);

				G::Engine.Con_Printf(", Flags: [");

				if (cmd->flags & FCMD_HUD_COMMAND)
					G::Engine.Con_Printf("h");

				if (cmd->flags & FCMD_GAME_COMMAND)
					G::Engine.Con_Printf("g");

				if (cmd->flags & FCMD_WRAPPER_COMMAND)
					G::Engine.Con_Printf("w");

				G::Engine.Con_Printf("]\n");
			}
		}
		else
		{
			G::Engine.Con_Printf("Name: %s", cmd->name);

			G::Engine.Con_Printf(", Flags: [");
			if (cmd->flags != 0)
			{
				if (cmd->flags & FCMD_HUD_COMMAND)
					G::Engine.Con_Printf("h");

				if (cmd->flags & FCMD_GAME_COMMAND)
					G::Engine.Con_Printf("g");

				if (cmd->flags & FCMD_WRAPPER_COMMAND)
					G::Engine.Con_Printf("w");
			}

			G::Engine.Con_Printf("]\n");
		}

		cmd = cmd->next;
	}

	auto cvar = G::Engine.GetFirstCvarPtr();
	while (cvar)
	{
		if (str)
		{
			if (strstr(cvar->name, str))
			{
				G::Engine.Con_Printf("Name: %s, String: %s, Value: %f\n", cvar->name, cvar->string, cvar->value);
			}
		}
		else
		{
			G::Engine.Con_Printf("Name: %s, String: %s, Value: %f\n", cvar->name, cvar->string, cvar->value);
		}

		cvar = cvar->next;
	}
}

static void Entity_Create_f()
{
	if (C::sv_cheats->value <= 0.0f)
	{
		G::Engine.Con_Printf("Can't use cheat command with disabled sv_cheats value.\n");
		return;
	}

	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <Name>\n", G::Engine.Cmd_Argv(0));
		return;
	}

	U::Ent::Spawn(G::Engine.Cmd_Argv(1), CSimon::Instance().TraceEyes());
}

static void Entity_Remove_f()
{
	if (C::sv_cheats->value <= 0.0f)
	{
		G::Engine.Con_Printf("Can't use cheat command with disabled sv_cheats value.\n");
		return;
	}

	U::Ent::Despawn(true);
}

static void Entity_Killed_f()
{
	if (C::sv_cheats->value <= 0.0f)
	{
		G::Engine.Con_Printf("Can't use cheat command with disabled sv_cheats value.\n");
		return;
	}

	U::Ent::Despawn(false);
}

static void Entity_SetName_f()
{
	if (C::sv_cheats->value <= 0.0f)
	{
		G::Engine.Con_Printf("Can't use cheat command with disabled sv_cheats value.\n");
		return;
	}

	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <Name>\n", G::Engine.Cmd_Argv(0));
		return;
	}

	auto pszName = G::Engine.Cmd_Argv(1);
	if (!*pszName)
	{
		G::Engine.Con_Printf("Name is not set.\n");
		return;
	}

	auto edict = CSimon::Instance().TraceEntity();
	if (!edict)
	{
		G::Engine.Con_Printf("Could not find entity.\n");
		return;
	}

	G::Engine.Con_Printf("Set the name of %s to %s.\n", STRING(edict->v.classname), pszName);
	edict->v.classname = MAKE_STRING(_strdup(pszName));
}

static void Entity_GetName_f()
{
	auto ent = CSimon::Instance().TraceEntity();
	if (!ent)
	{
		G::Engine.Con_Printf("Could not find entity.\n");
		return;
	}

	G::Engine.Con_Printf("Entity name is %s\n", STRING(ent->v.classname));
}

static void Cmd_DumpEnts_f()
{
	auto pfn = [](const char *name, void *func, void *func_table, void *arg) -> bool
		{
			if (!strncmp(name, "item_", 5) ||
				!strncmp(name, "weapon_", 7) ||
				!strncmp(name, "monster_", 8))
			{
				G::Engine.Con_Printf("%s\n", name);
			}

			return true;
		};

	U::Ent::Enumerate(pfn, nullptr);
}

static void Cmd_FindEnts_f()
{
	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <SubString>\n", G::Engine.Cmd_Argv(0));
		return;
	}

	auto pfn = [](const char *name, void *func, void *func_table, void *arg) -> bool
		{
			const char *c_badFuncs[] =
			{
				"GiveFnptrsToDll",
				"DelayedUse",
				"GetEntityAPI2"
				"DllEntryPoint"
			};

			for (auto &&s : c_badFuncs)
			{
				if (!strcmp(s, name))
					return true;
			}

			if (strstr(name, "::"))
				return true;

			if (strstr(name, G::Engine.Cmd_Argv(1)))
			{
				G::Engine.Con_Printf("%s\n", name);
			}

			return true;
		};

	U::Ent::Enumerate(pfn, nullptr);
}

static void Cmd_SpawnGameStuff_f()
{
	if (*P::cstate != ca_active)
	{
		G::Engine.Con_Printf("Can't %s, not connected\n", G::Engine.Cmd_Argv(0));
		return;
	}

	struct offsets_t
	{
		int weapon_offset;
		int ammo_offset;
		int item_offset;
	} offsets;

	offsets.weapon_offset = 0;
	offsets.ammo_offset = 0;
	offsets.item_offset = 0;

	auto pfn = [](const char *name, void *func, void *func_table, void *arg) -> bool
		{
			offsets_t *offsets = (offsets_t *)arg;

			if (!strncmp(name, "weapon_", 7) && strcmp(name, "weapon_dualweapon") && strcmp(name, "weapon_action"))
			{
				float offset = (offsets->weapon_offset) * 28.0f;
				Vector pos = CSimon::Instance().TraceEyes() + Vector(offset, 0.0f, 32.0f);
				U::Ent::Spawn(name, pos);

				offsets->weapon_offset++;

				return true;
			}

			if (!strncmp(name, "ammo_", 5))
			{
				if (!strcmp(name, "ammo_flashlightbattery") ||
					!strcmp(name, "ammo_mp5"))
					return true;

				float offset = (offsets->ammo_offset) * 28.0f;
				Vector pos = CSimon::Instance().TraceEyes() + Vector(offset, -64.0f, 32.0f);
				U::Ent::Spawn(name, pos);

				offsets->ammo_offset++;

				return true;
			}

			if (!strncmp(name, "item_", 5))
			{
				if (!strcmp(name, "item_suit") ||
					!strcmp(name, "item_battery") ||
					!strcmp(name, "item_security") ||
					!strcmp(name, "item_longjump") ||
					!strcmp(name, "item_antidote") ||
					!strcmp(name, "item_sodacan") ||
					!strcmp(name, "item_padlock") ||
					!strcmp(name, "item_armour"))
					return true;

				float offset = (offsets->item_offset) * 28.0f;
				Vector pos = CSimon::Instance().TraceEyes() + Vector(offset, -128.0f, 32.0f);
				U::Ent::Spawn(name, pos);

				offsets->item_offset++;

				return true;
			}

			return true;
		};

	U::Ent::Enumerate(pfn, &offsets);
}

static void Cmd_SpawnKohtepSet_f()
{
	auto GiveNamedItemToMe = [](const char *name) -> void
		{
			P::GiveNamedItem(CSimon::Instance().Player(), 0, name, false, false);
		};

	GiveNamedItemToMe("item_nightvision");

	GiveNamedItemToMe("weapon_p345");
	GiveNamedItemToMe("weapon_rifle");

	GiveNamedItemToMe("weapon_g43");
	GiveNamedItemToMe("weapon_nightstick");

	GiveNamedItemToMe("ammo_p345");
	GiveNamedItemToMe("ammo_p345");
	GiveNamedItemToMe("ammo_p345");
	GiveNamedItemToMe("ammo_p345");
	GiveNamedItemToMe("ammo_rifle");
	GiveNamedItemToMe("ammo_rifle");
	GiveNamedItemToMe("ammo_rifle");
	GiveNamedItemToMe("ammo_rifle");
	GiveNamedItemToMe("ammo_g43");
	GiveNamedItemToMe("ammo_g43");
	GiveNamedItemToMe("ammo_g43");
	GiveNamedItemToMe("ammo_g43");

	if (G::Engine.Cmd_Argc() > 1 && *G::Engine.Cmd_Argv(1) == '1')
	{
		G::Engine.pfnClientCmd("quickselset1 1; quickselset2 2; quickselset3 3");
	}
}

static void Cmd_Give_f()
{
	if (C::sv_cheats->value <= 0.0f)
	{
		G::Engine.Con_Printf("Can't use cheat command with disabled sv_cheats value.\n");
		return;
	}

	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <Item Name>\n", G::Engine.Cmd_Argv(0));
		return;
	}

	P::GiveNamedItem(CSimon::Instance().Player(), 0, G::Engine.Cmd_Argv(1), false, false);
}

static void Cmd_About_f()
{
	G::Engine.Con_Printf("MadSimonX - A debugging environment builder for the Cry of Fear mod.\n");
	G::Engine.Con_Printf("Developed by Aleksandr B. aka kohtep\n");
	G::Engine.Con_Printf("\n");
	G::Engine.Con_Printf("Github: https://github.com/kohtep\n");
	G::Engine.Con_Printf("Steam: https://steamcommunity.com/profiles/76561198045711038\n");
}

static void Cmd_HurtMe_f()
{
	if (C::sv_cheats->value <= 0.0f)
	{
		G::Engine.Con_Printf("Can't use cheat command with disabled sv_cheats value.\n");
		return;
	}

	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <Damage>\n", G::Engine.Cmd_Argv(0));
		G::Engine.Con_Printf("Current health: %0.f\n", CSimon::Instance().GetHealth());
		return;
	}

	auto value = atoi(G::Engine.Cmd_Argv(1));

	CSimon::Instance().SetHealth(CSimon::Instance().GetHealth() - value);
	G::Engine.Con_DPrintf("Your health is now %0.f.\n", CSimon::Instance().GetHealth());
}

void Simon_Balls_Out_f()
{
	CSimon::Instance().Player()->m_bScaredBreathing = false;
}

void InitConCmds()
{
	// Register MadSimon commands

	G::Engine.pfnAddCommand("find", Cmd_Find_f);

	G::Engine.pfnAddCommand("ent_create", Entity_Create_f);
	G::Engine.pfnAddCommand("ent_remove", Entity_Remove_f);
	G::Engine.pfnAddCommand("ent_killed", Entity_Killed_f);
	G::Engine.pfnAddCommand("ent_setname", Entity_SetName_f);
	G::Engine.pfnAddCommand("ent_getname", Entity_GetName_f);
	G::Engine.pfnAddCommand("ent_dump", Cmd_DumpEnts_f);
	G::Engine.pfnAddCommand("ent_find", Cmd_FindEnts_f);
	G::Engine.pfnAddCommand("give", Cmd_Give_f);
	G::Engine.pfnAddCommand("hurtme", Cmd_HurtMe_f);

	G::Engine.pfnAddCommand("madsimon", Cmd_About_f);
	G::Engine.pfnAddCommand("nofear", Simon_Balls_Out_f);

	// Register MadSimon CVars

	C::always_first_deploy = G::Engine.pfnRegisterVariable("always_first_deploy", "0", 0);
	C::infinite_stamina = G::Engine.pfnRegisterVariable("infinite_stamina", U::App::HasDebugParam() ? "1" : "0", 0);
	C::infinite_health = G::Engine.pfnRegisterVariable("infinite_health", U::App::HasDebugParam() ? "1" : "0", 0);
	C::infinite_ammo = G::Engine.pfnRegisterVariable("infinite_ammo", U::App::HasDebugParam() ? "1" : "0", 0);
	C::nprintf_time = G::Engine.pfnRegisterVariable("hud_nprintf_time", "4", 0);
	C::ent_info = G::Engine.pfnRegisterVariable("ent_info", U::App::HasDebugParam() ? "1" : "0", 0);
	C::ent_info_type = G::Engine.pfnRegisterVariable("ent_info_type", U::App::HasDebugParam() ? "4" : "0", 0);
	C::ent_info_filter = G::Engine.pfnRegisterVariable("ent_info_filter", "", 0);
	C::noviewpunch = G::Engine.pfnRegisterVariable("noviewpunch", U::App::HasDebugParam() ? "1" : "0", 0);

	// Find & register native stuff

	C::sv_cheats = G::Engine.pfnGetCvarPointer("sv_cheats");

	G::Engine.pfnAddCommand("god", pfnHost_God_f);
	G::Engine.pfnAddCommand("notarget", pfnHost_Notarget_f);
	G::Engine.pfnAddCommand("noclip", pfnHost_Noclip_f);

	// Raw Input

	C::sensitivity = G::Engine.pfnGetCvarPointer("sensitivity");
	C::m_rawinput = G::Engine.pfnRegisterVariable("m_rawinput", "0", 0);
	C::m_yaw = G::Engine.pfnGetCvarPointer("m_yaw");
	C::m_pitch = G::Engine.pfnGetCvarPointer("m_pitch");
}