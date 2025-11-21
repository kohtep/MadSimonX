#pragma once

#include <stdint.h>

namespace U::Memory
{
	extern void *PtrOffset(const void *addr, ptrdiff_t offset, bool dereference = false);

	extern uint8_t *FindU8(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value, bool backward = false, ptrdiff_t offset = 0);
	extern uint16_t *FindU16(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t value, bool backward = false, ptrdiff_t offset = 0);
	extern uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value[3], bool backward = false, ptrdiff_t offset = 0);
	extern uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward = false, ptrdiff_t offset = 0);
	extern uint32_t *FindU32(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward = false, ptrdiff_t offset = 0);

	extern bool WriteMemory(void *addr, const void *data, size_t size, ptrdiff_t offset = 0, bool use_setmem = false);
	extern bool WriteU8(void *addr, uint8_t value, ptrdiff_t offset = 0);
	extern bool WriteU16(void *addr, uint16_t value, ptrdiff_t offset = 0);
	extern bool WriteU24(void *addr, uint32_t value, ptrdiff_t offset = 0);
	extern bool WriteU24(void *addr, const int8_t value[3], ptrdiff_t offset = 0);
	extern bool WriteU32(void *addr, uint32_t value, ptrdiff_t offset = 0);
	extern bool WritePointer(void *addr, const void *value, ptrdiff_t offset = 0);

	extern bool CheckU8(const void *addr, uint8_t value, ptrdiff_t offset = 0);
	extern bool CheckU16(const void *addr, uint16_t value, ptrdiff_t offset = 0);
	extern bool CheckU24(const void *addr, uint32_t value, ptrdiff_t offset = 0);
	extern bool CheckU24(const void *addr, const uint8_t value[3], ptrdiff_t offset = 0);
	extern bool CheckU32(const void *addr, uint32_t value, ptrdiff_t offset = 0);

	extern void *Advance(void *pAddr, ptrdiff_t nOffset, bool bDereference = false);

	extern void *Resolve(void *pAddr, ptrdiff_t nPreOffset, ptrdiff_t nPostOffset);
	extern void *Resolve(void *pAddr, ptrdiff_t nOffset = 0);

	extern bool Splice(void *splice_addr, void *jump_to, void *out_tramp, bool place_call);
	extern bool SpliceAPI(const char *modname, const char *funcname, void *jump_to, void *out_tramp, bool place_call);
	extern bool Unsplice(void *tramp);

	extern bool IsInBounds(uintptr_t addr, uintptr_t addr_lower, uintptr_t addr_upper);
	extern bool IsInBounds(const void *addr, const void *addr_lower, const void *addr_upper);

	extern bool FillChar(void *addr, int value, size_t size);
	extern bool FillNops(void *addr, size_t size);

	extern bool IsDllLoaded(const char *name);

	extern bool CreateMemoryBackup(const void *address, size_t size);
	extern bool RestoreMemoryBackups();
}