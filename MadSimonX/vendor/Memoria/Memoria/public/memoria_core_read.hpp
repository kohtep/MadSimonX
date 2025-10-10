#pragma once

#include "memoria_common.hpp"
#include "memoria_core_signature.hpp"

#include <stdint.h>
#include <span>

#include "memoria_utils_optional.hpp"

MEMORIA_BEGIN

extern Memoria::Optional<uint8_t> ReadU8(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<uint16_t> ReadU16(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<uint32_t> ReadU24(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<uint32_t> ReadU32(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<uint64_t> ReadU64(const void *addr, ptrdiff_t offset = 0);

extern Memoria::Optional<int8_t> ReadI8(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<int16_t> ReadI16(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<int32_t> ReadI24(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<int32_t> ReadI32(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<int64_t> ReadI64(const void *addr, ptrdiff_t offset = 0);

extern Memoria::Optional<float> ReadFloat(const void *addr, ptrdiff_t offset = 0);
extern Memoria::Optional<double> ReadDouble(const void *addr, ptrdiff_t offset = 0);

extern bool ReadAStr(const void *addr, char *out, size_t max_size, ptrdiff_t offset = 0);
extern bool ReadWStr(const void *addr, wchar_t *out, size_t max_size, ptrdiff_t offset = 0);

extern bool GetMemoryBlock(const void *source, size_t size, void *dest);
extern Memoria::Vector<uint8_t> GetMemoryData(const void *source, size_t size);

extern std::span<const uint8_t> GetMemorySpan(const void *source, size_t size);
extern std::span<uint8_t> GetMemorySpan(void *source, size_t size);

MEMORIA_END