#pragma once

#include "memoria_common.hpp"

MEMORIA_BEGIN

using OnErrorEvent_t = void(*)(int value);

extern void SetError(int value);
extern void ResetError();
extern int GetError();

extern void SetErrorCallback(OnErrorEvent_t value);
extern OnErrorEvent_t GetErrorCallback();

MEMORIA_END

// No errors.
#define ME_NO_ERROR             0

// Any error related to memory access, ranging from an invalid pointer 
// (nullptr, out of range) to an internal WinAPI error.
#define ME_INVALID_MEMORY       1

// Any error related to an invalid argument, such as an uninitialized 
// CSignature object or a size_t argument with an invalid value.
#define ME_INVALID_ARGUMENT     2

// Error specific to memory write functions. Indicates that the call to 
// VirtualProtect to enable write permissions failed.
#define ME_INVALID_PROTECTION_1 3

// Error specific to memory write functions. Indicates that the call to 
// VirtualProtect to restore access permissions failed.
#define ME_INVALID_PROTECTION_2 4

// Any error related to failure in finding certain data, e.g., 
// FindReference failed to find anything.
#define ME_NOT_FOUND            5