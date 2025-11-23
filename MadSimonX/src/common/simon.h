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
	edict_t *_entity;
	CBasePlayer *_player;

	Vector _eyes;

	Vector _origin;
	Vector _angles;

	float _height;

private:
	void CalcGroundDistance();
	void PrintDebugInfo();

public:
	static CSimon &Instance();

	void Update();
	void Reset();

	bool IsValid() const { return (_entity != nullptr); }
	bool IsOnGround() const;
	bool IsSwimming() const;

	void SetHealth(float value);

	float GetHealth() const;
	float GetHeight() const;

	CBasePlayer *GetPlayer() const;
	edict_t *GetEntity() const;

	Vector GetEyes() const;
	Vector GetViewTraceEnd();
	edict_t *GetViewTraceEntity();

	void GetViewVectors(Vector *forward, Vector *right = nullptr, Vector *up = nullptr) const;

	void GiveItem(const char *item_name);
};