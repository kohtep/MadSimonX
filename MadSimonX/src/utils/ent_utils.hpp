#pragma once

#include "core/global.hpp"

// Utility
namespace U::Ent
{
	using IterFunc_t = bool(*)(const char *name, void *addr, void *table_addr, void *arg);

	extern void Spawn(const char *name, const Vector &pos);
	extern void Despawn(bool bFullDelete);

	extern void Enumerate(IterFunc_t pfnEnumer, void *arg);
}