#include "memoria_common.hpp"

#include "memoria_ext_patch.hpp"
#include "memoria_utils_vector.hpp"

MEMORIA_BEGIN

static Memoria::FixedVector<void(*)(), 64> ExitCallbacks{};

bool Startup()
{
	return true;
}

bool Cleanup()
{
	for (auto cb : ExitCallbacks)
		cb();

	return true;
}

void RegisterOnExitCallback(void(*cb)())
{
	AssertMsg(cb, "Callback cannot be null.");

	if (cb) ExitCallbacks.push_back(cb);
}

MEMORIA_END