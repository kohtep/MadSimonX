#include "memoria_core_misc.hpp"

#include "memoria_utils_format.hpp"

#include "memoria_core_windows.hpp"

#include <Windows.h>
#include <inttypes.h>

#include "memoria_utils_secure.hpp"

#undef max

#ifdef MEMORIA_USE_LAZYIMPORT
	#define CloseHandle      LI_FN_EX("kernel32.dll", CloseHandle)
	#define CreateThread     LI_FN_EX("kernel32.dll", CreateThread)
	#define VirtualQuery     LI_FN_EX("kernel32.dll", VirtualQuery)
	#define LoadLibraryA     LI_FN_EX("kernel32.dll", LoadLibraryA)
#endif

MEMORIA_BEGIN

bool IsMemoryValid(const void *addr, ptrdiff_t offset)
{
	if (offset != 0)
		addr = (void *)(uintptr_t(addr) + offset);

	if (!addr)
		return false;

	MEMORY_BASIC_INFORMATION memInfo;

	if (VirtualQuery(addr, &memInfo, sizeof(memInfo)) == 0)
		return false;

	return !(memInfo.Protect == 0 || memInfo.Protect == PAGE_NOACCESS);
}

bool IsMemoryExecutable(const void *addr, ptrdiff_t offset)
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

bool MakeWritable(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return false;

	DWORD protect = mbi.Protect;
	DWORD newProtect = protect;

	switch (protect)
	{
	case PAGE_EXECUTE:
		newProtect = PAGE_EXECUTE_READWRITE;
		break;
	case PAGE_EXECUTE_READ:
		newProtect = PAGE_EXECUTE_READWRITE;
		break;
	case PAGE_READONLY:
		newProtect = PAGE_READWRITE;
		break;
	case PAGE_NOACCESS:
		newProtect = PAGE_READWRITE;
		break;
	}

	if (newProtect == protect)
		return false;

	DWORD oldProtect;
	return VirtualProtect(addr, mbi.RegionSize, newProtect, &oldProtect) != 0;
}

bool MakeReadable(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return false;

	DWORD protect = mbi.Protect;
	DWORD newProtect = protect;

	switch (protect)
	{
	case PAGE_EXECUTE:
		newProtect = PAGE_EXECUTE_READ;
		break;
	case PAGE_NOACCESS:
		newProtect = PAGE_READONLY;
		break;
	}

	if (newProtect == protect)
		return false;

	DWORD oldProtect;
	return VirtualProtect(addr, mbi.RegionSize, newProtect, &oldProtect) != 0;
}

bool MakeExecutable(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return false;

	DWORD protect = mbi.Protect;
	DWORD newProtect = protect;

	switch (protect)
	{
	case PAGE_READONLY:
		newProtect = PAGE_EXECUTE_READ;
		break;
	case PAGE_READWRITE:
		newProtect = PAGE_EXECUTE_READWRITE;
		break;
	case PAGE_NOACCESS:
		newProtect = PAGE_EXECUTE;
		break;
	}

	if (newProtect == protect)
		return false;

	DWORD oldProtect;
	return VirtualProtect(addr, mbi.RegionSize, newProtect, &oldProtect) != 0;
}

bool RemoveWritable(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return false;

	DWORD protect = mbi.Protect;
	DWORD newProtect = protect;

	switch (protect) {
	case PAGE_READWRITE:
		newProtect = PAGE_READONLY;
		break;
	case PAGE_EXECUTE_READWRITE:
		newProtect = PAGE_EXECUTE_READ;
		break;
	}

	if (newProtect == protect)
		return false;

	DWORD oldProtect;
	return VirtualProtect(addr, mbi.RegionSize, newProtect, &oldProtect) != 0;
}

bool RemoveReadable(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return false;

	DWORD protect = mbi.Protect;
	DWORD newProtect = protect;

	switch (protect)
	{
	case PAGE_READONLY:
		newProtect = PAGE_NOACCESS;
		break;
	case PAGE_READWRITE:
		newProtect = PAGE_NOACCESS;
		break;
	case PAGE_EXECUTE_READ:
		newProtect = PAGE_EXECUTE;
		break;
	case PAGE_EXECUTE_READWRITE:
		newProtect = PAGE_EXECUTE;
		break;
	}

	if (newProtect == protect)
		return false;

	DWORD oldProtect;
	return VirtualProtect(addr, mbi.RegionSize, newProtect, &oldProtect) != 0;
}

bool RemoveExecutable(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return false;

	DWORD protect = mbi.Protect;
	DWORD newProtect = protect;

	switch (protect)
	{
	case PAGE_EXECUTE:
		newProtect = PAGE_NOACCESS;
		break;
	case PAGE_EXECUTE_READ:
		newProtect = PAGE_READONLY;
		break;
	case PAGE_EXECUTE_READWRITE:
		newProtect = PAGE_READWRITE;
		break;
	}

	if (newProtect == protect)
		return false;

	DWORD oldProtect;
	return VirtualProtect(addr, mbi.RegionSize, newProtect, &oldProtect) != 0;
}

void *GetBaseAddress(const void *addr)
{
	HMODULE result;

	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(addr), &result))
	{
		return nullptr;
	}

	return result;
}

bool GetModuleName(HMODULE hModule, char *out, size_t max_size)
{
	if (!out || max_size == 0)
		return false;

	char buffer[MAX_PATH];
	if (GetModuleFileNameA(hModule, buffer, MAX_PATH) == 0)
	{
		out[0] = '\0';
		return false;
	}

	const char *filename = strrchr(buffer, '\\');
	filename = filename ? filename + 1 : buffer;

	strncpy(out, filename, max_size);
	return true;
}

bool GetModuleNameForAddress(const void *address, char *out, size_t max_size)
{
	if (!out || max_size == 0)
		return false;

	HMODULE base = (HMODULE)GetBaseAddress(address);
	if (!base)
	{
		out[0] = '\0';
		return false;
	}

	return GetModuleName(base, out, max_size);
}

bool BeautifyPointer(const void *addr, char *out, size_t max_size)
{
	if (!out || max_size == 0)
		return false;

	if (!addr)
	{
		strncpy(out, "null", max_size);
		return true;
	}

	void *base = GetBaseAddress(addr);
	if (!base)
	{
		FormatBufSafe(out, max_size, "%p", addr);
		return true;
	}

	char modname[64];
	if (!GetModuleName((HMODULE)base, modname, sizeof(modname)))
	{
		FormatBufSafe(out, max_size, "%p", addr);
		return true;
	}

	char *last_dot = strrchr(modname, '.');
	if (last_dot)
		*last_dot = '\0';

	uintptr_t offset = (uintptr_t)addr - (uintptr_t)base;
	FormatBuf(out, "%s.%llx", modname, (unsigned long long)offset);
	return true;
}

size_t Align(size_t value, int alignment)
{
	//Assert(alignment != 0);

	if (alignment == 0)
		return value;

	size_t mask = alignment - 1;

	if (value > SIZE_MAX - mask)
		return 0;

	return (value + mask) & ~mask;
}

void *Align(const void *value, int alignment)
{
	if (alignment == 0)
		return const_cast<void *>(value);

	uintptr_t addr = reinterpret_cast<uintptr_t>(value);
	uintptr_t mask = static_cast<uintptr_t>(alignment - 1);

	if (addr > SIZE_MAX - mask)
		return nullptr;

	addr = (addr + mask) & ~mask;
	return reinterpret_cast<void *>(addr);
}

void *GetSelfAddress()
{
	return GetBaseAddress(_ReturnAddress());
}

HMODULE GetSelfHandle()
{
	return reinterpret_cast<HMODULE>(GetSelfAddress());
}

DWORD BeginThread(void (*fnFunction)(LPVOID), LPVOID param)
{
	DWORD nThreadId;

	HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fnFunction), param, 0, &nThreadId);
	if (hThread == NULL)
		return 0;

	CloseHandle(hThread);
	return nThreadId;	
}

DWORD BeginThread(void (*fnFunction)())
{
	DWORD nThreadId;

	HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fnFunction), nullptr, 0, &nThreadId);
	if (hThread == NULL)
		return 0;

	CloseHandle(hThread);
	return nThreadId;
}

using EnumModulesFn_t = bool(*)(PLDR_DATA_TABLE_ENTRY entry, LPVOID param);

static void EnumModules(EnumModulesFn_t fn, LPVOID param)
{
#ifdef _WIN64
	PPEB pPeb = (PPEB)__readgsqword(0x60);
#elif _WIN32
	PPEB pPeb = (PPEB)__readfsdword(0x30);
#else
	Assert(false);
	return 0;
#endif

	PLIST_ENTRY pListHead = &(pPeb->Ldr->InMemoryOrderModuleList);
	PLIST_ENTRY pListEntry = pListHead->Flink;

	while (pListEntry != pListHead)
	{
		PLDR_DATA_TABLE_ENTRY pModule = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		if (!fn(pModule, param))
			break;

		pListEntry = pListEntry->Flink;
	}
}

using EnumExports_t = bool(*)(PLDR_DATA_TABLE_ENTRY entry, const char *name, void *address, WORD ordinal, LPVOID param);

static bool IsForwardExport(void *function_ptr, PIMAGE_EXPORT_DIRECTORY exportDir, DWORD exportSize)
{
	BYTE *ptr = (BYTE *)function_ptr;
	BYTE *start = (BYTE *)exportDir;
	BYTE *end = start + exportSize;

	return ptr >= start && ptr < end;
}

static void *ResolveForwardExport(const char *forward_str, bool force_load_library)
{
	char dll_name_raw[256];
	char func_name[256];

	strncpy_s(dll_name_raw, forward_str, sizeof(dll_name_raw));
	dll_name_raw[sizeof(dll_name_raw) - 1] = '\0';

	char *dot = strchr(dll_name_raw, '.');
	if (!dot)
		return nullptr;

	*dot = '\0';
	const char *dll_name_part = dll_name_raw;
	const char *func_name_part = dot + 1;

	strncpy_s(func_name, func_name_part, sizeof(func_name));
	func_name[sizeof(func_name) - 1] = '\0';

	char dll_name_full[MAX_PATH];
	strncpy_s(dll_name_full, dll_name_part, sizeof(dll_name_full) - 5);
	dll_name_full[sizeof(dll_name_full) - 5] = '\0';

	size_t len = strlen(dll_name_full);
	volatile char suffix[] = { '.', 'd', 'l', 'l' };

	if (len < 4 || strncmp(dll_name_full + len - 4, const_cast<const char *>(suffix), 4) != 0)
		strncat_s(dll_name_full, const_cast<const char *>(suffix), 4);

	HMODULE module = GetModuleHandleDirect(FNV1a64(dll_name_full));
	if (!module)
	{
		// TODO: force_load_library
		return nullptr;
	}

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)module;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((PBYTE)module + dosHeader->e_lfanew);
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)module + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	DWORD exportSize = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	if (!exportDir || exportSize == 0)
		return nullptr;

	PDWORD functions = (PDWORD)((PBYTE)module + exportDir->AddressOfFunctions);
	PDWORD names = (PDWORD)((PBYTE)module + exportDir->AddressOfNames);
	PWORD ordinals = (PWORD)((PBYTE)module + exportDir->AddressOfNameOrdinals);

	for (DWORD i = 0; i < exportDir->NumberOfNames; i++)
	{
		const char *name = (const char *)((PBYTE)module + names[i]);

		if (strcmp(name, func_name) == 0)
		{
			if (ordinals[i] >= exportDir->NumberOfFunctions)
				return nullptr;

			return (void *)((PBYTE)module + functions[ordinals[i]]);
		}
	}

	return nullptr;
}

static void EnumExports(HMODULE hModule, EnumExports_t fn, LPVOID param)
{
	struct Args_t
	{
		HMODULE Target;
		EnumExports_t Processor;
		LPVOID Param;
	} args;

	args.Target = hModule;
	args.Processor = fn;
	args.Param = param;

	EnumModules(+[](PLDR_DATA_TABLE_ENTRY entry, LPVOID param) -> bool
		{
			Args_t *args = reinterpret_cast<Args_t *>(param);

			if (args->Target && entry->DllBase != args->Target)
				return true;

			HMODULE hModule = (HMODULE)entry->DllBase;

			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
			if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
				return true;

			PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((PBYTE)hModule + dosHeader->e_lfanew);
			if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
				return true;

			PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)hModule + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
			DWORD exportSize = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

			if (!exportDir || exportSize == 0)
				return true;

			PDWORD functions = (PDWORD)((PBYTE)hModule + exportDir->AddressOfFunctions);
			PDWORD names = (PDWORD)((PBYTE)hModule + exportDir->AddressOfNames);
			PWORD ordinals = (PWORD)((PBYTE)hModule + exportDir->AddressOfNameOrdinals);

			for (DWORD i = 0; i < exportDir->NumberOfNames; i++)
			{
				if (ordinals[i] >= exportDir->NumberOfFunctions)
					continue;

				const char *function_name = (const char *)((PBYTE)hModule + names[i]);
				void *function_ptr = (void *)((PBYTE)hModule + functions[ordinals[i]]);

				if (IsForwardExport(function_ptr, exportDir, exportSize))
				{
					function_ptr = ResolveForwardExport(static_cast<const char *>(function_ptr), true);

					if (!function_ptr)
						continue;
				}

				if (!args->Processor(entry, function_name, function_ptr, ordinals[i], args->Param))
					return false;
			}

			return true;
		}, &args);
}

HMODULE GetModuleHandleDirect(fnv1a_t module_name_hash)
{
	struct Args_t
	{
		HMODULE Result;
		uint64_t Hash;
	} args;

	args.Result = nullptr;
	args.Hash = module_name_hash;

	EnumModules(+[](PLDR_DATA_TABLE_ENTRY entry, LPVOID param) -> bool
		{
			Args_t *args = reinterpret_cast<Args_t *>(param);

			if (FNV1a64(entry->BaseDllName.Buffer) == args->Hash)
			{
				args->Result = reinterpret_cast<HMODULE>(entry->DllBase);
				return false;
			}

			return true;
		}, &args);

	return args.Result;
}

void *GetProcAddressDirect(fnv1a_t module_name_hash, fnv1a_t function_name_hash)
{
	struct Args_t
	{
		uint64_t ModuleHash;
		uint64_t SymbolHash;
		void *Result;
	} args;

	args.ModuleHash = module_name_hash;
	args.SymbolHash = function_name_hash;
	args.Result = nullptr;

	EnumExports(NULL, +[](PLDR_DATA_TABLE_ENTRY entry, const char *name, void *address, WORD ordinal, LPVOID param) -> bool
		{
			Args_t *args = reinterpret_cast<Args_t *>(param);

			if (args->ModuleHash != 0 && FNV1a64(entry->BaseDllName.Buffer) != args->ModuleHash)
				return true;

			if (FNV1a64(name) == args->SymbolHash)
			{
				args->Result = address;
				return false;
			}

			return true;
		}, &args);

	return args.Result;
}

void *GetProcAddressDirect(fnv1a_t function_name_hash)
{
	return GetProcAddressDirect(0, function_name_hash);
}

using CreateInterfaceFn_t = void *(*)(const char *name, int *returnCode);

void *GetInterfaceAddress(HMODULE handle, const char *interface_name)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle || !interface_name || !*interface_name)
		return nullptr;

	volatile char createInterfaceName[17]{};

	// Crea -> aerC
	// teIn -> nIet
	// terf -> fret
	// ace0 -> eca
	*(uint32_t *)&createInterfaceName[0] = 'aerC';
	*(uint32_t *)&createInterfaceName[4] = 'nIet';
	*(uint32_t *)&createInterfaceName[8] = 'fret';
	*(uint32_t *)&createInterfaceName[12] = 'eca';

	auto pfnGetInterface = reinterpret_cast<CreateInterfaceFn_t>(
		GetProcAddress(handle, const_cast<LPCSTR>(createInterfaceName))
		);

	if (!pfnGetInterface)
		return nullptr;

	return pfnGetInterface(interface_name, nullptr);
}

void *GetInterfaceAddress(const char *module_name, const char *interface_name)
{
	if (!module_name || !*module_name || !interface_name || !*interface_name)
		return nullptr;

	HMODULE handle = GetModuleHandleA(module_name);
	if (!handle)
		return nullptr;

	return GetInterfaceAddress(handle, interface_name);
}

MEMORIA_END