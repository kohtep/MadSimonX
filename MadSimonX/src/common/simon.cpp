#include "simon.h"

#include "shared/build_number.h"

#include "utils/nprintf.hpp"
#include "common/concmds.hpp"

using namespace G;

class CCofComputer : public CBaseEntity
{
public:
	char m_sUserName[64];
	char m_sPassword[64];
};

void CSimon::DebugHeader()
{
	U::NPrintf::Add();
	U::NPrintf::Add();
	U::NPrintf::Add();

	U::NPrintf::Add("MadSimonX / kohtep");
	U::NPrintf::Add("Build: %d", BUILD_NUM);
	U::NPrintf::Add();
}

void CSimon::DebugSimon()
{
	U::NPrintf::Add(".::  Simon Info  ::.");
	U::NPrintf::Add("Health:  %3.f", m_Entity->v.health);
	U::NPrintf::Add("Stamina: %3.f", m_Player->m_fStamina);
	U::NPrintf::Add("Speed:   %3.f", m_Entity->v.velocity.Length2D());
	U::NPrintf::Add("Armor:   %3.f", m_Player->pev->armorvalue);

	U::NPrintf::Add();
}

void CSimon::DebugWeapon()
{
	auto item = m_Player->m_pActiveItem;
	if (item)
	{
		U::NPrintf::Add(".::  Weapon Info  ::.");
		U::NPrintf::Add("Name: %s", STRING(item->pev->classname));
		U::NPrintf::Add("Id: %d", item->m_iId);
		U::NPrintf::Add("LFlags: %08X", item->m_iLFlags);
		U::NPrintf::Add("Flags: %08X", item->pev->flags);
		U::NPrintf::Add();
	}
}

void CSimon::DebugMap()
{
	U::NPrintf::Add(".::  Map Info  ::.");

	char levelname[64];
	char *plevelname;

	strcpy_s(levelname, Engine.pfnGetLevelName());
	char *ext = strrchr(levelname, '.');
	if (ext)
		*ext = '\0';
	plevelname = strchr(levelname, '/');
	if (plevelname)
		plevelname++;
	else
		plevelname = levelname;

	U::NPrintf::Add("Name: %s", plevelname);
	U::NPrintf::Add();
}

void CSimon::DebugThisDude()
{
	auto dbp = P::UTIL_FindEntityByString(nullptr, "classname", "dave_benson_phillips");
	if (dbp)
	{
		U::NPrintf::Add(".::  This Dude  ::.");
		U::NPrintf::Add("pev->iuser1: %d", dbp->pev->iuser1);
		U::NPrintf::Add();
	}
}

void CSimon::DebugComputer()
{
	auto pComputer = (CCofComputer *)P::UTIL_FindEntityByString(nullptr, "classname", "cof_computer");
	if (pComputer)
	{
		U::NPrintf::Add(".::  Computer  ::.");
		U::NPrintf::Add("Username: %s", pComputer->m_sUserName);
		U::NPrintf::Add("Password: %s", pComputer->m_sPassword);
		U::NPrintf::Add();
	}
}

CSimon::CSimon()
{
	m_Entity = nullptr;
	m_Player = nullptr;

	m_Eyes = {};

	m_Origin = {};
	m_Angles = {};

	m_ShowInfo = nullptr;
}

void CSimon::Think()
{
	//
	// Before real thinking we must find all Simon's stuff.
	//
	// If we have not found something yet, then we should try
	// to find it and wait for the next 'think' request, that's
	// why we do 'return' after every try.
	//

	if (!m_ShowInfo)
	{
		m_ShowInfo = Engine.pfnRegisterVariable("simon_showinfo", "1", 0);
	}

	//
	// Simon can't think until connected to the game server.
	//

	if (!m_Entity)
	{
		m_Entity = (edict_t *)Server.pfnPEntityOfEntIndex(1);
		return;
	}
	
	if (!m_Player)
	{
		auto data = (CBasePlayer *)m_Entity->pvPrivateData;

		if (!data)
		{
			//
			// CBasePlayer class is not initialized yet, we need to
			// wait a little.
			//

			return;
		}

		m_Player = data;
		return;
	}

	//
	// Now we can think about some really important things.
	//

	Vector angles;
	Engine.GetViewAngles(angles);

	m_Origin = m_Entity->v.origin;
	m_Angles = angles;

	//
	// Get Simon's eyes position.
	//

	m_Eyes = m_Entity->v.origin + Vector(0.0f, 0.0f, 20.0f);

	//
	// Always perform 'first deploy' animation for every
	// weapon.
	//

	if (C::always_first_deploy->value > 0.0f)
	{
		memset(m_Player->m_bFirstDeploy, 0, sizeof(m_Player->m_bFirstDeploy));
	}

	//
	// It can be optimized, I know, but I don't want to implement
	// a better way to set these values right now.
	//

	m_Player->m_bInfiniteAmmo    = C::infinite_ammo->value > 0.0f;
	m_Player->m_bInfiniteHealth  = C::infinite_health->value > 0.0f;
	m_Player->m_bInfiniteStamina = C::infinite_stamina->value > 0.0f;

	//m_Player->pev->maxspeed = m_Player->pev->armorvalue > 0.0f ? 65.0f : 75.0f;

	if (m_ShowInfo->value > 0.0)
	{
		DebugHeader();
		DebugSimon();
		DebugMap();
		DebugWeapon();
		//DebugThisDude();
		DebugComputer();
	}
}

void CSimon::Flush()
{
	m_Entity = nullptr;
	m_Player = nullptr;
}

Vector CSimon::TraceEyes()
{
	Vector forward;
	Engine.pfnAngleVectors(m_Angles, forward, nullptr, nullptr);

	pmtrace_t tr;
	G::Engine.pEventAPI->EV_SetTraceHull(2);
	G::Engine.pEventAPI->EV_PlayerTrace(m_Eyes, m_Eyes + forward * 8192.0f, PM_GLASS_IGNORE, -1, &tr);

	return tr.endpos;
}

edict_t *ENT(entvars_t *pev)
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

edict_t *CSimon::TraceEntity()
{
	Vector forward;
	Engine.pfnAngleVectors(m_Angles, forward, nullptr, nullptr);

	TraceResult tr;
	
	auto pent = (edict_t *)ENT(m_Player->pev);
	if (!pent)
		return pent;

	Server.pfnTraceLine(m_Eyes, m_Eyes + forward * 8192.0f, 0, (edict_t *)pent, &tr);

	// prohibit the world tracing
	if (tr.pHit->v.origin.x == 0.0f && tr.pHit->v.origin.y == 0.0f && tr.pHit->v.origin.z == 0.0f)
		return nullptr;

	return (edict_t *)tr.pHit;
}

CSimon &CSimon::Instance()
{
	static CSimon Simon{};
	return Simon;
}