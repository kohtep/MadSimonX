#pragma once

#include "memoria_common.hpp"

#include <stdint.h>
#include <type_traits>

MEMORIA_BEGIN

inline constexpr uint32_t FNV1A_32_BASIS = 0x811c9dc5;
inline constexpr uint32_t FNV1A_32_PRIME = 0x01000193;

inline constexpr uint64_t FNV1A_64_BASIS = 0xcbf29ce484222325ull;
inline constexpr uint64_t FNV1A_64_PRIME = 0x00000100000001B3ull;

inline constexpr char tolower_constexpr(char ch)
{
	return (ch >= 'A' && ch <= 'Z') ? (ch + 'a' - 'A') : ch;
}

inline constexpr wchar_t towlower_constexpr(wchar_t ch)
{
	return (ch >= L'A' && ch <= L'Z') ? (ch + L'a' - L'A') : ch;
}

inline constexpr uint32_t FNV1a32(const char *const str, bool case_insensitive = true) noexcept
{
	return (str[0] == '\0')
		? FNV1A_32_BASIS
		: ((FNV1a32(str + 1, case_insensitive) ^
			static_cast<uint8_t>(case_insensitive ? tolower_constexpr(str[0]) : str[0]))
			* FNV1A_32_PRIME);
}

inline constexpr uint32_t FNV1a32(const wchar_t *const str, bool case_insensitive = true) noexcept
{
	return (str[0] == L'\0')
		? FNV1A_32_BASIS
		: ((FNV1a32(str + 1, case_insensitive) ^
			static_cast<uint32_t>(case_insensitive ? towlower_constexpr(str[0]) : str[0]))
			* FNV1A_32_PRIME);
}

inline constexpr uint64_t FNV1a64(const char *const str, bool case_insensitive = true) noexcept
{
	return (str[0] == '\0')
		? FNV1A_64_BASIS
		: ((FNV1a64(str + 1, case_insensitive) ^
			static_cast<uint8_t>(case_insensitive ? tolower_constexpr(str[0]) : str[0]))
			* FNV1A_64_PRIME);
}

inline constexpr uint64_t FNV1a64(const wchar_t *const str, bool case_insensitive = true) noexcept
{
	return (str[0] == L'\0')
		? FNV1A_64_BASIS
		: ((FNV1a64(str + 1, case_insensitive) ^
			static_cast<uint64_t>(case_insensitive ? towlower_constexpr(str[0]) : str[0]))
			* FNV1A_64_PRIME);
}

// Stub. Should be removed in the future.
using fnv1a_t = uint64_t;

//
// The FNV1a64_t and FNV1a32_t classes are necessary to enable usage like this:
//
//   gSomeMgr.Dispatch<"Init">();
//
// The `Dispatch` method itself looks like this:
//
//   template <Memoria::FNV1a64_t Name>
//   CallchainResult Dispatch()
//   {
//       return Dispatch<Name>();
//   }
//
// It is not possible to pass `Name` as an argument in such a way that the hash of the "Init"
// string is passed instead of the string itself. This is due to certain `constexpr` constraints,
// which require the variable to remain alive during compile-time evaluation.
// Microsoft's compiler does not consider a string that is guaranteed to stay alive
// as actually remaining so during execution. Therefore, such a string has to be passed
// as a template argument.
//

template <size_t N>
struct FNV1a64_t
{
	uint64_t Hash;

	constexpr FNV1a64_t(const char(&in)[N])
		: Hash{ N > 1 ? FNV1a64(in) : 0 } {}

	constexpr FNV1a64_t(const wchar_t(&in)[N])
		: Hash{ N > 1 ? FNV1a64(in) : 0 } {}
};

template <size_t N>
struct FNV1a32_t
{
	uint32_t Hash;

	constexpr FNV1a32_t(const char(&in)[N])
		: Hash{ N > 1 ? FNV1a32(in) : 0 } {}

	constexpr FNV1a32_t(const wchar_t(&in)[N])
		: Hash{ N > 1 ? FNV1a32(in) : 0 } {}
};

MEMORIA_END

#define FNV32(s) Memoria::FNV1a32(s)
#define FNV64(s) Memoria::FNV1a64(s)