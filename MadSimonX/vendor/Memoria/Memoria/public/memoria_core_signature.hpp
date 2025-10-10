#pragma once

#include "memoria_common.hpp"
#include "memoria_utils_vector.hpp"
#include "memoria_utils_optional.hpp"
#include "memoria_utils_unicode.hpp"

MEMORIA_BEGIN

class CSignature
{
public:
	Memoria::Vector<uint8_t> _payload;
	Memoria::Vector<uint8_t> _mask;
	bool _has_optionals;

public:
	CSignature() = delete;
	CSignature(const char *str);
	CSignature(const void *data, size_t size, Memoria::Optional<uint8_t> ignore_byte = std::nullopt);

	const Memoria::Vector<uint8_t> &GetPayload() const { return _payload; }
	const Memoria::Vector<uint8_t> &GetMask() const { return _mask; }

	bool IsEmpty() const;
	bool HasOptionals() const;

	Memoria::Vector<Memoria::Optional<uint8_t>> CreatePattern() const;

	bool Match(const void *addr) const;
};

MEMORIA_END