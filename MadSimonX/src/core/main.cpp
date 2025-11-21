#include <Windows.h>

#include "core/global.hpp"
#include "core/hooks.hpp"

#include "common/concmds.hpp"
#include "common/rawinput.hpp"

#include "utils/mem_utils.hpp"

static void EntryMain(HINSTANCE hinstDLL, LPVOID lpReserved)
{
	//
	// When loading MadSimon using the CrashRpt.dll method, most data
	// is not yet ready.
	//
	// Therefore, during the loading stage we obtain the main available information
	// and set up deferred initialization via the HUD_Frame callback,
	// which is invoked when all data becomes available.
	//

	InitCoreGlobals();

	ApplyCoreHooks();
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		EntryMain(hinstDLL, lpReserved);
		break;

	case DLL_PROCESS_DETACH:
		RestoreHooks();
		break;
	}

	return TRUE;
}