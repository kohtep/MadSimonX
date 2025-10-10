#pragma once

#include "memoria_common.hpp"

#include <stdint.h>

MEMORIA_BEGIN

extern bool WriteMemory(void *addr, const void *data, size_t size, ptrdiff_t offset = 0, bool use_setmem = false);

extern bool WriteU8(void *addr, uint8_t value, ptrdiff_t offset = 0);
extern bool WriteU16(void *addr, uint16_t value, ptrdiff_t offset = 0);
extern bool WriteU24(void *addr, uint32_t value, ptrdiff_t offset = 0);
extern bool WriteU24(void *addr, const int8_t value[3], ptrdiff_t offset = 0);
extern bool WriteU32(void *addr, uint32_t value, ptrdiff_t offset = 0);
extern bool WriteU64(void *addr, uint64_t value, ptrdiff_t offset = 0);

extern bool WriteI8(void *addr, int8_t value, ptrdiff_t offset = 0);
extern bool WriteI16(void *addr, int16_t value, ptrdiff_t offset = 0);
extern bool WriteI24(void *addr, int32_t value, ptrdiff_t offset = 0);
extern bool WriteI24(void *addr, const int8_t value[3], ptrdiff_t offset = 0);
extern bool WriteI32(void *addr, int32_t value, ptrdiff_t offset = 0);
extern bool WriteI64(void *addr, int64_t value, ptrdiff_t offset = 0);

extern bool WriteFloat(void *addr, float value, ptrdiff_t offset = 0);
extern bool WriteDouble(void *addr, double value, ptrdiff_t offset = 0);
extern bool WritePointer(void *addr, const void *value, ptrdiff_t offset = 0);
extern bool WriteRelative(void *addr, const void *value, ptrdiff_t offset = 0);

extern bool WriteAStr(void *addr, const char *value, ptrdiff_t offset = 0);
extern bool WriteWStr(void *addr, const wchar_t *value, ptrdiff_t offset = 0);

extern bool FillChar(void *addr, int value, size_t size);
extern bool FillNops(void *addr, size_t size);

MEMORIA_END