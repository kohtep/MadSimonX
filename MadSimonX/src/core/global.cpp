#include "global.hpp"

#include <assert.h>
#include <memory>

HINSTANCE G::Instance;

cl_enginefunc_t G::Engine;
cldll_func_t G::Client;
enginefuncs_t G::Server;
DLL_FUNCTIONS G::EntityInterface;

static U::Memory::module_t gEngine;
static U::Memory::module_t gModLib;
static U::Memory::module_t gClientLib;
static U::Memory::module_t gGameUI;

U::Memory::module_t &M::GetEngine()
{
	assert(gEngine.Handle && "Engine library is not ready yet. Please review the caller's logic.");
	return gEngine;
}

U::Memory::module_t &M::GetModLib()
{
	assert(gModLib.Handle && "Mod library is not ready yet. Please review the caller's logic.");
	return gModLib;
}

U::Memory::module_t &M::GetClient()
{
	assert(gClientLib.Handle && "Client library is not ready yet. Please review the caller's logic.");
	return gClientLib;
}

U::Memory::module_t &M::GetGameUI()
{
	assert(gGameUI.Handle && "GameUI library is not ready yet. Please review the caller's logic.");
	return gGameUI;
}

#define SET_PTR(var, moduleFunc, offset) \
    (var = reinterpret_cast<decltype(var)>( \
        reinterpret_cast<uintptr_t>(M::moduleFunc().Handle) + static_cast<uintptr_t>(offset)))

#define SET_IFACE(var, moduleFunc, name)                                      \
    do {                                                                      \
        auto _ci_fn = reinterpret_cast<CreateInterfaceFn>(                    \
            GetProcAddress(static_cast<HMODULE>(M::moduleFunc().Handle),      \
                           "CreateInterface"));                               \
        (var) = _ci_fn                                                        \
            ? reinterpret_cast<decltype(var)>(_ci_fn((name), nullptr))        \
            : nullptr;                                                        \
    } while (0)

#define SET_DEREF_PTR(var, moduleFunc, offset) \
    (var = *reinterpret_cast<decltype(var)*>( \
        reinterpret_cast<uintptr_t>(M::moduleFunc().Handle) + static_cast<uintptr_t>(offset)))

void InitCoreGlobals()
{
	std::construct_at(&gEngine, "hw.dll");

	SET_PTR(P::Client, GetEngine, 0x012B9BE0);

	// It is important to back up the structure before hooking it
	// in order to avoid capturing our own HUD_Frame.
	memcpy(&G::Client, P::Client, sizeof(G::Client));
}

void InitGlobals()
{
	std::construct_at(&gModLib, "hl.dll");
	std::construct_at(&gClientLib, "client.dll");
	std::construct_at(&gGameUI, "GameUI.dll");

	SET_PTR(P::UTIL_FindEntityByString, GetModLib, 0x118480);
	SET_PTR(P::GiveNamedItem, GetModLib, 0xE0A80);
	SET_PTR(P::UTIL_ScreenShake, GetModLib, 0x119DE0);
	SET_PTR(P::ViewPunch, GetModLib, 0xF91B0);
	SET_PTR(P::CBaseEntity_Create, GetModLib, 0x17950);
	SET_PTR(P::UTIL_Remove, GetModLib, 0x119900);

	SET_PTR(P::cstate, GetEngine, 0x10CF280);
	SET_PTR(P::gGlobals, GetModLib, 0x223188);

	SET_PTR(P::Engine, GetEngine, 0x001A9A90);
	SET_PTR(P::Server, GetEngine, 0x001CED18);
	SET_PTR(P::EntityInterface, GetEngine, 0x0080D8C0);

	SET_PTR(P::BunnyHopSpeedUpBlockPtr, GetModLib, 0x66BC);
	SET_PTR(P::NPrintfDelayFix1, GetEngine, 0x4B06A);
	SET_PTR(P::NPrintfDelayFix2, GetEngine, 0x4B06C);

	SET_PTR(P::WorldPrecache, GetModLib, 0x120EB0);
	SET_PTR(P::WorldPrecacheCall, GetModLib, 0x1E6A78);

	SET_PTR(P::UnlockGodmodeFunc, GetModLib, 0xEB227);
	SET_PTR(P::UnlockNoclipFunc, GetModLib, 0xEB20C);

	SET_PTR(P::sv_numedicts, GetEngine, 0x008724F8);
	SET_DEREF_PTR(P::sv_edicts, GetEngine, 0x00872500);

	SET_DEREF_PTR(P::game, GetEngine, 0x1D08CC);

	SET_IFACE(P::gameui, GetGameUI, "GameUI007");

	memcpy(&G::Engine, P::Engine, sizeof(G::Engine));
	memcpy(&G::Server, P::Server, sizeof(G::Server));
	memcpy(&G::EntityInterface, P::EntityInterface, sizeof(G::EntityInterface));

	SET_PTR(P::mx_accum, GetClient, 0x543580);
	SET_PTR(P::my_accum, GetClient, 0x543584);
}