#include "global.hpp"

HINSTANCE G::Instance;

cl_enginefunc_t G::Engine;
cldll_func_t G::Client;
enginefuncs_t G::Server;
DLL_FUNCTIONS G::EntityInterface;

using namespace Memoria;

static std::unique_ptr<CMemoryModule> gEngine;
static std::unique_ptr<CMemoryModule> gModLib;

CMemoryModule &M::GetEngine()
{
	AssertMsg(gEngine->IsLoaded(), "Engine library is not ready yet. Please review the caller's logic.");
	return *gEngine.get();
}

CMemoryModule &M::GetModLib()
{
	AssertMsg(gModLib->IsLoaded(), "Mod library is not ready yet. Please review the caller's logic.");
	return *gModLib.get();
}

void InitGlobals()
{
	gEngine = CMemoryModule::CreateFromLibrary("hw.dll");
	gModLib = CMemoryModule::CreateFromLibrary("hl.dll");
}

#define SET_PTR(var, moduleFunc, offset) \
    (var = reinterpret_cast<decltype(var)>( \
        reinterpret_cast<uintptr_t>(M::moduleFunc().GetBase()) + static_cast<uintptr_t>(offset)))

#define SET_DEREF_PTR(var, moduleFunc, offset) \
    (var = *reinterpret_cast<decltype(var)*>( \
        reinterpret_cast<uintptr_t>(M::moduleFunc().GetBase()) + static_cast<uintptr_t>(offset)))

static void InitPointers_1_55()
{
	SET_PTR(P::UTIL_FindEntityByString, GetModLib, 0x170750);
	SET_PTR(P::GiveNamedItem, GetModLib, 0x125350);
	SET_PTR(P::UTIL_ScreenShake, GetModLib, 0x1710E0);
	SET_PTR(P::ViewPunch, GetModLib, 0x12CC20);
	SET_PTR(P::CBaseEntity_Create, GetModLib, 0x014790);
	SET_PTR(P::UTIL_Remove, GetModLib, 0x173C80);

	SET_PTR(P::cstate, GetEngine, 0x10CF280);
	SET_PTR(P::gGlobals, GetModLib, 0x2196B0);

	SET_PTR(P::Engine, GetEngine, 0x001A9A90);
	SET_PTR(P::Server, GetEngine, 0x001CED18);
	SET_PTR(P::Client, GetEngine, 0x012B9BE0);
	SET_PTR(P::EntityInterface, GetEngine, 0x0080D8C0);

	SET_PTR(P::BunnyHopSpeedUpBlockPtr, GetModLib, 0x1483F6);
	SET_PTR(P::NPrintfDelayFix1, GetEngine, 0x4B06A);
	SET_PTR(P::NPrintfDelayFix2, GetEngine, 0x4B06C);

	SET_PTR(P::WorldPrecache, GetEngine, 0x17E500);
	SET_PTR(P::WorldPrecacheCall, GetEngine, 0x1F8610);

	SET_PTR(P::UnlockGodmodeFunc, GetModLib, 0x11E4C8);
	SET_PTR(P::UnlockNoclipFunc, GetModLib, 0x11E498);

	SET_PTR(P::sv_numedicts, GetEngine, 0x008724F8);
	SET_DEREF_PTR(P::sv_edicts, GetEngine, 0x00872500);
}

void InitPointers()
{
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
	SET_PTR(P::Client, GetEngine, 0x012B9BE0);
	SET_PTR(P::EntityInterface, GetEngine, 0x0080D8C0);

	SET_PTR(P::BunnyHopSpeedUpBlockPtr, GetModLib, 0x66BC);
	SET_PTR(P::NPrintfDelayFix1, GetEngine, 0x4B06A);
	SET_PTR(P::NPrintfDelayFix2, GetEngine, 0x4B06C);

	SET_PTR(P::WorldPrecache, GetEngine, 0x17E500);
	SET_PTR(P::WorldPrecacheCall, GetEngine, 0x1F8610);

	SET_PTR(P::UnlockGodmodeFunc, GetModLib, 0xEB227);
	SET_PTR(P::UnlockNoclipFunc, GetModLib, 0xEB20C);

	SET_PTR(P::sv_numedicts, GetEngine, 0x008724F8);
	SET_DEREF_PTR(P::sv_edicts, GetEngine, 0x00872500);
}