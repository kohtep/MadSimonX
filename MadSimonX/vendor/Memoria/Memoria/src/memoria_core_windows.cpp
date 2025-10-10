#include "memoria_core_windows.hpp"

#include "memoria_core_misc.hpp"

#include <VersionHelpers.h>
#include <string_view>
#include <TlHelp32.h>

#include "memoria_utils_secure.hpp"

#ifdef MEMORIA_USE_LAZYIMPORT
	#define GetModuleHandleA    LI_FN(GetModuleHandleA)
#endif

MEMORIA_BEGIN

// GetModuleHandleA

using SectionCallbackFn = bool(*)(PIMAGE_SECTION_HEADER section, LPVOID param);

void EnumSections(HMODULE handle, SectionCallbackFn cb, LPVOID param)
{
	if (!cb)
		return;

	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)handle;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dosHeader + dosHeader->e_lfanew);

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

	for (unsigned int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		if (!cb(section, param))
			break;

		section++;
	}
}

PIMAGE_SECTION_HEADER GetSectionByIndex(HMODULE handle, DWORD image_directory)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return nullptr;

	if (image_directory >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES)
		return nullptr;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)handle;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dosHeader + dosHeader->e_lfanew);

	auto dataDir = ntHeaders->OptionalHeader.DataDirectory[image_directory];

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

	DWORD va = dataDir.VirtualAddress;

	for (unsigned int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		DWORD start = section->VirtualAddress;
		DWORD end = start + section->Misc.VirtualSize;

		if (va >= start && va < end)
			return section;

		section++;
	}

	return nullptr;
}

PIMAGE_SECTION_HEADER GetSectionByFlags(HMODULE handle, DWORD flags, bool match_exactly)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return nullptr;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)handle;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dosHeader + dosHeader->e_lfanew);

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

	for (unsigned int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		if (match_exactly)
		{
			if (section->Characteristics == flags)
			{
				return section;
			}
		}
		else
		{
			if ((section->Characteristics & flags) != 0)
			{
				return section;
			}
		}

		section++;
	}

	return nullptr;
}

PIMAGE_SECTION_HEADER GetSectionByName(HMODULE handle, const char *name)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle || !name || !*name)
		return nullptr;

	PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
	PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(
		reinterpret_cast<DWORD_PTR>(dosHeader) + dosHeader->e_lfanew);

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

	size_t name_len = strlen(name);

	for (unsigned int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		if (memcmp(section->Name, name, name_len) == 0)
		{
			return section;
		}

		section++;
	}

	return nullptr;
}

PIMAGE_SECTION_HEADER GetEntrySection(HMODULE handle)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return nullptr;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)handle;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dosHeader + dosHeader->e_lfanew);

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
	LPVOID pEntryPoint = PtrOffset(handle, ntHeaders->OptionalHeader.AddressOfEntryPoint);

	for (unsigned int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		auto &&[lo, hi] = GetSectionBounds(section);

		if (IsInBounds(pEntryPoint, lo, hi))
			return section;

		section++;
	}

	return nullptr;
}

std::pair<PVOID, PVOID> GetSectionBounds(HMODULE handle, PIMAGE_SECTION_HEADER section)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return {};

	auto sectionStart = (PVOID)((DWORD_PTR)handle + section->VirtualAddress);
	auto sectionEnd = (PVOID)((DWORD_PTR)sectionStart + section->Misc.VirtualSize - 1);

	return std::make_pair(sectionStart, sectionEnd);
}

std::pair<PVOID, PVOID> GetSectionBounds(PIMAGE_SECTION_HEADER section)
{
	HMODULE handle;

	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)section, &handle))
		return {};

	if (!handle)
		return {};

	return GetSectionBounds(handle, section);
}

HMODULE GetExeBase()
{
#ifdef _M_X64
	PPEB peb = (PPEB)__readgsqword(0x60);
#elif defined(_M_IX86)
	PPEB peb = (PPEB)__readfsdword(0x30);
#else
#error Unsupported architecture
#endif

	if (!peb || !peb->Ldr)
		return nullptr;

	PLIST_ENTRY head = peb->Ldr->InLoadOrderModuleList.Flink;
	if (!head)
		return nullptr;

	PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(head, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	return reinterpret_cast<HMODULE>(entry->DllBase);
}

void *GetImageBase(HMODULE handle)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return nullptr;

	IMAGE_DOS_HEADER *pDOSHeader = (IMAGE_DOS_HEADER *)handle;

	if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	IMAGE_NT_HEADERS *pNTHeaders = (IMAGE_NT_HEADERS *)((BYTE *)pDOSHeader + pDOSHeader->e_lfanew);

	if (pNTHeaders->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	return (void *)pNTHeaders->OptionalHeader.ImageBase;
}

void *GetImageBase(const char *name)
{
	HMODULE handle;

	if (!name || !*name)
		handle = GetExeBase();
	else
		handle = GetModuleHandleA(name);

	if (!handle)
		return nullptr;

	return GetImageBase(handle);
}

std::pair<WORD, WORD> GetModuleVersion(HMODULE handle)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle)
		return { 0, 0 };

	IMAGE_DOS_HEADER *pDOSHeader = (IMAGE_DOS_HEADER *)handle;
	if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return { 0, 0 };

	IMAGE_NT_HEADERS *pNTHeaders = (IMAGE_NT_HEADERS *)((BYTE *)pDOSHeader + pDOSHeader->e_lfanew);
	if (pNTHeaders->Signature != IMAGE_NT_SIGNATURE)
		return { 0, 0 };

	return
	{
		pNTHeaders->OptionalHeader.MajorOperatingSystemVersion,
		pNTHeaders->OptionalHeader.MinorOperatingSystemVersion
	};
}

std::pair<WORD, WORD> GetModuleVersion(const char *name)
{
	HMODULE handle;

	if (!name || !*name)
		handle = GetExeBase();
	else
		handle = GetModuleHandleA(name);

	if (!handle)
		return { 0, 0 };

	return GetModuleVersion(handle);
}

DWORD GetModuleSize(HMODULE handle)
{
	if (!handle)
		handle = GetExeBase();

	PIMAGE_DOS_HEADER dos = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
	PIMAGE_NT_HEADERS nt = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(dos) + dos->e_lfanew);

	return nt->OptionalHeader.SizeOfImage;
}

size_t ParseExportDirectory(HMODULE handle, ExportFunc_t *out, size_t max_size)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle || !out || max_size == 0)
		return 0;

	size_t count = 0;
	BYTE *base = reinterpret_cast<BYTE *>(handle);
	IMAGE_DOS_HEADER *dosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(base);

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	IMAGE_NT_HEADERS *ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(base + dosHeader->e_lfanew);

	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	DWORD exportDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (!exportDirRVA)
		return 0;

	IMAGE_EXPORT_DIRECTORY *exportDir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY *>(base + exportDirRVA);
	DWORD *functions = reinterpret_cast<DWORD *>(base + exportDir->AddressOfFunctions);
	DWORD *names = reinterpret_cast<DWORD *>(base + exportDir->AddressOfNames);
	WORD *ordinals = reinterpret_cast<WORD *>(base + exportDir->AddressOfNameOrdinals);

	for (DWORD i = 0; i < exportDir->NumberOfFunctions && count < max_size; ++i)
	{
		ExportFunc_t &func = out[count];
		func.Address = reinterpret_cast<void *>(base + functions[i]);
		func.Name = nullptr;

		for (DWORD j = 0; j < exportDir->NumberOfNames; ++j)
		{
			if (ordinals[j] == i)
			{
				func.Name = reinterpret_cast<const char *>(base + names[j]);
				break;
			}
		}

		func.Ordinal = static_cast<WORD>(exportDir->Base + i);
		++count;
	}

	return count;
}

size_t ParseImportDirectory(HMODULE handle, ImportFunc_t *out, size_t max_size)
{
	if (!handle)
		handle = GetExeBase();

	if (!handle || !out || max_size == 0)
		return 0;

	size_t count = 0;
	BYTE *base = reinterpret_cast<BYTE *>(handle);
	IMAGE_DOS_HEADER *dosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(base);

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	IMAGE_NT_HEADERS *ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(base + dosHeader->e_lfanew);

	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	DWORD importDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (!importDirRVA)
		return 0;

	IMAGE_IMPORT_DESCRIPTOR *importDesc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR *>(base + importDirRVA);

	while (importDesc->Name && count < max_size)
	{
		const char *dllName = reinterpret_cast<const char *>(base + importDesc->Name);
		IMAGE_THUNK_DATA *origThunk = reinterpret_cast<IMAGE_THUNK_DATA *>(base + importDesc->OriginalFirstThunk);
		IMAGE_THUNK_DATA *iatThunk = reinterpret_cast<IMAGE_THUNK_DATA *>(base + importDesc->FirstThunk);

		if (!origThunk)
			origThunk = iatThunk;

		while (origThunk->u1.AddressOfData && count < max_size)
		{
			ImportFunc_t &func = out[count];
			func.DLLName = dllName;
			func.IATAddress = reinterpret_cast<void *>(&iatThunk->u1.Function);

			if (origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				func.Ordinal = static_cast<WORD>(origThunk->u1.Ordinal & 0xFFFF);
				func.FuncName = nullptr;
			}
			else
			{
				IMAGE_IMPORT_BY_NAME *importByName = reinterpret_cast<IMAGE_IMPORT_BY_NAME *>(base + origThunk->u1.AddressOfData);
				func.FuncName = reinterpret_cast<const char *>(importByName->Name);
				func.Ordinal = 0;
			}

			++origThunk;
			++iatThunk;
			++count;
		}

		++importDesc;
	}

	return count;
}

DWORD GetMainThreadId()
{
	auto hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return 0;

	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);
		return 0;
	}

	DWORD result = 0;
	int cur_tid = GetCurrentProcessId();

	do
	{
		if (te32.th32OwnerProcessID == cur_tid)
		{
			result = te32.th32ThreadID;
			break;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);

	return result;
}

MEMORIA_END