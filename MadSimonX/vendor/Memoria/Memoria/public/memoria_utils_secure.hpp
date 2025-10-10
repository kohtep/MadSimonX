#pragma once

#include "memoria_common.hpp"
#include "memoria_core_hash.hpp"
#include "memoria_utils_assert.hpp"
#include "memoria_core_misc.hpp"

#include <stdint.h>
#include <limits>

MEMORIA_BEGIN

#pragma push_macro("max")
#undef max

class CRandomSimple
{
	static inline constexpr uint64_t _multiplier = 6364136223846793005ull;
	static inline constexpr uint64_t _increment = 1442695040888963407ull;

private:
	uint64_t _state = 0;

	template <typename T>
	__forceinline T Get() noexcept;

	template <typename T>
	__forceinline T Get(T min, T max) noexcept;

public:
	static constexpr uint64_t MakeSeedFrom(uint64_t basis, size_t lcg_iterations = 3) noexcept;
	static constexpr uint64_t MakeSeed(size_t lcg_iterations = 3) noexcept;

	constexpr CRandomSimple() noexcept = default;

	constexpr CRandomSimple(uint64_t seed) noexcept
		: _state(seed)
	{
	}

	constexpr void Skip() noexcept
	{
		if (_state == 0)
			_state = MakeSeed();

		_state = _state * _multiplier + _increment;
	}

	uint8_t  GetU8() noexcept { return Get<uint8_t>(0, UINT8_MAX); }
	uint16_t GetU16() noexcept { return Get<uint16_t>(0, UINT16_MAX); }
	uint32_t GetU24() noexcept { return Get<uint32_t>(0, 0xFFFFFF); }
	uint32_t GetU32() noexcept { return Get<uint32_t>(0, UINT32_MAX); }
	uint64_t GetU64() noexcept { return Get<uint64_t>(0, UINT64_MAX); }

	int8_t  GetI8() noexcept { return Get<int8_t>(INT8_MIN, INT8_MAX); }
	int16_t GetI16() noexcept { return Get<int16_t>(INT16_MIN, INT16_MAX); }
	int32_t GetI24() noexcept { return static_cast<int32_t>(Get<uint32_t>(0, 0xFFFFFF)); }
	int32_t GetI32() noexcept { return Get<int32_t>(INT32_MIN, INT32_MAX); }
	int64_t GetI64() noexcept { return Get<int64_t>(INT64_MIN, INT64_MAX); }

	uint8_t  GetU8(uint8_t min, uint8_t max) noexcept { return Get<uint8_t>(min, max); }
	uint16_t GetU16(uint16_t min, uint16_t max) noexcept { return Get<uint16_t>(min, max); }
	uint32_t GetU24(uint32_t min, uint32_t max) noexcept { return Get<uint32_t>(min, max) & 0xFFFFFF; }
	uint32_t GetU32(uint32_t min, uint32_t max) noexcept { return Get<uint32_t>(min, max); }
	uint64_t GetU64(uint64_t min, uint64_t max) noexcept { return Get<uint64_t>(min, max); }

	int8_t  GetI8(int8_t min, int8_t max) noexcept { return Get<int8_t>(min, max); }
	int16_t GetI16(int16_t min, int16_t max) noexcept { return Get<int16_t>(min, max); }
	int32_t GetI24(int32_t min, int32_t max) noexcept { return static_cast<int32_t>(Get<uint32_t>(min, max) & 0xFFFFFF); }
	int32_t GetI32(int32_t min, int32_t max) noexcept { return Get<int32_t>(min, max); }
	int64_t GetI64(int64_t min, int64_t max) noexcept { return Get<int64_t>(min, max); }

	uintptr_t Get() noexcept
	{
		if constexpr (sizeof(uintptr_t) == 8)
			return static_cast<uintptr_t>(GetU64());
		else
			return static_cast<uintptr_t>(GetU32());
	}

	float  GetFloat() noexcept { return GetFloat(0.0f, 1.0f); }
	double GetDouble() noexcept { return GetDouble(0.0, 1.0); }

	inline float  GetFloat(float min, float max) noexcept;
	inline double GetDouble(double min, double max) noexcept;

	bool CoinFlip() noexcept { return GetU8(0, 1) != 0; }
	bool OneIn(uint32_t n) noexcept { return GetU32(0, n - 1) == 0; }
	bool Chance(uint8_t percent) noexcept { return percent != 0 && GetU8(1, 100) <= percent; }
};

constexpr uint64_t CRandomSimple::MakeSeedFrom(uint64_t basis, size_t lcg_iterations) noexcept
{
	while (lcg_iterations--)
		basis = basis * _multiplier + _increment;

	return basis;
}

constexpr uint64_t CRandomSimple::MakeSeed(size_t lcg_iterations) noexcept
{
	const uint64_t h = static_cast<uint64_t>((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0'));
	const uint64_t m = static_cast<uint64_t>((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0'));
	const uint64_t s = static_cast<uint64_t>((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0'));

	const uint64_t base_seed = (h << 48) | (m << 40) | (s << 32) | (h * 3600 + m * 60 + s);

	return MakeSeedFrom(base_seed, lcg_iterations);
}

template <typename T>
T CRandomSimple::Get() noexcept
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	Skip();

	constexpr size_t state_bits = std::numeric_limits<decltype(_state)>::digits;
	constexpr size_t result_bits = std::numeric_limits<T>::digits;

	uint64_t value = _state >> (state_bits - result_bits);
	return static_cast<T>(value);
}

template <typename T>
T CRandomSimple::Get(T min, T max) noexcept
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	if (min > max)
	{
		AssertMsg(false, "CRandomSimple::Get: min is greater than max");
		std::swap(min, max);
	}

	Skip();

	using U = std::make_unsigned_t<T>;
	U umin = static_cast<U>(min);
	U umax = static_cast<U>(max);

	U value;
	if (umax == std::numeric_limits<U>::max() && umin == 0)
	{
		value = static_cast<U>(_state);
	}
	else
	{
		uint64_t range = static_cast<uint64_t>(umax) - static_cast<uint64_t>(umin) + 1;
		AssertMsg(range != 0, "CRandomSimple::Get: range is zero");
		value = static_cast<U>(umin + (_state % range));
	}

	return static_cast<T>(value);
}

float CRandomSimple::GetFloat(float min, float max) noexcept
{
	Skip();

	constexpr float FLOAT_DENOMINATOR = 18446744073709551616.0f; // 2^64 as float

	const float normalized = static_cast<float>(_state) / FLOAT_DENOMINATOR;
	return min + normalized * (max - min);
}

double CRandomSimple::GetDouble(double min, double max) noexcept
{
	Skip();

	constexpr double DOUBLE_DENOMINATOR = 18446744073709551616.0; // 2^64 as double

	const double normalized = static_cast<double>(_state) / DOUBLE_DENOMINATOR;
	return min + normalized * (max - min);
}

MEMORIA_END

MEMORIA_BEGIN

#pragma pack(push, 1)

//
// A small class for hiding `const char *` data.
//
// Use the `STRING` macro for convenient expansion of the encoded string.
//
template <size_t Size>
class CSecureString
{
public:
	static constexpr size_t Len = Size - 1;

private:
	//
	// I don't like the `mutable` solution, but it allows `Unpack` to be called in `c_str`,
	// which is a const method. At the moment, I don't really want to complicate the architecture
	// just to get rid of this modifier. Maybe someday in the future.
	//

	mutable char _data[Len];
	mutable uint8_t _seed;

private:
	static constexpr char rol(char c, uint8_t shift) noexcept
	{
		return static_cast<char>((static_cast<uint8_t>(c) << shift) |
			(static_cast<uint8_t>(c) >> (8 - shift)));
	}

	static constexpr char ror(char c, uint8_t shift) noexcept
	{
		return static_cast<char>((static_cast<uint8_t>(c) >> shift) |
			(static_cast<uint8_t>(c) << (8 - shift)));
	}

	static constexpr char mix(char c, size_t i) noexcept
	{
		return static_cast<char>((static_cast<uint8_t>(c) ^ 0x5A) + (i * 0x33));
	}

	static constexpr char unmix(char c, size_t i) noexcept
	{
		return static_cast<char>(((static_cast<uint8_t>(c) - (i * 0x33)) ^ 0x5A));
	}

	constexpr void Pack(const char *in) noexcept
	{
		for (size_t i = 0; i != Len; ++i)
			_data[i] = in[i];

		for (size_t i = 0; i != Len; ++i)
			_data[i] = rol(mix(_data[i], i), 3);

		_seed = static_cast<uint8_t>(CRandomSimple::MakeSeed());
		uint8_t key = _seed;

		for (size_t i = 0; i != Len; ++i)
		{
			_data[i] ^= key;
			key = static_cast<uint8_t>(CRandomSimple::MakeSeedFrom(key));
		}
	}

	__forceinline void Unpack() const noexcept
	{
		if (_seed == 0)
			return;

		uint8_t key = _seed;

		for (size_t i = 0; i != Len; ++i)
		{
			_data[i] ^= key;
			key = static_cast<uint8_t>(CRandomSimple::MakeSeedFrom(key));
		}

		for (size_t i = 0; i != Len; ++i)
			_data[i] = unmix(ror(_data[i], 3), i);

		_seed = 0;
	}

public:
	constexpr CSecureString(const char(&in)[Size]) noexcept
	{
		Pack(in);
	}

	template <typename T = char>
	constexpr CSecureString(const T *in) noexcept
	{
		Pack(in);
	}

	__forceinline const char *c_str() const noexcept
	{
		Unpack();
		return _data;
	}
};

#pragma pack(pop)

template <FNV1a64_t DllName, FNV1a64_t FuncName, typename T>
class CLazyImportFunc
{
	using T_t = std::remove_pointer_t<T>;

	T _ptr = nullptr;

	T GetFunction()
	{
		if (!_ptr)
			_ptr = reinterpret_cast<T>(Memoria::GetProcAddressDirect(DllName.Hash, FuncName.Hash));

		return _ptr;
	}

public:
	template <typename... Args>
	__forceinline auto operator()(Args&&... args) -> decltype(std::declval<T_t>()(std::forward<Args>(args)...))
	{
		using ReturnT = decltype(std::declval<T_t>()(std::forward<Args>(args)...));

		if (auto fn = GetFunction(); fn != nullptr)
		{
			if constexpr (std::is_void_v<ReturnT>)
				fn(std::forward<Args>(args)...);
			else
				return fn(std::forward<Args>(args)...);
		}
		else
		{
			if constexpr (!std::is_void_v<ReturnT>)
				return {};
		}
	}

	explicit operator bool()
	{
		return GetFunction() != nullptr;
	}
};

#pragma pop_macro("max")

MEMORIA_END

#define STRING(str) []() [[msvc::forceinline]] { constexpr auto s = Memoria::CSecureString(str); return s; }().c_str()

#define LI_FN(func) Memoria::CLazyImportFunc<"", #func, decltype(&func)>()
#define LI_FN_EX(dll, func) Memoria::CLazyImportFunc<dll, #func, decltype(&func)>()

#define LI_FN_DECL(func) Memoria::CLazyImportFunc<"", #func, decltype(&func)> func;
#define LI_FN_DECL_EX(dll, func) Memoria::CLazyImportFunc<dll, #func, decltype(&func)> func;