#include <Windows.h>

#include "core/global.hpp"
#include "core/hooks.hpp"
#include "common/concmds.hpp"

static void EntryMain(HINSTANCE hinstDLL, LPVOID lpReserved)
{
	Memoria::Startup();

	InitGlobals();
	InitPointers();

	memcpy(&G::Engine, P::Engine, sizeof(G::Engine));
	memcpy(&G::Server, P::Server, sizeof(G::Server));
	memcpy(&G::Client, P::Client, sizeof(G::Client));
	memcpy(&G::EntityInterface, P::EntityInterface, sizeof(G::EntityInterface));

	InitConCmds();
	InitHooks();

	G::Engine.Con_Printf("MadSimonX successfully initialized.\n");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		EntryMain(hinstDLL, lpReserved);
	}

	return TRUE;
}