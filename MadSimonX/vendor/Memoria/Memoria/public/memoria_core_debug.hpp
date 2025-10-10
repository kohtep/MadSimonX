#pragma once

#include "memoria_common.hpp"
#include "memoria_utils_vector.hpp"

#include <Windows.h>

MEMORIA_BEGIN

/**
 * @brief Retrieves the function name for the specified address from the PDB file, if available.
 *
 * @param address Pointer to the function.
 * @param out Destination buffer for the symbol name.
 * @param max_size Maximum size of the output buffer.
 *
 * @return true if the symbol was found, false otherwise.
 */
extern bool GetSymbolName(const void *address, char *out, size_t max_size);

/**
 * @brief Formats the function name for the specified address using the PDB file, if available.
 *
 * @param address Pointer to the function.
 * @param out Destination buffer to receive the formatted name.
 * @param max_size Maximum size of the output buffer.
 * @param concat_module_name If `true`, the module name will be used as a prefix.
 * @param memory_beautify_on_error Use an alternative formatting method if debug information is unavailable.
 *
 * @note The function may accept a pointer to an arbitrary code location inside a function;
 *       in such cases, it will attempt to retrieve the function start address from the PDB file.
 *       If this fails and the `memory_beautify_on_error` parameter is set to `true`, formatting
 *       will be delegated to the `Memoria::BeautifyPointer` function.
 *
 * @warning In some cases, the function name cannot be retrieved from the PDB file even if it is present
 *          (in practice, this was observed only once — when attempting to retrieve the `main` name).
 *
 * @return true if formatting succeeded, false otherwise.
 */
extern bool GetBeautyFunctionAddress(const void *address, char *out, size_t max_size, bool concat_module_name = true, bool memory_beautify_on_error = true);

/**
 * @brief Retrieves a list of absolute function ranges for the specified module.
 *
 * Intended for x64 architecture — it will most likely not work on x86.
 *
 * @param handle Module handle.
 *
 * @warning The function extracts information from the `IMAGE_DIRECTORY_ENTRY_EXCEPTION` section,
 *       which is usually missing in x86 binaries, so the collection for such files will be empty.
 *
 * @return A tuple collection of function address ranges (start and end addresses).
 */
extern Memoria::Vector<std::tuple<void */*addr_start*/, void */*addr_end*/>> GetFunctionEntries(HMODULE handle);

/**
 * @brief Retrieves the base address of the specified function.
 *
 * @param address Pointer to an arbitrary code location within the function.
 *
 * @warning The function extracts information from the `IMAGE_DIRECTORY_ENTRY_EXCEPTION` section,
 *       which is usually missing in x86 binaries, so the collection for such files will be empty.
 *
 * @return The base address of the function.
 */
extern void *GetFunctionBaseAddressFromItsCode(const void *address);

/**
 * @brief Retrieves the call stack for the calling code.
 *
 * @return The call stack.
 */
extern __declspec(noinline) Memoria::FixedVector<void *, 128> GetStackBacktrace();

/**
 * @brief Retrieves the call stack in a beautified format for the calling code.
 * 
 * @code
 *   "Memoria.exe!Memoria::GetStackBacktrace+0xAD [Memoria.b687d]"
 *   "Memoria.exe!Memoria::GetBeautyStackBacktrace+0x98 [Memoria.75b48]"
 *   "Memoria.exe!main+0x55 [Memoria.7a935]"
 *   "Memoria.exe!invoke_main+0x39 [Memoria.b89c9]"
 *   "Memoria.exe!__scrt_common_main_seh+0x132 [Memoria.b88b2]"
 *   "Memoria.exe!__scrt_common_main+0xE [Memoria.b876e]"
 *   "Memoria.exe!mainCRTStartup+0xE [Memoria.b8a5e]"
 *   "KERNEL32.DLL!BaseThreadInitThunk+0x14 [KERNEL32.17374]"
 *   "ntdll.dll!RtlUserThreadStart+0x21 [ntdll.4cc91]"
 * @endcode
 * 
 * @return A vector of strings representing the call stack.
 */
//extern Memoria::Vector<std::string> GetBeautyStackBacktrace();

extern void *GetImageDirectoryData(void *base, bool image, uint16_t dir, uint32_t *size);

MEMORIA_END