#include "simon.h"

#include "shared/build_number.h"

#include "utils/nprintf.hpp"
#include "common/concmds.hpp"

using namespace G;

static edict_t *ENT(entvars_t *pev)
{
	if (pev->pContainingEntity)
		return (edict_t *)pev->pContainingEntity;
	else
	{
		Server.pfnAlertMessage(at_console, "entvars_t pContainingEntity is NULL, calling into engine\n");

		auto pent = Server.pfnFindEntityByVars((entvars_t *)pev);
		if (pent)
		{
			pev->pContainingEntity = (edict_t *)pent;
			return (edict_t *)pent;
		}

		Server.pfnAlertMessage(at_console, "DAMN!  Even the engine couldn't FindEntityByVars!\n");
		return nullptr;
	}
}

class CCofComputer : public CBaseEntity
{
public:
	char m_sUserName[64];
	char m_sPassword[64];
};

CSimon::CSimon() : _origin{}, _angles{}, _eyes{}
{
	_entity = nullptr;
	_player = nullptr;

	_height = -1.0f;
}

void CSimon::Update()
{
	//
	// Before real thinking we must find all Simon's stuff.
	//
	// If we have not found something yet, then we should try
	// to find it and wait for the next 'think' request, that's
	// why we do 'return' after every try.
	//

	//
	// Simon can't think until connected to the game server.
	//

	if (!_entity)
	{
		_entity = (edict_t *)Server.pfnPEntityOfEntIndex(1);
		return;
	}
	
	if (!_player)
	{
		auto data = (CBasePlayer *)_entity->pvPrivateData;

		if (!data)
		{
			//
			// CBasePlayer class is not initialized yet, we need to
			// wait a little.
			//

			return;
		}

		_player = data;
		return;
	}

	//
	// Now we can think about some really important things.
	//

	Vector angles;
	Engine.GetViewAngles(angles);

	_origin = _entity->v.origin;
	_angles = angles;

	//
	// Get Simon's eyes position.
	//

	_eyes = _entity->v.origin + Vector(0.0f, 0.0f, 20.0f);

	//
	// Always perform 'first deploy' animation for every
	// weapon.
	//

	if (C::always_first_deploy && C::always_first_deploy->value > 0.0f)
	{
		memset(_player->m_bFirstDeploy, 0, sizeof(_player->m_bFirstDeploy));
	}

	//
	// It can be optimized, I know, but I don't want to implement
	// a better way to set these values right now.
	//

	_player->m_bInfiniteAmmo    = C::infinite_ammo->value > 0.0f;
	_player->m_bInfiniteHealth  = C::infinite_health->value > 0.0f;
	_player->m_bInfiniteStamina = C::infinite_stamina->value > 0.0f;

	CalcGroundDistance();

	if (C::show_info && C::show_info->value > 0.0f)
	{
		PrintDebugInfo();
	}
}

void CSimon::Reset()
{
	_entity = nullptr;
	_player = nullptr;
}

Vector CSimon::GetViewTraceEnd()
{
	Vector forward;
	Engine.pfnAngleVectors(_angles, forward, nullptr, nullptr);

	pmtrace_t tr;
	G::Engine.pEventAPI->EV_SetTraceHull(2);
	G::Engine.pEventAPI->EV_PlayerTrace(_eyes, _eyes + forward * 8192.0f, PM_GLASS_IGNORE, -1, &tr);

	return tr.endpos;
}

edict_t *CSimon::GetViewTraceEntity()
{
	Vector forward;
	Engine.pfnAngleVectors(_angles, forward, nullptr, nullptr);

	TraceResult tr;
	
	auto pent = (edict_t *)ENT(_player->pev);
	if (!pent)
		return pent;

	Server.pfnTraceLine(_eyes, _eyes + forward * 8192.0f, 0, (edict_t *)pent, &tr);

	// prohibit the world tracing
	if (tr.pHit->v.origin.x == 0.0f && tr.pHit->v.origin.y == 0.0f && tr.pHit->v.origin.z == 0.0f)
		return nullptr;

	return (edict_t *)tr.pHit;
}

void CSimon::GiveItem(const char *item_name)
{
	P::GiveNamedItem(_player, 0, item_name, false, false);
}

bool CSimon::IsOnGround() const
{
	return (_entity->v.flags & FL_ONGROUND);
}

bool CSimon::IsSwimming() const
{
	return (_entity->v.waterlevel >= 2);
}

void CSimon::SetHealth(float value)
{
	if (_entity)
		_entity->v.health = value;
}

float CSimon::GetHealth() const
{
	return _entity ? _entity->v.health : -1.0f;
}

float CSimon::GetHeight() const
{
	return _height;
}

CBasePlayer *CSimon::GetPlayer() const
{
	return _player;
}

edict_t *CSimon::GetEntity() const
{
	return _entity;
}

Vector CSimon::GetEyes() const
{
	return _eyes;
}

void CSimon::GetViewVectors(Vector *forward, Vector *right, Vector *up) const
{
	Engine.pfnAngleVectors(_angles, 
		reinterpret_cast<float *>(forward), 
		reinterpret_cast<float *>(right), 
		reinterpret_cast<float *>(up));
}

void CSimon::PrintDebugInfo()
{
	U::NPrintf::Add();
	U::NPrintf::Add();
	U::NPrintf::Add();

	U::NPrintf::Add("MadSimonX / kohtep");
	U::NPrintf::Add("Build: %d", build_number());
	U::NPrintf::Add();

	U::NPrintf::Add(".::  Simon Info  ::.");
	U::NPrintf::Add("Health:  %3.f", _entity->v.health);
	U::NPrintf::Add("Stamina: %3.f", _player->m_fStamina);
	U::NPrintf::Add("Speed:   %3.f", _entity->v.velocity.Length2D());
	U::NPrintf::Add("Armor:   %3.f", _player->pev->armorvalue);
	U::NPrintf::Add();

	auto item = _player->m_pActiveItem;
	if (item)
	{
		U::NPrintf::Add(".::  Weapon Info  ::.");
		U::NPrintf::Add("Name: %s", STRING(item->pev->classname));
		U::NPrintf::Add("Id: %d", item->m_iId);
		U::NPrintf::Add("LFlags: %08X", item->m_iLFlags);
		U::NPrintf::Add("Flags: %08X", item->pev->flags);
		U::NPrintf::Add();
	}

	U::NPrintf::Add(".::  Map Info  ::.");
	char levelname[64];
	char *plevelname;
	strcpy_s(levelname, Engine.pfnGetLevelName());
	char *ext = strrchr(levelname, '.');
	if (ext) *ext = '\0';
	plevelname = strchr(levelname, '/');
	if (plevelname) plevelname++; else plevelname = levelname;
	U::NPrintf::Add("Name: %s", plevelname);
	U::NPrintf::Add();

	auto dbp = P::UTIL_FindEntityByString(nullptr, "classname", "dave_benson_phillips");
	if (dbp)
	{
		U::NPrintf::Add(".::  This Dude  ::.");
		U::NPrintf::Add("pev->iuser1: %d", dbp->pev->iuser1);
		U::NPrintf::Add();
	}

	auto pComputer = (CCofComputer *)P::UTIL_FindEntityByString(nullptr, "classname", "cof_computer");
	if (pComputer)
	{
		U::NPrintf::Add(".::  Computer  ::.");
		U::NPrintf::Add("Username: %s", pComputer->m_sUserName);
		U::NPrintf::Add("Password: %s", pComputer->m_sPassword);
		U::NPrintf::Add();
	}
}

void CSimon::CalcGroundDistance()
{
	if (!_player || !_player->pev)
	{
		_height = -1.0f;
		return;
	}

	edict_t *pent = (edict_t *)ENT(_player->pev);
	if (!pent)
	{
		_height = -1.0f;
		return;
	}

	TraceResult tr, tr2;

	Vector start = pent->v.origin;
	start.z += pent->v.mins.z;

	Vector end = start;
	end.z = -8192.0f;

	G::Server.pfnTraceLine(start, end, 0, pent, &tr);
	float height = start.z - tr.vecEndPos.z;

	int hitIndex = 0;
	if (tr.flFraction < 1.0f && tr.pHit)
		hitIndex = G::Server.pfnIndexOfEdict(tr.pHit);

	G::Server.pfnTraceLine(start, end, 1, pent, &tr2);
	float h2 = start.z - tr2.vecEndPos.z;
	if (tr2.flFraction < 1.0f && h2 < height)
		height = h2;

	int maxClients = G::Engine.GetMaxClients();
	if (hitIndex > 0 && hitIndex <= maxClients && tr2.flFraction < 1.0f)
	{
		edict_t *ent = G::Server.pfnPEntityOfEntIndex(hitIndex);
		if (ent)
			height = start.z - (ent->v.origin.z + ent->v.maxs.z);
	}

	if (height > 0.0f && (pent->v.flags & FL_ONGROUND))
		height = 0.0f;

	_height = height;
}

CSimon &CSimon::Instance()
{
	static CSimon Simon{};
	return Simon;
}