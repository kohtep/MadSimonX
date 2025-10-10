#pragma once

#include "memoria_common.hpp"
#include "memoria_utils_vector.hpp"

#include <stdint.h>
#include <memory>

MEMORIA_BEGIN

//
// Macros to serve as a reminder that the project should be rewritten for C99 compliance in the future.
//

#define MAX_PATCHES_COUNT 128
#define MAX_PATCH_SIZE 128

//
// Holder of patch information.
//
// The absence of a destructor here is intentional. 
// Please do not forget to call `Memoria::Cleanup` or `Memoria::FreePatches`.
//
class CPatch
{
private:
	CPatch(const CPatch &) = delete;
	CPatch &operator=(const CPatch &) = delete;

private:
	void *_dest_address = nullptr;
	bool _active = false;

	Memoria::FixedVector<uint8_t, MAX_PATCH_SIZE> _data_origin{};
	Memoria::FixedVector<uint8_t, MAX_PATCH_SIZE> _data_patch{};

#ifdef _DEBUG
	// Introduces significant overhead to the object size. 
	// It is better to avoid using it in release configurations 
	// to maintain a smaller binary size.

	Memoria::FixedVector<void *, 128> _stack_backtrace{};
#endif

public:
	bool IsActive() const;
	bool IsValid() const;

	void Apply();
	void Restore();
	void Toggle(bool state);

	void *GetAddress() const { return _dest_address; }

	const auto &GetDataOrigin() const { return _data_origin; }
	const auto &GetDataPatch() const { return _data_patch; }

	CPatch() = default;
	CPatch(void *dest_address, const void *source_address, size_t size, bool instant_deploy);
};

extern CPatch *PatchU8(void *addr, uint8_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchU16(void *addr, uint16_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchU24(void *addr, uint32_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchU24(void *addr, uint8_t value[3], bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchU32(void *addr, uint32_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchU64(void *addr, uint64_t value, bool instant_deploy = true, ptrdiff_t offset = 0);

extern CPatch *PatchI8(void *addr, int8_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchI16(void *addr, int16_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchI24(void *addr, int32_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchI24(void *addr, int8_t value[3], bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchI32(void *addr, int32_t value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchI64(void *addr, int64_t value, bool instant_deploy = true, ptrdiff_t offset = 0);

extern CPatch *PatchFloat(void *addr, float value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchDouble(void *addr, double value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchPointer(void *addr, const void *value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchRelative(void *addr, const void *value, bool instant_deploy = true, ptrdiff_t offset = 0);

extern CPatch *PatchAStr(void *addr, const char *value, bool instant_deploy = true, ptrdiff_t offset = 0);
extern CPatch *PatchWStr(void *addr, const wchar_t *value, bool instant_deploy = true, ptrdiff_t offset = 0);

extern Memoria::FixedVector<CPatch, MAX_PATCHES_COUNT> &GetPatches();
extern bool FreePatches();

MEMORIA_END