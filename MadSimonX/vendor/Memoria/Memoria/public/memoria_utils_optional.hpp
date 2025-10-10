//
// memoria_utils_optional.hpp
// 
// An alternative to std::optional<T> to eliminate dependency on CRT.
// 
// The Memoria version does not have overloaded operators like bool 
// to encourage writing more explicit code.
//

#pragma once

#include "memoria_common.hpp"

#include <optional>

MEMORIA_BEGIN

template<typename T>
class Optional
{
private:
	bool _has_value = false;
	alignas(T) unsigned char _data[sizeof(T)] = {};

	T *ptr()
	{
		return reinterpret_cast<T *>(&_data);
	}

	const T *ptr() const
	{
		return reinterpret_cast<const T *>(&_data);
	}

public:
	constexpr Optional() noexcept = default;

	constexpr Optional(std::nullopt_t) noexcept
		: _has_value(false)
	{
	}

	Optional(const T &value)
		: _has_value(true)
	{
		std::construct_at(ptr(), value);
	}

	Optional(T &&value)
		: _has_value(true)
	{
		std::construct_at(ptr(), std::move(value));
	}

	Optional(const Optional &other)
		: _has_value(other._has_value)
	{
		if (_has_value)
		{
			std::construct_at(ptr(), *other.ptr());
		}
	}

	Optional(Optional &&other) noexcept
		: _has_value(other._has_value)
	{
		if (_has_value)
		{
			std::construct_at(ptr(), std::move(*other.ptr()));
			other.reset();
		}
	}

	Optional &operator=(const Optional &other)
	{
		if (this != &other)
		{
			reset();
			if (other._has_value)
			{
				std::construct_at(ptr(), *other.ptr());
				_has_value = true;
			}
		}
		return *this;
	}

	Optional &operator=(Optional &&other) noexcept
	{
		if (this != &other)
		{
			reset();
			if (other._has_value)
			{
				std::construct_at(ptr(), std::move(*other.ptr()));
				_has_value = true;
				other.reset();
			}
		}
		return *this;
	}

	Optional &operator=(std::nullopt_t) noexcept
	{
		reset();
		return *this;
	}

	void reset()
	{
		if (_has_value)
		{
			std::destroy_at(ptr());
			_has_value = false;
		}
	}

	T &value()
	{
		return *ptr();
	}

	const T &value() const
	{
		return *ptr();
	}

	bool has_value() const
	{
		return _has_value;
	}
};

MEMORIA_END