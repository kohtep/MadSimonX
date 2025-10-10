#pragma once

#include "memoria_common.hpp"

#include "memoria_utils_vector.hpp"

#include <stdint.h>
#include <Windows.h>
#include <utility>

MEMORIA_BEGIN

/**
 * @brief Retrieves a pointer to the section header corresponding to `image_directory` in the specified `handle` module.
 *
 * @param handle Handle to the EXE or DLL.
 * @param image_directory Directory index, e.g., `IMAGE_DIRECTORY_ENTRY_EXPORT`.
 *
 * @return Pointer to the `IMAGE_SECTION_HEADER` structure for the specified section, or nullptr if not found.
 */
extern PIMAGE_SECTION_HEADER GetSectionByIndex(HMODULE handle, DWORD image_directory);

/**
 * @brief Searches for a section in the specified `handle` module by section flags.
 *
 * @param handle Handle to the EXE or DLL.
 * @param flags Flags of the target section, e.g., `IMAGE_SCN_CNT_CODE`.
 * @param match_exactly If `true`, the `flags` value must match exactly. Otherwise,
 *                      a section containing the specified flags will be returned.
 *
 * @note Searching by flags with `match_exactly` set to `true` is considered the preferred method.
 *       The author of these lines has encountered MSVC compiler binaries containing internal Visual C++
 *       functions that search for sections in exactly this way.
 *
 * @return Pointer to the `IMAGE_SECTION_HEADER` structure for the matched section, or nullptr if not found.
 */
extern PIMAGE_SECTION_HEADER GetSectionByFlags(HMODULE handle, DWORD flags, bool match_exactly = true);

/**
 * @brief Searches for a section in the specified `handle` module by section name.
 *
 * @param handle Handle to the EXE or DLL.
 * @param name Name of the section, e.g., `.rdata`.
 *
 * @return Pointer to the `IMAGE_SECTION_HEADER` structure for the matched section, or nullptr if not found.
 */
extern PIMAGE_SECTION_HEADER GetSectionByName(HMODULE handle, const char *name);

/**
 * @brief Returns a pointer to the section that contains the entry point.
 *
 * @param handle Handle to the EXE or DLL.
 *
 * @return Pointer to the `IMAGE_SECTION_HEADER` structure for the matched section, or nullptr if not found.
 */
extern PIMAGE_SECTION_HEADER GetEntrySection(HMODULE handle);

/**
 * @brief Calculates the start and end addresses of the specified section.
 *
 * @param handle Handle to the EXE or DLL.
 * @param section Pointer to the section.
 *
 * @return A pair of pointers: the first points to the beginning of the section,
 *         the second points to the last byte of the section. Returns an empty pair if the handle is invalid.
 */
extern std::pair<PVOID, PVOID> GetSectionBounds(HMODULE handle, PIMAGE_SECTION_HEADER section);

/**
 * @brief Calculates the start and end addresses of the specified section within the current module.
 *
 * @param section Pointer to the section.
 *
 * @return A pair of pointers: the first points to the beginning of the section,
 *         the second points to the last byte of the section. Returns an empty pair if the module handle cannot be obtained.
 */
extern std::pair<PVOID, PVOID> GetSectionBounds(PIMAGE_SECTION_HEADER section);

extern HMODULE GetExeBase();

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *GetImageBase(HMODULE handle);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *GetImageBase(const char *name);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern std::pair<WORD, WORD> GetModuleVersion(HMODULE handle);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern std::pair<WORD, WORD> GetModuleVersion(const char *name);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern DWORD GetModuleSize(HMODULE handle);

struct ExportFunc_t
{
    void *Address;
    const char *Name;
    WORD Ordinal;
};

extern size_t ParseExportDirectory(HMODULE handle, ExportFunc_t *out, size_t max_size);

struct ImportFunc_t
{
	const char *DLLName;

	// nullptr if ordinal used
	const char *FuncName;

	WORD Ordinal;

	void *IATAddress;
};

extern size_t ParseImportDirectory(HMODULE handle, ImportFunc_t *out, size_t max_size);

extern DWORD GetMainThreadId();

MEMORIA_END