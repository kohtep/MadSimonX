#pragma once

#include "memoria_common.hpp"

// TODO: Export only GetVTableForClass?

MEMORIA_BEGIN

extern void *GetRTTIDescriptor(const void *addr_start, const void *addr_min, const void *addr_max, const char *rtti_name);
extern void **GetVTableForDescriptor(const void *addr_start, const void *addr_min, const void *addr_max, const void *rtti_descriptor);

extern void **GetVTableForClass(const void *addr_start, const void *addr_min, const void *addr_max, const char *rtti_name);

MEMORIA_END