#pragma once

#include "memoria_common.hpp"

MEMORIA_BEGIN

struct MemoriaContext_t
{
	// All functions that work with memory check its validity before performing any operations.
	// This can be slow, so a parameter called SafeMode was introduced,
	// which verifies all pointers for validity before starting work.
	//
	// It is recommended to disable this if you're confident that the memory is guaranteed to be valid.
	bool SafeMode = true;

	MemoriaContext_t() = default;
};

static MemoriaContext_t memoria_ctx;

void SetSafeModeState(bool value)
{
	memoria_ctx.SafeMode = value;
}

bool IsSafeModeActive()
{
	return memoria_ctx.SafeMode;
}

MEMORIA_END