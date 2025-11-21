#include "ent_utils.hpp"

#include "common/simon.h"

#include "utils/mem_utils.hpp"

void U::Ent::Spawn(const char *name, const Vector &pos)
{
	if (!name || !*name)
	{
		G::Engine.Con_Printf("Incorrect entity name.\n");
		return;
	}

	G::Engine.Con_DPrintf("Creating '%s'...\n", name);

	//
	// Creating named entity via CBaseEntity static method.
	//
	// _strdup is needed because pfnCreateEntity stores 'name' address
	// in pev->classname variable, so data at this address should be
	// constant.
	//

	if (!P::CBaseEntity_Create(_strdup(name), &pos, &G::ZeroVec, nullptr))
	{
		G::Engine.Con_Printf("Could not create entity '%s'.\n", name);
	}
}

void U::Ent::Despawn(bool bFullDelete)
{
	auto edict = CSimon::Instance().TraceEntity();
	if (!edict)
	{
		G::Engine.Con_Printf("Could not find entity.\n");
		return;
	}

	auto ent = (CBaseEntity *)edict->pvPrivateData;
	ent->Killed((entvars_t *)&CSimon::Instance().Entity()->v, 0);

	if (bFullDelete)
		P::UTIL_Remove(ent);
}

void U::Ent::Enumerate(U::Ent::IterFunc_t pfnEnumer, void *arg)
{
	if (!pfnEnumer)
		return;

	auto dos = (IMAGE_DOS_HEADER *)M::GetModLib().Handle;
	auto nt = (IMAGE_NT_HEADERS *)(U::Memory::PtrOffset(dos, dos->e_lfanew));
	auto dir = (IMAGE_EXPORT_DIRECTORY *)(U::Memory::PtrOffset(dos, nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));

	auto nameTable = (unsigned int *)((int)dos + dir->AddressOfNames);
	auto funcTable = (unsigned int *)((int)dos + dir->AddressOfFunctions);
	auto ordTable = (unsigned short *)((int)dos + dir->AddressOfNameOrdinals);

	for (auto i = 0u; i < dir->NumberOfNames; i++)
	{
		auto curName = (char *)((int)dos + *(int *)(nameTable));
		auto curFunc = (void *)((int)dos + *(int *)(funcTable));
		if (!pfnEnumer(curName, curFunc, nullptr, arg))
			break;

		nameTable++;
		funcTable++;
		ordTable++;
	}
}