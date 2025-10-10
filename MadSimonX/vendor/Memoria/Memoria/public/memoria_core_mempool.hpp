#pragma once

#include "memoria_common.hpp"

MEMORIA_BEGIN

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern bool Free(void *addr);

extern bool FreeAll();

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *New(size_t size);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *Alloc(size_t size, bool is_executable = false, bool is_readable = true, bool is_writable = true);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *AllocNear(const void *addr_source, size_t size, bool is_executable = false, bool is_readable = true, bool is_writable = true);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
extern void *AllocFar(const void *addr_source, size_t size, bool is_executable = false, bool is_readable = true, bool is_writable = true);


MEMORIA_END