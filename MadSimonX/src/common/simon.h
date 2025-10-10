#pragma once

#include "core/global.hpp"

class CSimon
{
	CSimon();
	~CSimon() = default;
	CSimon(const CSimon &other) = default;
	CSimon &operator=(const CSimon &other) = default;
	CSimon(CSimon &&other) noexcept = default;
	CSimon &operator=(CSimon &&other) noexcept = default;

private:
	edict_t *m_Entity;
	CBasePlayer *m_Player;

	Vector m_Eyes;

	Vector m_Origin;
	Vector m_Angles;

	cvar_t *m_ShowInfo;

private:
	void DebugHeader();
	void DebugSimon();
	void DebugWeapon();
	void DebugMap();
	void DebugThisDude();
	void DebugComputer();

public:
	static CSimon &Instance();

	void Think();
	void Flush();

	void SetHealth(float value) { if (m_Entity) m_Entity->v.health = value; }
	float GetHealth() { return m_Entity ? m_Entity->v.health : -1.0f; }

	auto Player() { return m_Player; }
	auto Entity() { return m_Entity; }

	Vector TraceEyes();
	edict_t *TraceEntity();
};