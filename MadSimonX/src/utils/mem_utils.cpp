#include "mem_utils.hpp"

#include <assert.h>
#include <Windows.h>
#include <cstring>

#include "../vendor/hde/public/hde32.h"

using FindMemoryCmp_t = bool(*)(const void *addr1, const void *addr2, size_t size, void *param);

#define MEM_NAMESPACE_BEGIN namespace U::Memory {
#define MEM_NAMESPACE_END }

MEM_NAMESPACE_BEGIN

bool IsInBounds(uintptr_t addr, uintptr_t addr_lower, uintptr_t addr_upper)
{
	if (addr < addr_lower)
		return false;

	if (addr >= addr_upper)
		return false;

	return true;
}

bool IsInBounds(const void *addr, const void *addr_lower, const void *addr_upper)
{
	auto naddr = reinterpret_cast<uintptr_t>(addr);
	auto lower = reinterpret_cast<uintptr_t>(addr_lower);
	auto upper = reinterpret_cast<uintptr_t>(addr_upper);

	return IsInBounds(naddr, lower, upper);
}

static bool FindMemoryCmp(const void *addr1, const void *addr2, size_t size, void *param)
{
	return memcmp(addr1, addr2, size) == 0;
}

static bool IsMemoryExecutable(const void *addr, ptrdiff_t offset)
{
	if (offset != 0)
		addr = (void *)(uintptr_t(addr) + offset);

	if (!addr)
		return false;

	MEMORY_BASIC_INFORMATION memInfo;

	if (VirtualQuery(addr, &memInfo, sizeof(memInfo)) == 0)
		return false;

	if (memInfo.Protect == 0 || memInfo.Protect == PAGE_NOACCESS)
		return false;

	return memInfo.Protect == PAGE_EXECUTE ||
		memInfo.Protect == PAGE_EXECUTE_READ ||
		memInfo.Protect == PAGE_EXECUTE_READWRITE ||
		memInfo.Protect == PAGE_EXECUTE_WRITECOPY;
}

bool WriteMemory(void *addr, const void *data, size_t size, ptrdiff_t offset, bool use_setmem)
{
	if (offset != 0)
		addr = PtrOffset(addr, offset);

	DWORD new_protection, old_protection;

	if (IsMemoryExecutable(addr, offset))
		new_protection = PAGE_EXECUTE_READWRITE;
	else
		new_protection = PAGE_READWRITE;

	if (!VirtualProtect(addr, size, new_protection, &old_protection))
		return false;

	if (use_setmem)
	{
		memset(addr, static_cast<int>(*static_cast<const uint8_t *>(data)), size);
	}
	else
	{
		memcpy(addr, data, size);
	}

	if (!VirtualProtect(addr, size, old_protection, &old_protection))
		return false;

	return true;
}

static bool CheckMemory(const void *addr, const void *value, size_t size, ptrdiff_t offset)
{
	if (!addr)
		return false;

	return memcmp(PtrOffset(addr, offset), value, size) == 0;
}

void *PtrOffset(const void *addr, ptrdiff_t offset, bool dereference)
{
	void *result = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(addr) + offset);

	if (dereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

void *FindMemory(const void *addr_start, const void *addr_min, const void *addr_max, const void *data, size_t size, bool backward,
	ptrdiff_t offset = 0, FindMemoryCmp_t comparator = FindMemoryCmp, void *comparator_param = nullptr)
{
	assert(addr_min != nullptr && addr_max != nullptr && addr_min <= addr_max);

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

bool WritePointer(void *addr, const void *value, ptrdiff_t offset)
{
	return WriteMemory(addr, &value, sizeof(value), offset);
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

void *Advance(void *pAddr, ptrdiff_t nOffset, bool bDereference)
{
	if (!pAddr)
		return nullptr;

	void *result = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(pAddr) + nOffset);

	if (bDereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

void *Resolve(void *pAddr, ptrdiff_t nPreOffset, ptrdiff_t nPostOffset)
{
	if (!pAddr)
		return nullptr;

	return Resolve(reinterpret_cast<void *>(ptrdiff_t(pAddr) + nPreOffset), nPostOffset);
}

void *Resolve(void *pAddr, ptrdiff_t nOffset)
{
	if (!pAddr)
		return nullptr;

	return Advance(pAddr, *static_cast<int *>(pAddr) + nOffset);
}

static size_t CalculateInstructionSize32(const void *addr, ptrdiff_t offset)
{
	hde32s hs;
	return hde32_disasm(reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(addr) + offset), &hs);
}

static size_t CalculateInstructionBoundary32(const void *addr, size_t min_size)
{
	size_t size = 0;

	while (true)
	{
		size += CalculateInstructionSize32(addr, size);

		if (size >= min_size)
			return size;
	}
}

#pragma pack(push, 1)
struct TrampolineMeta
{
	uint32_t hook_size;
	void *original_addr;
};
#pragma pack(pop)

bool Splice(void *splice_addr, void *jump_to, void *out_tramp, bool place_call)
{
	if (!splice_addr || !jump_to)
		return false;

	size_t patch_size = CalculateInstructionBoundary32(splice_addr, 5);
	if (patch_size < 5)
		return false;

	uint8_t *src = static_cast<uint8_t *>(splice_addr);
	uint8_t *dst = static_cast<uint8_t *>(jump_to);

	size_t meta_size = sizeof(TrampolineMeta);
	size_t tramp_code_size = patch_size + 5;
	size_t total_size = meta_size + tramp_code_size + patch_size;

	uint8_t *base = static_cast<uint8_t *>(
		VirtualAlloc(nullptr, total_size,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE));

	if (!base)
		return false;

	TrampolineMeta *meta = reinterpret_cast<TrampolineMeta *>(base);
	uint8_t *tramp = base + meta_size;
	uint8_t *orig_copy = tramp + tramp_code_size;

	meta->hook_size = static_cast<uint32_t>(patch_size);
	meta->original_addr = splice_addr;

	bool src_is_jmp = (src[0] == 0xE9);

	if (src_is_jmp)
	{
		int32_t orig_rel = 0;
		std::memcpy(&orig_rel, src + 1, sizeof(orig_rel));
		uint8_t *orig_target = src + 5 + orig_rel;

		tramp[0] = 0xE9;
		int32_t tramp_rel = static_cast<int32_t>(orig_target - (tramp + 5));
		std::memcpy(tramp + 1, &tramp_rel, sizeof(tramp_rel));

		if (patch_size > 5)
			std::memset(tramp + 5, 0x90, patch_size - 5);
	}
	else
	{
		std::memcpy(tramp, src, patch_size);

		uint8_t *tramp_jmp = tramp + patch_size;
		tramp_jmp[0] = 0xE9;
		int32_t rel_back = static_cast<int32_t>((src + patch_size) - (tramp_jmp + 5));
		std::memcpy(tramp_jmp + 1, &rel_back, sizeof(rel_back));
	}

	std::memcpy(orig_copy, src, patch_size);

	DWORD old_protect = 0;
	if (!VirtualProtect(src, patch_size, PAGE_EXECUTE_READWRITE, &old_protect))
	{
		VirtualFree(base, 0, MEM_RELEASE);
		return false;
	}

	std::memset(src, 0x90, patch_size);

	src[0] = place_call ? 0xE8 : 0xE9;
	int32_t rel_to = static_cast<int32_t>(dst - (src + 5));
	std::memcpy(src + 1, &rel_to, sizeof(rel_to));

	FlushInstructionCache(GetCurrentProcess(), src, patch_size);

	DWORD tmp = 0;
	VirtualProtect(src, patch_size, old_protect, &tmp);

	if (out_tramp)
		*reinterpret_cast<void **>(out_tramp) = static_cast<void *>(tramp);

	return true;
}

bool SpliceAPI(const char *modname, const char *funcname, void *jump_to, void *out_tramp, bool place_call)
{
	HMODULE h = GetModuleHandleA(modname);
	if (!h)
		return false;

	FARPROC f = GetProcAddress(h, "GetCursorPos");
	if (!f)
		return false;

	return U::Memory::Splice(f, jump_to, out_tramp, place_call);
}

bool Unsplice(void *tramp)
{
	if (!tramp)
		return false;

	uint8_t *tramp_code = static_cast<uint8_t *>(tramp);
	TrampolineMeta *meta = reinterpret_cast<TrampolineMeta *>(tramp_code - sizeof(TrampolineMeta));

	uint32_t hook_size = meta->hook_size;
	if (hook_size == 0)
		return false;

	uint8_t *orig = static_cast<uint8_t *>(meta->original_addr);
	if (!orig)
		return false;

	size_t patch_size = hook_size;
	size_t tramp_code_size = patch_size + 5;
	uint8_t *orig_copy = tramp_code + tramp_code_size;

	DWORD old_protect = 0;
	if (!VirtualProtect(orig, patch_size, PAGE_EXECUTE_READWRITE, &old_protect))
		return false;

	std::memcpy(orig, orig_copy, patch_size);

	FlushInstructionCache(GetCurrentProcess(), orig, patch_size);

	DWORD tmp = 0;
	VirtualProtect(orig, patch_size, old_protect, &tmp);

	uint8_t *base = reinterpret_cast<uint8_t *>(meta);
	VirtualFree(base, 0, MEM_RELEASE);

	return true;
}

bool FillChar(void *addr, int value, size_t size)
{
	return WriteMemory(addr, &value, size, 0, true);
}

bool FillNops(void *addr, size_t size)
{
	return FillChar(addr, 0x90, size);
}

bool IsDllLoaded(const char *name)
{
	return (GetModuleHandleA(name) != nullptr);
}

class MemoryBackup
{
private:
	void *_address;
	size_t _size;
	unsigned char *_data;

public:
	MemoryBackup(const void *address, size_t size);
	~MemoryBackup();
};

static const int MaxBackups = 64;
static MemoryBackup *g_backups[MaxBackups] = {};
static int g_backupCount = 0;

MemoryBackup::MemoryBackup(const void *address, size_t size)
	: _address(const_cast<void *>(address))
	, _size(static_cast<size_t>(size))
	, _data(0)
{
	if (_address != 0 && _size > 0)
	{
		_data = new unsigned char[_size];
		std::memcpy(_data, _address, _size);
	}
}

MemoryBackup::~MemoryBackup()
{
	if (_address == 0 || _data == 0 || _size == 0)
	{
		if (_data != 0)
			delete[] _data;

		return;
	}

	DWORD oldProtect;

	if (VirtualProtect(_address, _size, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		std::memcpy(_address, _data, _size);
		DWORD dummy;
		VirtualProtect(_address, _size, oldProtect, &dummy);
	}

	delete[] _data;
}

bool CreateMemoryBackup(const void *address, size_t size)
{
	if (g_backupCount >= MaxBackups)
	{
		return false;
	}

	MemoryBackup *backup = new MemoryBackup(address, size);
	g_backups[g_backupCount] = backup;
	++g_backupCount;

	return true;
}

bool RestoreMemoryBackups()
{
	if (g_backupCount <= 0)
	{
		return false;
	}

	for (int i = g_backupCount - 1; i >= 0; --i)
	{
		if (g_backups[i] != nullptr)
		{
			delete g_backups[i];
			g_backups[i] = nullptr;
		}
	}

	g_backupCount = 0;
	return true;
}

MEM_NAMESPACE_END