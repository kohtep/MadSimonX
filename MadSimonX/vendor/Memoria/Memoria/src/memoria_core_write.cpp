#include "memoria_core_write.hpp"

#include "memoria_core_options.hpp"
#include "memoria_core_misc.hpp"
#include "memoria_core_errors.hpp"

#include <Windows.h>
#include <string_view>

MEMORIA_BEGIN

bool WriteMemory(void *addr, const void *data, size_t size, ptrdiff_t offset, bool use_setmem)
{
	if (IsSafeModeActive() && !IsMemoryValid(addr, offset))
	{
		SetError(ME_INVALID_MEMORY);
		return false;
	}

	if (offset != 0)
		addr = PtrOffset(addr, offset);

	DWORD new_protection, old_protection;

	if (IsMemoryExecutable(addr, offset))
		new_protection = PAGE_EXECUTE_READWRITE;
	else
		new_protection = PAGE_READWRITE;

	if (!VirtualProtect(addr, size, new_protection, &old_protection))
	{
		SetError(ME_INVALID_PROTECTION_1);
		return false;
	}

	if (use_setmem)
	{
		memset(addr, static_cast<int>(*static_cast<const uint8_t *>(data)), size);
	}
	else
	{
		memcpy(addr, data, size);
	}

	if (!VirtualProtect(addr, size, old_protection, &old_protection))
	{
		SetError(ME_INVALID_PROTECTION_2);
		return false;
	}

	return true;
}

bool WriteU8(void *addr, uint8_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteU16(void *addr, uint16_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteU24(void *addr, const int8_t value[3], ptrdiff_t offset)
{
	return WriteMemory(addr, value, 3, offset);
}

bool WriteU24(void *addr, uint32_t value, ptrdiff_t offset)
{
	uint8_t bytes[3] =
	{
		static_cast<uint8_t>(value & 0xFF),
		static_cast<uint8_t>((value >> 8) & 0xFF),
		static_cast<uint8_t>((value >> 16) & 0xFF)
	};

	return WriteMemory(addr, &bytes, sizeof(bytes), offset);
}

bool WriteU32(void *addr, uint32_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteU64(void *addr, uint64_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteI8(void *addr, int8_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteI16(void *addr, int16_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteI24(void *addr, const int8_t value[3], ptrdiff_t offset)
{
	return WriteMemory(addr, value, 3, offset);
}

bool WriteI24(void *addr, int32_t value, ptrdiff_t offset)
{
	int8_t bytes[3] =
	{
		static_cast<int8_t>(value & 0xFF),
		static_cast<int8_t>((value >> 8) & 0xFF),
		static_cast<int8_t>((value >> 16) & 0xFF)
	};

	return WriteMemory(addr, &bytes, sizeof(bytes), offset);
}

bool WriteI32(void *addr, int32_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteI64(void *addr, int64_t value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteFloat(void *addr, float value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteDouble(void *addr, double value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WritePointer(void *addr, const void *value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
}

bool WriteRelative(void *addr, const void *value, ptrdiff_t offset)
{
	if (!IsIn32BitRange(addr, value))
		return false;

#pragma warning(suppress : 4244) // conversion from '__int64' to 'uint32_t', possible loss of data
	uint32_t rel = (uint8_t *)value - ((uint8_t *)addr + sizeof(int32_t));

	return WriteU32(addr, rel, offset);
}

bool WriteAStr(void *addr, const char *value, ptrdiff_t offset)
{
	if (!value) 
		return false;

	size_t len = strlen(value);
	return WriteMemory(addr, value, (len + 1) * sizeof(char), offset);
}

bool WriteWStr(void *addr, const wchar_t *value, ptrdiff_t offset)
{
	if (!value) 
		return false;

	size_t len = wcslen(value);
	return WriteMemory(addr, value, (len + 1) * sizeof(wchar_t), offset);
}

bool FillChar(void *addr, int value, size_t size)
{
	return WriteMemory(addr, &value, size, 0, true);
}

bool FillNops(void *addr, size_t size)
{
	return FillChar(addr, 0x90, size);
}

MEMORIA_END