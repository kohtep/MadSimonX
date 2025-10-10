#pragma once

// TODO: Rename? memoria_core_misc -> memoria_core_utils

#include "memoria_common.hpp"
#include "memoria_core_hash.hpp"

#include <stdint.h>
#include <Windows.h>
#include <functional>

MEMORIA_BEGIN

typedef struct _PEB_LDR_DATA
{
	UINT8 _PADDING_[12];
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
#ifdef _WIN64
	UINT8 _PADDING_[24];
#else
	UINT8 _PADDING_[12];
#endif
	PEB_LDR_DATA *Ldr;
} PEB, *PPEB;

struct UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
};

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


/**
 * @brief Performs an offset relative to `addr` forward or backward by the value of `offset`.
 *
 * @param addr The starting address.
 * @param offset The value for the offset; can be positive (forward offset) or
 *               negative (backward offset).
 * @param dereference If `true`, the function will dereference the specified address
 *                    (after applying the offset) and return the result.
 *
 * @return The new address value after the offset or the dereferenced result if `dereference` is true.
 */
static __forceinline void *PtrOffset(const void *addr, ptrdiff_t offset, bool dereference = false)
{
	void *result = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(addr) + offset);

	if (dereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

/**
 * @brief Converts the relative offset, at which it is located,
 *        to an absolute address relative to `addr`.
 *
 * @param addr The address from which the offset will be read.
 * @param offset The offset relative to `addr` that will be applied before reading.
 *
 * @return The absolute address relative to `addr` where the offset is located.
 */
static __forceinline void *RelToAbs(const void *addr, ptrdiff_t offset = 0)
{
	return PtrOffset(addr, *static_cast<const int32_t *>(addr) + offset);
}

/**
 * @brief Converts the relative offset, at which it is located,
 *        to an absolute address relative to `addr`.
 *
 * @param addr The address from which the offset will be read.
 * @param pre_offset The offset relative to `addr` that will be applied before reading.
 * @param post_offset The offset relative to `addr` that will be applied after reading.
 *
 * @return The absolute address relative to `addr` where the offset is located.
 */
static __forceinline void *RelToAbsEx(const void *addr, ptrdiff_t pre_offset, ptrdiff_t post_offset)
{
	return RelToAbs(reinterpret_cast<void *>(ptrdiff_t(addr) + pre_offset), post_offset);
}

/**
 * @brief Forms an offset relative to `base` for the address `addr`.
 *
 * @param base The reference point.
 * @param addr The address for which the offset will be calculated.
 * @param imm_size An additional offset to the result that will be added after
 *                 the calculation. By default, it is 4 bytes. This is needed because
 *                 some assembly instructions require, for example, an offset of 5 instead of 4.
 *
 * @see CWriteBuffer::WriteRelative
 * @return The offset relative to `base` for the address `addr`, or `0` if offset cannot be calculated.
 */
static __forceinline int32_t CalcRel(const void *base, void *addr, size_t imm_size = sizeof(int32_t))
{
	intptr_t offset = reinterpret_cast<intptr_t>(PtrOffset(base, -reinterpret_cast<intptr_t>(addr) - imm_size));
	return static_cast<int32_t>(offset);
}

/**
 * @brief Performs an offset relative to `addr` forward by the value of `offset`.
 *
 * @param addr The starting address.
 * @param offset The value for the forward offset.
 * @param dereference If `true`, the function will dereference the specified address
 *                    (after applying the offset) and return the result.
 *
 * @return The new address value after the offset or the dereferenced result if `dereference` is true.
 */
static __forceinline void *PtrAdvance(const void *addr, size_t offset, bool dereference = false)
{
	void *result = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(addr) + offset);

	if (dereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

/**
 * @brief Performs an offset relative to `addr` backward by the value of `offset`.
 *
 * @param addr The starting address.
 * @param offset The value for the backward offset.
 * @param dereference If `true`, the function will dereference the specified address
 *                    (after applying the offset) and return the result.
 *
 * @return The new address value after the offset or the dereferenced result if `dereference` is true.
 */
static __forceinline void *PtrRewind(const void *addr, size_t offset, bool dereference = false)
{
	void *result = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(addr) - offset);

	if (dereference)
		result = *reinterpret_cast<void **>(result);

	return result;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
static __forceinline bool IsInBounds(uintptr_t addr, uintptr_t addr_lower, uintptr_t addr_upper)
{
	if (addr < addr_lower)
		return false;

	if (addr >= addr_upper)
		return false;

	return true;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
static __forceinline bool IsInBounds(const void *addr, const void *addr_lower, const void *addr_upper)
{
	auto naddr = reinterpret_cast<uintptr_t>(addr);
	auto lower = reinterpret_cast<uintptr_t>(addr_lower);
	auto upper = reinterpret_cast<uintptr_t>(addr_upper);

	return IsInBounds(naddr, lower, upper);
}

#pragma push_macro("max")
#undef max

/**
 * @brief
 *
 * @param
 *
 * @return
 */
static __forceinline bool IsIn32BitRange(uintptr_t addr1, uintptr_t addr2, ptrdiff_t offset = 0)
{
	uintptr_t adjusted_addr2 = static_cast<uintptr_t>(static_cast<ptrdiff_t>(addr2) + offset);
	uintptr_t diff = (addr1 > adjusted_addr2) ? (addr1 - adjusted_addr2) : (adjusted_addr2 - addr1);

	return diff <= std::numeric_limits<uint32_t>::max();
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
static __forceinline bool IsIn32BitRange(const void *addr1, const void *addr2, ptrdiff_t offset = 0)
{
	uintptr_t ptr1 = reinterpret_cast<uintptr_t>(addr1);
	uintptr_t ptr2 = reinterpret_cast<uintptr_t>(addr2);
	uintptr_t adjusted_ptr2 = static_cast<uintptr_t>(static_cast<ptrdiff_t>(ptr2) + offset);
	uintptr_t diff = (ptr1 > adjusted_ptr2) ? (ptr1 - adjusted_ptr2) : (adjusted_ptr2 - ptr1);

	return diff <= std::numeric_limits<uint32_t>::max();
}

#pragma pop_macro("max")

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool IsMemoryValid(const void *addr, ptrdiff_t offset = 0);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool IsMemoryExecutable(const void *addr, ptrdiff_t offset = 0);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool MakeWritable(void *addr);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool MakeReadable(void *addr);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool MakeExecutable(void *addr);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool RemoveWritable(void *addr);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool RemoveReadable(void *addr);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool RemoveExecutable(void *addr);

/**
 * @brief Returns the base address of the pointer `addr`.
 *
 * @param addr Pointer to an arbitrary memory location.
 *
 * @return Base address.
 */
extern void *GetBaseAddress(const void *addr);

/**
 * @brief Retrieves the address (ImageBase) of the module (DLL/EXE) that called the function.
 *
 * @return The ImageBase of the caller.
 */
extern __declspec(noinline) void *GetSelfAddress();

extern __declspec(noinline) HMODULE GetSelfHandle();

/**
 * @brief
 *
 * @param
 *
 * @return
 */
bool GetModuleName(HMODULE hModule, char *out, size_t max_size);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
bool GetModuleNameForAddress(const void *address, char *out, size_t max_size);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
bool BeautifyPointer(const void *addr, char *out, size_t max_size);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern size_t Align(size_t value, int alignment = 16);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *Align(const void *value, int alignment = 16);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern DWORD BeginThread(void (*fnFunction)(LPVOID), LPVOID param);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern HMODULE GetModuleHandleDirect(fnv1a_t module_name_hash);

extern void *GetProcAddressDirect(fnv1a_t function_name_hash);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *GetProcAddressDirect(fnv1a_t module_name_hash, fnv1a_t function_name_hash);

/**
 * @brief Retrieves a pointer to the `module_name` interface from the library specified by `handle`.
 *        Used for Valve Software-style modules that expose a `CreateInterface` function.
 *
 * @param handle Handle to the module.
 * @param module_name Name of the interface, e.g., `IFileSystemV009`.
 *
 * @return Pointer to the interface.
 */
extern void *GetInterfaceAddress(HMODULE handle, const char *module_name);

/**
 * @brief Retrieves a pointer to the `interface_name` interface from the library specified by `module_name`.
 *        Used for Valve Software-style modules that expose a `CreateInterface` function.
 *
 * @param module_name Name of the module.
 * @param interface_name Name of the interface, e.g., `IFileSystemV009`.
 *
 * @return Pointer to the interface.
 */
extern void *GetInterfaceAddress(const char *module_name, const char *nterface_name);

MEMORIA_END