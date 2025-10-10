#pragma once

#include <Windows.h>

#include "memoria_amalgamation.hpp"

#include "hlsdk/dlls/extdll.h"

#include "hlsdk/extended/basetypes.h"
#include "hlsdk/extended/playerconst.h"

#include "hlsdk/extended/progdefs.h"
#include "hlsdk/extended/edict.h"

#include "hlsdk/cl_dll/wrect.h"
#include "hlsdk/cl_dll/cl_dll.h"
#include "hlsdk/engine/cdll_int.h"
#include "hlsdk/engine/progdefs.h"
#include "hlsdk/engine/eiface.h"
#include "hlsdk/engine/edict.h"
#include "hlsdk/engine/server_static.h"
#include "hlsdk/common/studio_event.h"
#include "hlsdk/common/entity_types.h"
#include "hlsdk/common/pmtrace.h"
#include "hlsdk/common/r_studioint.h"
#include "hlsdk/common/triangleapi.h"
#include "hlsdk/pm_shared/pm_defs.h"
#include "hlsdk/engine/studio.h"
#include "hlsdk/common/r_efx.h"
#include "hlsdk/common/event_flags.h"
#include "hlsdk/common/entity_state.h"
#include "hlsdk/common/parsemsg.h"
#include "hlsdk/common/const.h"
#include "hlsdk/common/com_model.h"
#include "hlsdk/common/net_api.h"
#include "hlsdk/common/event_api.h"
#include "hlsdk/common/ref_params.h"
#include "hlsdk/common/screenfade.h"
#include "hlsdk/common/interface.h"

#include "hlsdk/dlls/enginecallback.h"
#include "hlsdk/dlls/util.h"

#include "hlsdk/dlls/cbase.h"
#include "hlsdk/dlls/basemonster.h"
#include "hlsdk/dlls/weapons.h"
#include "hlsdk/dlls/player.h"

// Globals
namespace G
{
	extern HINSTANCE Instance;

	//
	// Copies of the structures. Not intended for interception.
	//

	extern cl_enginefunc_t Engine;
	extern cldll_func_t Client;
	extern enginefuncs_t Server;
	extern DLL_FUNCTIONS EntityInterface;

	// Small helper for convenient work with the HLSDK.
	inline const auto ZeroVec = Vector(0.0f, 0.0f, 0.0f);
}

// Modules
namespace M
{
	extern Memoria::CMemoryModule &GetEngine(); // hw.dll
	extern Memoria::CMemoryModule &GetModLib(); // hl.dll
}

// Pointers
namespace P
{
	namespace Internal
	{
		using UTIL_FindEntityByString_t = CBaseEntity * (*)(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue);
		using UTIL_ScreenShake_t = void (*)(Vector *center, float amplitude, float frequency, float duration, float radius);
		using GiveNamedItem_t = void(__fastcall *)(CBasePlayer *that, int, const char *pszName, bool deletewhendropped, bool _unk_16a_param);
		using ViewPunch_t = void(__fastcall *)(CBasePlayer *that, int, float p, float y, float r);
		using CBaseEntity_Create_t = bool(__cdecl *)(const char *szName, const Vector *vecOrigin, const Vector *vecAngles, edict_t *pentOwner);
		using UTIL_Remove_t = void(__cdecl *)(CBaseEntity *pEntity);
		using WorldPrecache_t = void(*)(void *self);
	}

	inline Internal::UTIL_FindEntityByString_t UTIL_FindEntityByString;
	inline Internal::UTIL_ScreenShake_t UTIL_ScreenShake;
	inline Internal::GiveNamedItem_t GiveNamedItem;
	inline Internal::ViewPunch_t ViewPunch;
	inline Internal::CBaseEntity_Create_t CBaseEntity_Create;
	inline Internal::UTIL_Remove_t UTIL_Remove;
	inline Internal::WorldPrecache_t WorldPrecache;

	//
	// Pointers to the original structures. They may be used for interception
	// in the future. It is recommended to use the non-P variants to ensure
	// direct function calls.
	//

	inline cl_enginefunc_t *Engine;
	inline cldll_func_t *Client;
	inline enginefuncs_t *Server;
	inline DLL_FUNCTIONS *EntityInterface;

	inline cactive_t *cstate;
	inline globalvars_t **gGlobals;

	inline void *BunnyHopSpeedUpBlockPtr;
	inline void *WorldPrecacheCall;
	inline void *NPrintfDelayFix1;
	inline void *NPrintfDelayFix2;
	inline void *UnlockGodmodeFunc;
	inline void *UnlockNoclipFunc;

	inline int *sv_numedicts;
	inline edict_s *sv_edicts;
}

extern void InitGlobals();
extern void InitPointers();
extern void InitConCmds();

#define STRING(offset) ((const char *)(&(*P::gGlobals)->pStringBase[offset]))
#define MAKE_STRING(str) ((unsigned long long)(str) - (unsigned long long)STRING(0))