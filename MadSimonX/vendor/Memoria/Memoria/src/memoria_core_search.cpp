#include "memoria_core_search.hpp"

#include "memoria_core_misc.hpp"
#include "memoria_core_errors.hpp"
#include "memoria_core_options.hpp"
#include "memoria_utils_assert.hpp"

#include "hde64.h"

MEMORIA_BEGIN

using FindMemoryCmp_t = bool(*)(const void *addr1, const void *addr2, size_t size, void *param);

static bool FindMemoryCmp(const void *addr1, const void *addr2, size_t size, void *param)
{
	return memcmp(addr1, addr2, size) == 0;
}

void *FindMemory(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, size_t size, bool backward, 
	ptrdiff_t offset = 0, FindMemoryCmp_t comparator = FindMemoryCmp, void *comparator_param = nullptr)
{
	Assert(addr_min != nullptr && addr_max != nullptr && addr_min <= addr_max);

	if (IsSafeModeActive())
	{
		if (!IsMemoryValid(addr_start) || !IsMemoryValid(addr_min) || !IsMemoryValid(addr_max))
		{
			SetError(ME_INVALID_MEMORY);
			return nullptr;
		}

		if (!IsMemoryValid(data))
		{
			SetError(ME_INVALID_MEMORY);
			return nullptr;
		}

		if (size == 0)
		{
			SetError(ME_INVALID_ARGUMENT);
			return nullptr;
		}
	}

	if (!comparator)
		comparator = FindMemoryCmp;

	const void *result = static_cast<const void *>(addr_start);
	addr_max = reinterpret_cast<const void *>(reinterpret_cast<intptr_t>(addr_max) - size);

	do
	{
		if (!IsInBounds(result, addr_min, addr_max))
			return nullptr;

		if (comparator(result, data, size, comparator_param))
			return reinterpret_cast<void *>(uintptr_t(result) + offset);

		if (backward)
			result = reinterpret_cast<void *>(uintptr_t(result) - 1);
		else
			result = reinterpret_cast<void *>(uintptr_t(result) + 1);

	} while (true);

	return nullptr;
}

uint8_t *FindU8(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint8_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

uint16_t *FindU16(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint16_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint8_t value[3], bool backward, ptrdiff_t offset)
{
	return static_cast<uint32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, 3, backward, offset));
}

uint32_t *FindU24(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return static_cast<uint32_t *>(FindMemory(addr_start, addr_min, addr_max, &bytes, 3, backward, offset));
}

uint32_t *FindU32(const void *addr_start, const void *addr_min, const void *addr_max, uint32_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

uint64_t *FindU64(const void *addr_start, const void *addr_min, const void *addr_max, uint64_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<uint64_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

int8_t *FindI8(const void *addr_start, const void *addr_min, const void *addr_max, int8_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<int8_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

int16_t *FindI16(const void *addr_start, const void *addr_min, const void *addr_max, int16_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<int16_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

int32_t *FindI24(const void *addr_start, const void *addr_min, const void *addr_max, int8_t value[3], bool backward, ptrdiff_t offset)
{
	return static_cast<int32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, 3, backward, offset));
}

int32_t *FindI24(const void *addr_start, const void *addr_min, const void *addr_max, int32_t value, bool backward, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return static_cast<int32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, 3, backward, offset));
}

int32_t *FindI32(const void *addr_start, const void *addr_min, const void *addr_max, int32_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<int32_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

int64_t *FindI64(const void *addr_start, const void *addr_min, const void *addr_max, int64_t value, bool backward, ptrdiff_t offset)
{
	return static_cast<int64_t *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

float *FindFloat(const void *addr_start, const void *addr_min, const void *addr_max, float value, bool backward, ptrdiff_t offset)
{
	return static_cast<float *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

double *FindDouble(const void *addr_start, const void *addr_min, const void *addr_max, double value, bool backward, ptrdiff_t offset)
{
	return static_cast<double *>(FindMemory(addr_start, addr_min, addr_max, &value, sizeof(value), backward, offset));
}

void *FindBlock(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, size_t size, bool backward, ptrdiff_t offset)
{
	return static_cast<double *>(FindMemory(addr_start, addr_min, addr_max, data, size, backward, offset));
}

void *FindSignature(const void *addr_start, const void *addr_min, const void *addr_max, const CSignature &sig, bool backward, ptrdiff_t offset)
{
	if (sig.HasOptionals())
	{
		return static_cast<void *>(FindMemory(addr_start, addr_min, addr_max, sig.GetPayload().data(), sig.GetPayload().size(), backward, offset,
			+[](const void *addr1, const void *addr2, size_t size, void *param) -> bool
			{
				return reinterpret_cast<CSignature *>(param)->Match(addr1);
			}, const_cast<CSignature *>(&sig)));
	}
	else
	{
		return static_cast<void *>(FindMemory(addr_start, addr_min, addr_max, sig.GetPayload().data(), sig.GetPayload().size(), backward, offset));
	}
}

void *FindSignature(const void *addr_start, const void *addr_min, const void *addr_max, const char *sig, bool backward, ptrdiff_t offset)
{
	if (!sig || !*sig)
		return nullptr;

	CSignature s(sig);
	return FindSignature(addr_start, addr_min, addr_max, s, backward, offset);
}

void *FindFirstSignature(const void *addr_start, const void *addr_min, const void *addr_max, const Memoria::Vector<CSignature> &sigs, bool backward, ptrdiff_t offset)
{
	for (const auto &sig : sigs)
	{
		if (auto result = FindSignature(addr_start, addr_min, addr_max, sig, backward, offset); result != nullptr)
			return result;
	}

	return nullptr;
}

Memoria::Vector<Ref_t> FindReferences(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, uint16_t opcode,
	bool search_absolute, bool search_relative, bool stop_on_first_found, bool backward, ptrdiff_t pre_offset, ptrdiff_t offset)
{
	Memoria::Vector<Ref_t> refs{};

	if (!search_absolute && !search_relative)
	{
		SetError(ME_INVALID_ARGUMENT);
		return refs;
	}

	if (IsSafeModeActive())
	{
		if (!IsMemoryValid(addr_start) || !IsMemoryValid(addr_min) || !IsMemoryValid(addr_max))
		{
			SetError(ME_INVALID_ARGUMENT);
			return refs;
		}

		if (!IsMemoryValid(data))
		{
			SetError(ME_INVALID_ARGUMENT);
			return refs;
		}
	}

	const bool has_opcode_header = (opcode != 0);
	const bool is_two_bytes_opcode = (has_opcode_header) && (opcode > 255);
	void *result = const_cast<void *>(addr_start);

	if (search_absolute)
		addr_max = PtrOffset(addr_max, -static_cast<signed>(sizeof(void *)));
	else
		addr_max = PtrOffset(addr_max, -static_cast<signed>(sizeof(int32_t)));

	do
	{
		if (!IsInBounds(result, addr_min, addr_max))
		{
			SetError(ME_NOT_FOUND);
			return refs;
		}

		void *addr_abs;
		void *addr_rel;

		ptrdiff_t offs = (has_opcode_header) ? (is_two_bytes_opcode ? 2 : 1) : (0);

		if (has_opcode_header)
		{
			if (is_two_bytes_opcode)
			{
				result = FindU16(result, addr_min, addr_max, opcode, backward);
			}
			else
			{
				result = FindU8(result, addr_min, addr_max, static_cast<uint8_t>(opcode), backward);
			}

			if (result == nullptr)
			{
				SetError(ME_NOT_FOUND);
				return refs;
			}

			result = PtrOffset(result, offs);
		}

		addr_abs = (search_absolute) ? (*static_cast<void **>(result)) : (nullptr);
		addr_rel = (search_relative) ? (RelToAbsEx(result, 0, pre_offset)) : (nullptr);

		if (search_absolute)
		{
			if (addr_abs == data)
			{
				refs.emplace_back(true, result, addr_abs, offset);
			}
		}

		if (search_relative)
		{
			if (addr_rel == data)
			{
				refs.emplace_back(false, result, addr_rel, offset);
			}
		}

		if (backward)
			result = PtrRewind(result, 1);
		else
			result = PtrAdvance(result, 1);
	} while (true);

	return refs;
}

void *FindReference(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, uint16_t opcode, 
	bool search_absolute, bool search_relative, bool backward, ptrdiff_t pre_offset, ptrdiff_t offset)
{
	auto refs = FindReferences(addr_start, addr_min, addr_max, data, opcode, search_absolute, search_relative, true, backward, pre_offset, offset);

	if (refs.empty())
	{
		SetError(ME_NOT_FOUND);
		return nullptr;
	}

	return refs.back().xref;
}

void *FindAStr(const void *addr_start, const void *addr_min, const void *addr_max, const char *data, bool backward, ptrdiff_t offset)
{
	return FindBlock(addr_start, addr_min, addr_max, data, (strlen(data) * sizeof(char)) + sizeof(char), backward, offset);
}

void *FindWStr(const void *addr_start, const void *addr_min, const void *addr_max, const wchar_t *data, bool backward, ptrdiff_t offset)
{
	return FindBlock(addr_start, addr_min, addr_max, data, (wcslen(data) * sizeof(wchar_t)) + sizeof(wchar_t), backward, offset);
}

// for easy reading
enum class hde64_flags_t : uint32_t
{
    ModRM         = 0x00000001,
    Sib           = 0x00000002,
    Imm8          = 0x00000004,
    Imm16         = 0x00000008,
    Imm32         = 0x00000010,
    Imm64         = 0x00000020,
    Disp8         = 0x00000040,
    Disp16        = 0x00000080,
    Disp32        = 0x00000100,
    Relative      = 0x00000200,

    Error         = 0x00001000,
    ErrorOpcode   = 0x00002000,
    ErrorLength   = 0x00004000,
    ErrorLock     = 0x00008000,
    ErrorOperand  = 0x00010000,

    PrefixRepNz   = 0x01000000,
    PrefixRepX    = 0x02000000,
    Prefix66      = 0x04000000,
    Prefix67      = 0x08000000,
    PrefixLock    = 0x10000000,
    PrefixSeg     = 0x20000000,
    PrefixRex     = 0x40000000,
};

void *FindRelative(const void *addr_start, const void *addr_min, const void *addr_max, uint16_t opcode, size_t index, bool backward, ptrdiff_t offset)
{
	if (IsSafeModeActive())
	{
		if (!IsMemoryValid(addr_start) || !IsMemoryValid(addr_min) || !IsMemoryValid(addr_max))
		{
			SetError(ME_INVALID_MEMORY);
			return nullptr;
		}
	}

	void *result = const_cast<void *>(addr_start);
	hde64s hs;

	while (true)
	{
		if (!IsInBounds(result, addr_min, addr_max))
			return nullptr;

		memset(&hs, 0, sizeof(hde64s));
		hde64_disasm(result, &hs);

		if (hs.flags & F64_RELATIVE)
		{
			if (opcode != 0)
			{
				if (opcode > 255)
				{
					auto op1 = opcode & 0xFF;
					auto op2 = (opcode >> 8) & 0xFF;

					if (hs.opcode != op1 || hs.opcode2 != op2)
						continue;
				}
				else
				{
					if (hs.opcode != (opcode & 0xFF))
						continue;
				}
			}

			if (index == 0)
			{
				if (offset != 0 && !IsMemoryValid(result, offset))
				{
					SetError(ME_INVALID_MEMORY);
					return nullptr;
				}

				return PtrOffset(result, offset);
			}

			index--;
		}

		result = PtrOffset(result, hs.len);
	}

	return result;
}

MEMORIA_END