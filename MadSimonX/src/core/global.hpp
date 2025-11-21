#pragma once

#include <Windows.h>

#include "utils/mdl_utils.hpp"

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

struct IBaseSystem;

struct IGameUI007
{
	virtual void Create(bool Dispose) = 0;
	virtual bool Initialize(CreateInterfaceFn *factories, int count) = 0;
	virtual void Start(cl_enginefuncs_s *engineFuncs, int interfaceVersion, IBaseSystem *system) = 0;
	virtual void Shutdown() = 0;
	virtual bool ActivateGameUI() = 0;
	virtual bool ActivateDemoUI() = 0;
	virtual bool HasExclusiveInput() = 0;
	virtual void RunFrame() = 0;
	virtual void ConnectToServer(const char *game, int IP, int port) = 0;
	virtual void DisconnectFromServer() = 0;
	virtual void HideGameUI() = 0;
	virtual bool IsGameUIActive() = 0;
	virtual void LoadingStarted(const char *resourceType, const char *resourceName) = 0;
	virtual void LoadingFinished(const char *resourceType, const char *resourceName) = 0;
	virtual void StartProgressBar(const char *progressType, int progressSteps) = 0;
	virtual int ContinueProgressBar(int progressPoint, float progressFraction) = 0;
	virtual void StopProgressBar(bool bError, const char *failureReason, const char *extendedReason) = 0;
	virtual int SetProgressBarStatusText(const char *statusText) = 0;
	virtual void SetSecondaryProgressBar(float progress) = 0;
	virtual void SetSecondaryProgressBarText(const char *statusText) = 0;
	virtual void ValidateCDKey(bool force, bool inConnect) = 0;
	virtual void OnDisconnectFromServer(int eSteamLoginFailure, const char *username) = 0;
};

struct IGame
{
	virtual ~IGame() = 0;
	virtual bool Init(void *pvInstance) = 0;
	virtual bool Shutdown() = 0;
	virtual bool CreateGameWindow() = 0;
	virtual void SleepUntilInput(int time) = 0;
	virtual HWND GetMainWindow() = 0;
	virtual HWND *GetMainWindowAddress() = 0;
	virtual void SetWindowXY(int x, int y) = 0;
	virtual void SetWindowSize(int w, int h) = 0;
	virtual void GetWindowRect(int *x, int *y, int *w, int *h) = 0;
	virtual bool IsActiveApp() = 0;
	virtual bool IsMultiplayer() = 0;
	virtual void PlayStartupVideos() = 0;
	virtual void PlayAVIAndWait(const char *aviFile) = 0;
	virtual void SetCursorVisible(bool bState) = 0;
};

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
	extern U::Memory::module_t &GetEngine(); // hw.dll
	extern U::Memory::module_t &GetModLib(); // hl.dll
	extern U::Memory::module_t &GetClient(); // client.dll
	extern U::Memory::module_t &GetGameUI(); // GameUI.dll
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
		using WorldPrecache_t = void(__fastcall *)(void *self);

		using GetCursorPos_t = BOOL(WINAPI *)(LPPOINT lpPoint);
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

	inline IGame *game;
	inline IGameUI007 *gameui;

	inline WNDPROC WndProc;

	inline int *mx_accum;
	inline int *my_accum;
}

extern void InitCoreGlobals();
extern void InitGlobals();

extern void InitConCmds();

#define STRING(offset) ((const char *)(&(*P::gGlobals)->pStringBase[offset]))
#define MAKE_STRING(str) ((unsigned long long)(str) - (unsigned long long)STRING(0))