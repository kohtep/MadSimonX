#pragma once

#include "memoria_common.hpp"

#include "memoria_core_signature.hpp"

#include <stdint.h>

MEMORIA_BEGIN

/**
 * @brief Compares a byte at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckU8(const void *addr, uint8_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares an unsigned word (2 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckU16(const void *addr, uint16_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares 3 bytes at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @note Although the function accepts a double word (4 bytes), only the lower
 *       24 bits (00FFFFFFh) will be checked.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckU24(const void *addr, uint32_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares 3 bytes at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @note The byte order in the array must conform to Little-endian.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckU24(const void *addr, const uint8_t value[3], ptrdiff_t offset = 0);

/**
 * @brief Compares an unsigned double word (4 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckU32(const void *addr, uint32_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares an unsigned quad word (8 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckU64(const void *addr, uint64_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares a character (1 signed byte) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckI8(const void *addr, int8_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares a signed word (2 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckI16(const void *addr, int16_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares 3 bytes at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @note Although the function accepts a double word (4 bytes), only the lower
 *       24 bits (00FFFFFFh) will be checked.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckI24(const void *addr, int32_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares 3 signed bytes at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @note The byte order in the array must conform to Little-endian.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckI24(const void *addr, const int8_t value[3], ptrdiff_t offset = 0);

/**
 * @brief Compares a signed double word (4 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckI32(const void *addr, int32_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares a signed quad word (8 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckI64(const void *addr, int64_t value, ptrdiff_t offset = 0);

/**
 * @brief Compares a single-precision floating-point number (4 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckFloat(const void *addr, float value, ptrdiff_t offset = 0);

/**
 * @brief Compares a double-precision floating-point number (8 bytes) at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value Value to compare against.
 * @param offset Preliminary offset relative to 'addr'.
 *
 * @return true if the value at the specified address is identical to 'value', false otherwise.
 */
extern bool CheckDouble(const void *addr, double value, ptrdiff_t offset = 0);

/**
 * @brief Compares an ANSI string at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value The string value to compare with.
 * @param offset Preliminary offset relative to 'addr' before comparison.
 *
 * @return true if the memory content at the given address matches 'value', false otherwise.
 */
extern bool CheckAStr(const void *addr, const char *value, ptrdiff_t offset = 0);

/**
 * @brief Compares a wide-character (UTF-16/UTF-32) string at the specified memory address.
 *
 * @param addr Memory address to check.
 * @param value The wide string value to compare with.
 * @param offset Optional offset relative to 'addr' before comparison (in wchar_t).
 *
 * @return true if the memory content at the given address matches 'value', false otherwise.
 */
extern bool CheckWStr(const void *addr, const wchar_t *value, ptrdiff_t offset = 0);

extern bool CheckSignature(const void *addr, const CSignature &value, ptrdiff_t offset = 0);
extern bool CheckSignature(const void *addr, const char *value, ptrdiff_t offset = 0);

MEMORIA_END