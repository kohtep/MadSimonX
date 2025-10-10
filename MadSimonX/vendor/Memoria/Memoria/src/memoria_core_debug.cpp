#include "memoria_core_debug.hpp"

#include "memoria_core_misc.hpp"
#include "memoria_utils_format.hpp"
#include "memoria_utils_optional.hpp"

#include <Windows.h>
#include <dbghelp.h>
#include <optional>

MEMORIA_BEGIN

static Memoria::Optional<bool> SymInited;

using SymCleanupFunc_t = BOOL(WINAPI *)(HANDLE);
using SymInitializeFunc_t = BOOL(WINAPI *)(HANDLE, PCSTR, BOOL);
using SymFromAddrFunc_t = BOOL(WINAPI *)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);

static Memoria::Optional<SymCleanupFunc_t> SymCleanupFn;
static Memoria::Optional<SymInitializeFunc_t> SymInitializeFn;
static Memoria::Optional<SymFromAddrFunc_t> SymFromAddrFn;

static HMODULE GetDbgHelpModule()
{
	HMODULE hDbgHelp;

	if (hDbgHelp = GetModuleHandleW(L"dbghelp.dll"))
		hDbgHelp = LoadLibraryW(L"dbghelp.dll");

	return hDbgHelp;
}

static BOOL SymCleanupWrap(HANDLE hProcess)
{
	if (!SymCleanupFn.has_value())
	{
		if (auto hDbgHelp = GetDbgHelpModule())
			SymCleanupFn = reinterpret_cast<SymCleanupFunc_t>(GetProcAddress(hDbgHelp, "SymCleanup"));
		else
			SymCleanupFn = nullptr;
	}

	return SymCleanupFn.value() ? SymCleanupFn.value()(hProcess) : FALSE;
}

static BOOL SymInitializeWrap(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess)
{
	if (!SymInitializeFn.has_value())
	{
		if (auto hDbgHelp = GetDbgHelpModule())
			SymInitializeFn = reinterpret_cast<SymInitializeFunc_t>(GetProcAddress(hDbgHelp, "SymInitialize"));
		else
			SymInitializeFn = nullptr;
	}

	return SymInitializeFn.value() ? SymInitializeFn.value()(hProcess, UserSearchPath, fInvadeProcess) : FALSE;
}

static BOOL SymFromAddrWrap(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol)
{
	if (!SymFromAddrFn.has_value())
	{
		if (auto hDbgHelp = GetDbgHelpModule())
			SymFromAddrFn = reinterpret_cast<SymFromAddrFunc_t>(GetProcAddress(hDbgHelp, "SymFromAddr"));
		else
			SymFromAddrFn = nullptr;
	}

	return SymFromAddrFn.value() ? SymFromAddrFn.value()(hProcess, Address, Displacement, Symbol) : FALSE;
}

template <typename T>
static void *ImageDirectoryEntryToDataT(void *base,
	bool image,
	uint16_t dir,
	uint32_t *size)
{
	auto dos = reinterpret_cast<IMAGE_DOS_HEADER *>(base);
	if (dos->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	auto nt = reinterpret_cast<T *>(
		reinterpret_cast<uint8_t *>(base) + dos->e_lfanew);
	if (nt->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	if (size)
		*size = 0;

	if (dir >= nt->OptionalHeader.NumberOfRvaAndSizes)
		return nullptr;

	uint32_t addr = nt->OptionalHeader.DataDirectory[dir].VirtualAddress;
	if (!addr)
		return nullptr;

	if (size)
		*size = nt->OptionalHeader.DataDirectory[dir].Size;

	if (image || addr < nt->OptionalHeader.SizeOfHeaders)
		return reinterpret_cast<uint8_t *>(base) + addr;

	auto sec = IMAGE_FIRST_SECTION(reinterpret_cast<IMAGE_NT_HEADERS *>(nt));
	for (uint16_t i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec)
	{
		auto va = sec->VirtualAddress;
		auto sz = sec->SizeOfRawData > 0 ? sec->SizeOfRawData : sec->Misc.VirtualSize;

		if (addr >= va && addr < va + sz)
		{
			auto delta = addr - va;
			return reinterpret_cast<uint8_t *>(base) + sec->PointerToRawData + delta;
		}
	}

	return nullptr;
}

void *GetImageDirectoryData(void *base,
	bool image,
	uint16_t dir,
	uint32_t *size)
{
	auto dos = reinterpret_cast<IMAGE_DOS_HEADER *>(base);
	if (dos->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	auto ntBase = reinterpret_cast<IMAGE_NT_HEADERS *>(
		reinterpret_cast<uint8_t *>(base) + dos->e_lfanew);
	if (ntBase->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	if (ntBase->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		return ImageDirectoryEntryToDataT<IMAGE_NT_HEADERS64>(base, image, dir, size);

	if (ntBase->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return ImageDirectoryEntryToDataT<IMAGE_NT_HEADERS32>(base, image, dir, size);

	return nullptr;
}

static void SymShutdown()
{
	if (SymInited.has_value() && SymInited.value())
	{
		SymCleanupWrap(GetCurrentProcess());
		SymInited = std::nullopt;
	}
}

static bool SymInit()
{
	if (SymInited.has_value())
		return SymInited.value();

	SymInited = SymInitializeWrap(GetCurrentProcess(), NULL, TRUE);

	if (SymInited.value())
		RegisterOnExitCallback(SymShutdown);

	return SymInited.value();
}

// The variable is moved outside the function to eliminate the generation of __chkstk calls.
static char GetSymbolNameSymBuf[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];

bool GetSymbolName(const void *address, char *out, size_t max_size)
{
	if (!address || !out || max_size == 0)
		return false;

	if (!SymInit())
		return false;

	DWORD64 displacement = 0;
	PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(GetSymbolNameSymBuf);
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;

	if (!SymFromAddrWrap(GetCurrentProcess(), reinterpret_cast<DWORD64>(address), &displacement, symbol))
		return false;

	if (symbol->Name)
	{
		strncpy_s(out, max_size, symbol->Name, _TRUNCATE);
		return true;
	}

	return false;
}

bool GetBeautyFunctionAddress(const void *address, char *out, size_t max_size,
	bool concat_module_name, bool memory_beautify_on_error)
{
	if (!address || !out || max_size == 0)
		return false;

	void *pBaseAddress = GetFunctionBaseAddressFromItsCode(address);
	if (!pBaseAddress)
	{
		if (memory_beautify_on_error)
			return BeautifyPointer(address, out, max_size);
		out[0] = '\0';
		return false;
	}

	char name[1024];
	if (!GetSymbolName(pBaseAddress, name, sizeof(name)))
	{
		if (memory_beautify_on_error)
			return BeautifyPointer(address, out, max_size);
		out[0] = '\0';
		return false;
	}

	uintptr_t offset = reinterpret_cast<uintptr_t>(address) - reinterpret_cast<uintptr_t>(pBaseAddress);

	char offset_str[32];
	FormatBufSafe(offset_str, sizeof(offset_str), "+0x%llX", static_cast<unsigned long long>(offset));

	char module_prefix[512] = "";
	if (concat_module_name)
	{
		HMODULE hModule = reinterpret_cast<HMODULE>(GetBaseAddress(address));
		if (hModule)
		{
			GetModuleName(hModule, module_prefix, sizeof(module_prefix));
			strcat_s(module_prefix, sizeof(module_prefix), "!");
		}
	}

	char pointer_str[128];
	BeautifyPointer(address, pointer_str, sizeof(pointer_str));

	int written = FormatBufSafe(out, max_size, "%s%s%s [%s]", module_prefix, name, offset_str, pointer_str);
	return written > 0 && static_cast<size_t>(written) < max_size;
}

Memoria::Vector<std::tuple<void *, void *>> GetFunctionEntries(HMODULE handle)
{
	if (!handle)
		handle = GetModuleHandleA(0);

	if (!handle)
		return {};

	uint32_t size;
	PIMAGE_RUNTIME_FUNCTION_ENTRY pFunc = reinterpret_cast<PIMAGE_RUNTIME_FUNCTION_ENTRY>(
		GetImageDirectoryData(handle, TRUE, IMAGE_DIRECTORY_ENTRY_EXCEPTION, &size));

	if (!pFunc)
		return {};

	DWORD count = size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);

	Memoria::Vector<std::tuple<void *, void *>> result{};
	result.reserve(count);

	for (size_t i = 0u; i < count; ++i)
	{
		if (!pFunc[i].BeginAddress || !pFunc[i].EndAddress)
			continue;

		void *addr_begin = PtrOffset(handle, pFunc[i].BeginAddress);
		void *addr_end = PtrOffset(handle, pFunc[i].EndAddress);

		result.emplace_back(addr_begin, addr_end);
	}

	return result;
}

void *GetFunctionBaseAddressFromItsCode(const void *address)
{
	HMODULE hModule = static_cast<HMODULE>(GetBaseAddress(address));
	if (!hModule)
		return nullptr;

	uint32_t size;
	PIMAGE_RUNTIME_FUNCTION_ENTRY pFunc = reinterpret_cast<PIMAGE_RUNTIME_FUNCTION_ENTRY>(
		GetImageDirectoryData(hModule, TRUE, IMAGE_DIRECTORY_ENTRY_EXCEPTION, &size));

	if (!pFunc)
		return nullptr;

	DWORD count = size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);

	for (size_t i = 0u; i < count; ++i)
	{
		if (!pFunc[i].BeginAddress || !pFunc[i].EndAddress)
			continue;

		uintptr_t addr_begin = reinterpret_cast<uintptr_t>(hModule) + pFunc[i].BeginAddress;
		uintptr_t addr_end = reinterpret_cast<uintptr_t>(hModule) + pFunc[i].EndAddress;

		uintptr_t addr_needed = reinterpret_cast<uintptr_t>(address);

		if (IsInBounds(addr_needed, addr_begin, addr_end))
			return reinterpret_cast<void *>(addr_begin);
	}

	return nullptr;
}

Memoria::FixedVector<void *, 128> GetStackBacktrace()
{
	Memoria::FixedVector<void *, 128> result{};

	void *callers[128];
	int count = RtlCaptureStackBackTrace(2, _countof(callers), callers, NULL);
	
	for (int i = 0; i < count; i++)
		result.push_back(callers[i]);

	return result;
}

//Memoria::Vector<std::string> GetBeautyStackBacktrace()
//{
//	Memoria::Vector<std::string> result{};
//
//	auto backtrace = GetStackBacktrace();
//	if (backtrace.empty())
//		return result;
//
//	result.reserve(backtrace.size());
//
//	for (auto &&addr : backtrace)
//	{
//		auto sName = GetBeautyFunctionAddress(addr);
//		result.emplace_back(sName);
//	}
//
//	return result;
//}

MEMORIA_END