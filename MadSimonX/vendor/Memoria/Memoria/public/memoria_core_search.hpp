#pragma once

#include "memoria_common.hpp"

#include "memoria_utils_vector.hpp"
#include "memoria_core_signature.hpp"

#include <stdint.h>

MEMORIA_BEGIN

extern uint8_t  *FindU8(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value, bool backward = false, ptrdiff_t offset = 0);
extern uint16_t *FindU16(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t value, bool backward = false, ptrdiff_t offset = 0);
extern uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value[3], bool backward = false, ptrdiff_t offset = 0);
extern uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward = false, ptrdiff_t offset = 0);
extern uint32_t *FindU32(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward = false, ptrdiff_t offset = 0);
extern uint64_t *FindU64(const void *addr_start, const void *addr_min, const void *addr_max, uint64_t value, bool backward = false, ptrdiff_t offset = 0);

extern int8_t  *FindI8(const void *addr_start, const void *addr_min, const void *addr_max, int8_t value, bool backward = false, ptrdiff_t offset = 0);
extern int16_t *FindI16(const void *addr_start, const void *addr_min, const void *addr_max, int16_t value, bool backward = false, ptrdiff_t offset = 0);
extern int32_t *FindI24(const void *addr_start, const void *addr_min, const void *addr_max, int8_t value[3], bool backward = false, ptrdiff_t offset = 0);
extern int32_t *FindI24(const void *addr_start, const void *addr_min, const void *addr_max, int32_t value, bool backward = false, ptrdiff_t offset = 0);
extern int32_t *FindI32(const void *addr_start, const void *addr_min, const void *addr_max, int32_t value, bool backward = false, ptrdiff_t offset = 0);
extern int64_t *FindI64(const void *addr_start, const void *addr_min, const void *addr_max, int64_t value, bool backward = false, ptrdiff_t offset = 0);

extern float *FindFloat(const void *addr_start, const void *addr_min, const void *addr_max, float value, bool backward = false, ptrdiff_t offset = 0);
extern double *FindDouble(const void *addr_start, const void *addr_min, const void *addr_max, double value, bool backward = false, ptrdiff_t offset = 0);

extern void *FindBlock(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, size_t size, bool backward = false, ptrdiff_t offset = 0);
extern void *FindSignature(const void *addr_start, const void *addr_min, const void *addr_max, const CSignature &sig, bool backward = false, ptrdiff_t offset = 0);
extern void *FindSignature(const void *addr_start, const void *addr_min, const void *addr_max, const char *sig, bool backward = false, ptrdiff_t offset = 0);
extern void *FindFirstSignature(const void *addr_start, const void *addr_min, const void *addr_max, const Memoria::Vector<CSignature> &sig, bool backward = false, ptrdiff_t offset = 0);

struct Ref_t
{
	// xref points to 'void *' if true (e.g. CALL ref), 'int32_t' otherwise (mov REG, offset ref)
	bool is_absolute;

	// address of instruction (CALL ref, JMP ref, etc); points to opcode
	void *xref;

	// address itself
	void *ref;

	Ref_t() = delete;
	Ref_t(bool absolute, void *crossref, void *reference, ptrdiff_t offset) :
		is_absolute(absolute), xref(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(crossref) + offset)), ref(reference) {
	}
};

extern Memoria::Vector<Ref_t> FindReferences(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, uint16_t opcode,
	bool search_absolute, bool search_relative, bool stop_on_first_found = true, bool backward = false, ptrdiff_t pre_offset = sizeof(int32_t), ptrdiff_t offset = 0);

extern void *FindReference(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, uint16_t opcode, bool search_absolute, bool search_relative, 
	bool backward = false, ptrdiff_t pre_offset = sizeof(int32_t), ptrdiff_t offset = 0);

extern void *FindAStr(const void *addr_start, const void *addr_min, const void *addr_max, const char *data, bool backward = false, ptrdiff_t offset = 0);
extern void *FindWStr(const void *addr_start, const void *addr_min, const void *addr_max, const wchar_t *data, bool backward = false, ptrdiff_t offset = 0);

extern void *FindRelative(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t opcode = 0, size_t index = 0, bool backward = false, ptrdiff_t offset = 0);

MEMORIA_END