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

	void Update();
	void Reset();

	bool IsValid() const { return (m_Entity != nullptr); }

	void SetHealth(float value) { if (m_Entity) m_Entity->v.health = value; }
	float GetHealth() { return m_Entity ? m_Entity->v.health : -1.0f; }

	auto GetPlayer() { return m_Player; }
	auto GetEntity() { return m_Entity; }

	Vector GetEyes() const;
	Vector GetViewTraceEnd();
	edict_t *GetViewTraceEntity();

	void GetViewVectors(Vector *forward, Vector *right = nullptr, Vector *up = nullptr) const;

	void GiveItem(const char *item_name);
};