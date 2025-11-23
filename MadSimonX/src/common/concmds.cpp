#include "concmds.hpp"

#include "core/global.hpp"

#include "common/simon.h"

#include "utils/ent_utils.hpp"
#include "utils/app_utils.hpp"
#include "utils/console_utils.hpp"

// Cut Cmds

static void pfnHost_God_f()
{
	auto ent = CSimon::Instance().GetEntity();
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
	auto ent = CSimon::Instance().GetEntity();
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
	auto ent = CSimon::Instance().GetEntity();
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
	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <Name>\n", G::Engine.Cmd_Argv(0));
		return;
	}

	U::Ent::Spawn(G::Engine.Cmd_Argv(1), CSimon::Instance().GetViewTraceEnd());
}

static void Entity_Remove_f()
{
	U::Ent::Despawn(true);
}

static void Entity_Killed_f()
{
	U::Ent::Despawn(false);
}

static void Entity_SetName_f()
{
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

	auto edict = CSimon::Instance().GetViewTraceEntity();
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
	auto ent = CSimon::Instance().GetViewTraceEntity();
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

struct SpawnOffsets_t
{
	int weapon_offset;
	int ammo_offset;
	int item_offset;
};

struct SpawnGameStuffContext_t
{
	SpawnOffsets_t offsets;

	Vector eyes;
	Vector forward;
	Vector right;
	Vector up;

	float baseDist;
	float stepDist;
	float heightOffset;
	float weaponSideOffset;
	float ammoSideOffset;
	float itemSideOffset;
};

static bool SpawnGameStuff_EnumerCallback(const char *name, void *addr, void *table_addr, void *arg)
{
	SpawnGameStuffContext_t *ctx = (SpawnGameStuffContext_t *)arg;
	SpawnOffsets_t &offsets = ctx->offsets;

	if (!strncmp(name, "weapon_", 7) &&
		strcmp(name, "weapon_dualweapon") &&
		strcmp(name, "weapon_action"))
	{
		float dist = ctx->baseDist + offsets.weapon_offset * ctx->stepDist;

		Vector pos = ctx->eyes
			+ ctx->forward * dist
			+ ctx->right * ctx->weaponSideOffset
			+ ctx->up * ctx->heightOffset;

		U::Ent::Spawn(name, pos);

		offsets.weapon_offset++;

		return true;
	}

	if (!strncmp(name, "ammo_", 5))
	{
		if (!strcmp(name, "ammo_flashlightbattery") ||
			!strcmp(name, "ammo_mp5"))
			return true;

		float dist = ctx->baseDist + offsets.ammo_offset * ctx->stepDist;

		Vector pos = ctx->eyes
			+ ctx->forward * dist
			+ ctx->right * ctx->ammoSideOffset
			+ ctx->up * ctx->heightOffset;

		U::Ent::Spawn(name, pos);

		offsets.ammo_offset++;

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

		float dist = ctx->baseDist + offsets.item_offset * ctx->stepDist;

		Vector pos = ctx->eyes
			+ ctx->forward * dist
			+ ctx->right * ctx->itemSideOffset
			+ ctx->up * ctx->heightOffset;

		U::Ent::Spawn(name, pos);

		offsets.item_offset++;

		return true;
	}

	return true;
}

static void Cmd_SpawnGameStuff_f()
{
	if (*P::cstate != ca_active)
	{
		G::Engine.Con_Printf("Can't %s, not connected\n", G::Engine.Cmd_Argv(0));
		return;
	}

	CSimon &simon = CSimon::Instance();

	if (!simon.IsValid())
		return;

	SpawnGameStuffContext_t ctx;
	ctx.offsets.weapon_offset = 0;
	ctx.offsets.ammo_offset = 0;
	ctx.offsets.item_offset = 0;

	ctx.eyes = simon.GetEyes();
	simon.GetViewVectors(&ctx.forward, &ctx.right, &ctx.up);

	ctx.baseDist = 64.0f;
	ctx.stepDist = 28.0f;
	ctx.heightOffset = 32.0f;

	const float rowSpacing = 64.0f;

	ctx.ammoSideOffset = 0.0f;
	ctx.weaponSideOffset = rowSpacing;
	ctx.itemSideOffset = -rowSpacing;

	U::Ent::Enumerate(SpawnGameStuff_EnumerCallback, &ctx);
}

static void Cmd_SpawnKohtepSet_f()
{
	if (*P::cstate != ca_active)
	{
		G::Engine.Con_Printf("Can't %s, not connected\n", G::Engine.Cmd_Argv(0));
		return;
	}

	CSimon &simon = CSimon::Instance();

	if (!simon.IsValid())
		return;

	simon.GiveItem("weapon_p345");
	simon.GiveItem("weapon_rifle");

	simon.GiveItem("weapon_g43");
	simon.GiveItem("weapon_nightstick");

	simon.GiveItem("ammo_p345");
	simon.GiveItem("ammo_p345");
	simon.GiveItem("ammo_p345");
	simon.GiveItem("ammo_p345");
	simon.GiveItem("ammo_rifle");
	simon.GiveItem("ammo_rifle");
	simon.GiveItem("ammo_rifle");
	simon.GiveItem("ammo_rifle");
	simon.GiveItem("ammo_g43");
	simon.GiveItem("ammo_g43");
	simon.GiveItem("ammo_g43");
	simon.GiveItem("ammo_g43");

	G::Engine.pfnClientCmd("quickselset1 1; quickselset2 2; quickselset3 3; inventoryequip 2");
}

static void Cmd_Give_f()
{
	if (G::Engine.Cmd_Argc() < 2)
	{
		G::Engine.Con_Printf("Syntax: %s <Item Name>\n", G::Engine.Cmd_Argv(0));
		return;
	}

	P::GiveNamedItem(CSimon::Instance().GetPlayer(), 0, G::Engine.Cmd_Argv(1), false, false);
}

static const Color COL_FRAME(160, 160, 160);
static const Color COL_TITLE(255, 220, 50);
static const Color COL_SUBTITLE(150, 200, 255);
static const Color COL_TEXT(220, 220, 220);
static const Color COL_SECTION(255, 180, 80);
static const Color COL_BULLET(255, 120, 120);
static const Color COL_LABEL(180, 220, 180);

static void Cmd_About_f()
{
	U::Console::Print(COL_FRAME, "====================================================\n");

	U::Console::Print(COL_FRAME, "| ");
	U::Console::Print(COL_TITLE, "MadSimonX");
	U::Console::Print(COL_FRAME, " - ");
	U::Console::Print(COL_SUBTITLE, "Cry of Fear Mod Extensions\n");

	U::Console::Print(COL_FRAME, "| ");
	U::Console::Print(COL_TEXT, "Developed by Aleksandr B. (aka kohtep)\n");

	U::Console::Print(COL_FRAME, "|----------------------------------------------------\n");

	U::Console::Print(COL_FRAME, "| ");
	U::Console::Print(COL_SECTION, "Links:\n");

	U::Console::Print(COL_FRAME, "|   ");
	U::Console::Print(COL_BULLET, "* ");
	U::Console::Print(COL_LABEL, "GitHub: ");
	U::Console::Print("https://github.com/kohtep\n");

	U::Console::Print(COL_FRAME, "|   ");
	U::Console::Print(COL_BULLET, "* ");
	U::Console::Print(COL_LABEL, "Steam: ");
	U::Console::Print("https://steamcommunity.com/profiles/76561198045711038\n");

	U::Console::Print(COL_FRAME, "====================================================\n");
}

static void Cmd_HurtMe_f()
{
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

void Cmd_Simon_Balls_Out_f()
{
	CSimon::Instance().GetPlayer()->m_bScaredBreathing = false;
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

	G::Engine.pfnAddCommand("smn_kohtep2", Cmd_SpawnGameStuff_f);
	G::Engine.pfnAddCommand("smn_kohtep", Cmd_SpawnKohtepSet_f);
	G::Engine.pfnAddCommand("smn_nofear", Cmd_Simon_Balls_Out_f);
	G::Engine.pfnAddCommand("smn_about", Cmd_About_f);

	// Register MadSimon CVars

	C::nprintf_time = G::Engine.pfnRegisterVariable("hud_nprintf_time", "4", 0);

	C::ent_info = G::Engine.pfnRegisterVariable("ent_info", "0", 0);
	C::ent_info_filter = G::Engine.pfnRegisterVariable("ent_info_filter", "", 0);
	C::ent_info_type = G::Engine.pfnRegisterVariable("ent_info_type", "0", 0);

	C::always_first_deploy = G::Engine.pfnRegisterVariable("smn_always_first_deploy", "0", 0);
	C::infinite_stamina = G::Engine.pfnRegisterVariable("smn_infinite_stamina", "0", 0);
	C::infinite_health = G::Engine.pfnRegisterVariable("smn_infinite_health", "0", 0);
	C::infinite_ammo = G::Engine.pfnRegisterVariable("smn_infinite_ammo", "0", 0);
	C::noviewpunch = G::Engine.pfnRegisterVariable("smn_noviewpunch", "0", 0);
	C::bhop = G::Engine.pfnRegisterVariable("smn_bhop", "0", 0);
	C::show_info = G::Engine.pfnRegisterVariable("smn_showinfo", "1", 0);

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