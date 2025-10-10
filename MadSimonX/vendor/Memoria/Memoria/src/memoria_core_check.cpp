#include "memoria_core_check.hpp"

#include "memoria_core_misc.hpp"
#include "memoria_core_options.hpp"
#include "memoria_core_errors.hpp"

#include <string_view>

MEMORIA_BEGIN

bool CheckMemory(const void *addr, const void *value, size_t size, ptrdiff_t offset)
{
	if (IsSafeModeActive())
	{
		if (!IsMemoryValid(addr, offset))
		{
			SetError(ME_INVALID_MEMORY);
			return false;
		}

		if (!IsMemoryValid(value))
		{
			SetError(ME_INVALID_MEMORY);
			return false;
		}
	}

	return memcmp(PtrOffset(addr, offset), value, size) == 0;
}

bool CheckU8(const void *addr, uint8_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckU16(const void *addr, uint16_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckU24(const void *addr, uint32_t value, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return CheckMemory(addr, &bytes, 3, offset);
}

bool CheckU24(const void *addr, const uint8_t value[3], ptrdiff_t offset)
{
	return CheckMemory(addr, value, 3, offset);
}

bool CheckU32(const void *addr, uint32_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckU64(const void *addr, uint64_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckI8(const void *addr, int8_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckI16(const void *addr, int16_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckI24(const void *addr, int32_t value, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return CheckMemory(addr, &bytes, 3, offset);
}

bool CheckI24(const void *addr, const int8_t value[3], ptrdiff_t offset)
{
	return CheckMemory(addr, value, 3, offset);
}

bool CheckI32(const void *addr, int32_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckI64(const void *addr, int64_t value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckFloat(const void *addr, float value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckDouble(const void *addr, double value, ptrdiff_t offset)
{
	return CheckMemory(addr, &value, sizeof(value), offset);
}

bool CheckAStr(const void *addr, const char *value, ptrdiff_t offset)
{
	if (!value)
		return false;

	size_t len = strlen(value);
	return CheckMemory(addr, value, len, offset);
}

bool CheckWStr(const void *addr, const wchar_t *value, ptrdiff_t offset)
{
	if (!value)
		return false;

	size_t len = wcslen(value);
	return CheckMemory(addr, value, len * sizeof(wchar_t), offset * sizeof(wchar_t));
}

bool CheckSignature(const void *addr, const CSignature &value, ptrdiff_t offset)
{
	if (IsSafeModeActive())
	{
		if (value.IsEmpty())
		{
			SetError(ME_INVALID_ARGUMENT);
			return false;
		}

		if (!IsMemoryValid(addr, offset))
		{
			SetError(ME_INVALID_MEMORY);
			return false;
		}
	}

	if (offset != 0)
		addr = (reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(addr) + offset));

	return value.Match(addr);
}

bool CheckSignature(const void *addr, const char *value, ptrdiff_t offset)
{
	if (!value)
		return false;

	CSignature sig(value);
	return CheckSignature(addr, sig, offset);
}

MEMORIA_END