#pragma once

#include "memoria_common.hpp"

#include <cstddef>
#include <cstdint>

MEMORIA_BEGIN

static constexpr size_t kMaxUtf8CharSize = 4;

extern bool IsValidUnicode(uint32_t value);

extern uint32_t Utf8ToCodepoint(const uint8_t *bytes, size_t len);
extern size_t Utf8SeqLength(unsigned char first);

extern bool IsValidUtf8Char(const char *utf8, size_t size = -1, size_t *length = nullptr);
extern bool IsValidUtf16Char(const char *utf16, size_t size = -1, size_t *length = nullptr);
extern bool IsValidUtf32Char(const char *utf32, size_t size = -1, size_t *length = nullptr, bool bigEndian = false);

extern bool IsValidUtf8String(const char *utf8, size_t size = -1);
extern bool IsValidUtf16String(const char *utf16, size_t size = -1);
extern bool IsValidUtf32String(const char *utf32, size_t size = -1, bool bigEndian = false);

extern size_t EncodeUtf8(uint32_t value, char *buffer);
extern size_t EncodeUtf16(uint32_t value, char *buffer);
extern size_t EncodeUtf32(uint32_t value, char *buffer, bool bigEndian = false);

extern size_t DecodeUtf8(const char *buffer, uint32_t &value);
extern size_t DecodeUtf16(const char *buffer, uint32_t &value);
extern size_t DecodeUtf32(const char *buffer, uint32_t &value, bool bigEndian = false);

class CUTF8Char
{
private:
	char _bytes[kMaxUtf8CharSize];

public:
	CUTF8Char();
	CUTF8Char(const char *data);

	const char *data() const;
	size_t size() const;
	bool empty() const;
	void clear();

	bool operator==(const CUTF8Char &other) const;
	bool operator!=(const CUTF8Char &other) const;

	uint32_t to_codepoint() const;
	size_t write(char *buffer) const;
};

//
// Base class for UTF-8 string. Implements all core logic related to data management,
// such as resizing, adding or removing data, etc.
//
// Under no circumstances should this class define its own destructor,
// as it must remain a POD-compatible type to avoid generating `atexit` overhead
// calls for some derived classes. All destructors, if needed, must be implemented
// in the derived classes.
//
// This class, as well as its descendants, should avoid the use of virtual tables
// to prevent additional overhead. In general, their use might be justified,
// but it's preferable to avoid them if possible.
//
class CUTF8StringBase
{
	enum class eStringCase
	{
		Upper = 0,
		Lower = 1,
	};

protected:
	char *_buffer = nullptr;
	size_t _length = 0;
	size_t _capacity = 0;

	bool _expandable = false;

private:
	bool transform_case(eStringCase mode);
	bool ensure_capacity(size_t additional_size);

public:
	constexpr CUTF8StringBase() noexcept = default;

	const char *c_str() const;
	const char *data() const;

	size_t size() const;
	bool empty() const;

	bool validate() const;

	void clear();

	bool push_back(uint32_t codepoint);
	bool pop_back();

	bool from_string(const char *str);
	bool append(const char *str);

	bool to_upper() { return transform_case(eStringCase::Upper); }
	bool to_lower() { return transform_case(eStringCase::Lower); }
};

class CUTF8String : public CUTF8StringBase
{
public:
	CUTF8String();
	CUTF8String(const char *text);
	CUTF8String(const char8_t *text);
	~CUTF8String();

	CUTF8String(const CUTF8String &other);
	CUTF8String &operator=(const CUTF8String &other);

	CUTF8String(CUTF8String &&other) noexcept;
	CUTF8String &operator=(CUTF8String &&other) noexcept;

	CUTF8String &operator=(const char *text);
	CUTF8String &operator=(const char8_t *text);
};

template <size_t MaxSize>
class CFixedUTF8String : public CUTF8StringBase
{
private:
	char8_t _container[(MaxSize + 1) * kMaxUtf8CharSize]{};

	void lazy_init()
	{
		this->_buffer = reinterpret_cast<char *>(this->_container);
		this->_buffer[0] = '\0';

		this->_capacity = MaxSize;
		this->_length = 0;
	}

public:
	constexpr CFixedUTF8String() noexcept = default;

	CFixedUTF8String(const char *text)
	{
		_buffer = reinterpret_cast<char *>(_container);
		_buffer[0] = '\0';

		_capacity = MaxSize;
		_length = 0;

		from_string(text);
	}

	CFixedUTF8String(const char8_t *text)
	{
		_buffer = reinterpret_cast<char *>(_container);
		_buffer[0] = '\0';

		_capacity = MaxSize;
		_length = 0;

		from_string(reinterpret_cast<const char *>(text));
	}

	CFixedUTF8String<MaxSize> &operator=(const char *str)
	{
		if (_capacity == 0)
			lazy_init();

		if (!from_string(str))
			clear();

		return *this;
	}

	CFixedUTF8String<MaxSize> &operator=(const char8_t *str)
	{
		if (_capacity == 0)
			lazy_init();

		if (!from_string(reinterpret_cast<const char *>(str)))
			clear();

		return *this;
	}
};

using CFixedUTF8String32   = CFixedUTF8String<32>;
using CFixedUTF8String64   = CFixedUTF8String<64>;
using CFixedUTF8String128  = CFixedUTF8String<128>;
using CFixedUTF8String256  = CFixedUTF8String<256>;
using CFixedUTF8String512  = CFixedUTF8String<512>;
using CFixedUTF8String1024 = CFixedUTF8String<1024>;

class CUChar
{
private:
	uint32_t _value = 0;

public:
	CUChar();
	explicit CUChar(uint32_t value);

	uint32_t value() const;
	void set_value(uint32_t value);

	bool empty() const;
	void clear();

	bool operator==(const CUChar &other) const;
	bool operator!=(const CUChar &other) const;

	bool is_valid() const;

	size_t encode_utf8(char *buffer) const;
	size_t encode_utf16(char *buffer) const;
	size_t encode_utf32(char *buffer) const;

	size_t decode_utf8(const char *buffer);
	size_t decode_utf16(const char *buffer);
	size_t decode_utf32(const char *buffer);
};

// TODO
class CStringBase
{
protected:
	char *_data = nullptr;
	size_t _length = 0;
	size_t _capacity = 0;

public:

};

template <size_t MaxSize>
class CFixedString
{
private:
	char _buffer[(MaxSize + 1)];
	size_t _length = 0;

public:
	CFixedString();
	CFixedString(const char *str);

	const char *c_str() const;
	const char *data() const;
	char *data();

	size_t size() const;
	size_t capacity() const;
	bool empty() const;

	void clear();

	char &operator[](size_t index);
	const char &operator[](size_t index) const;

	CFixedString &operator=(const char *str);
	bool operator==(const CFixedString &other) const;
	bool operator!=(const CFixedString &other) const;

	CFixedString &append(const char *str);
	CFixedString &operator+=(const char *str);

	void push_back(char ch);
	void pop_back();

	size_t find(char ch) const;
	size_t rfind(char ch) const;
	bool contains(char ch) const;

	CFixedString substr(size_t pos, size_t count) const;

	void to_upper();
	void to_lower();

	void trim();
	void trim_left();
	void trim_right();

	bool starts_with(const char *prefix) const;
	bool ends_with(const char *suffix) const;

	CFixedString &add_prefix(const char *str);
	CFixedString &add_suffix(const char *str);

	char *begin();
	char *end();
	const char *begin() const;
	const char *end() const;
	const char *cbegin() const;
	const char *cend() const;
};

template <size_t MaxSize>
CFixedString<MaxSize>::CFixedString()
{
	_buffer[0] = 0;
	_length = 0;
}

template <size_t MaxSize>
CFixedString<MaxSize>::CFixedString(const char *str)
{
	_length = 0;
	if (str)
	{
		while (str[_length] != char(0) && _length < MaxSize)
		{
			_buffer[_length] = str[_length];
			++_length;
		}
	}
	_buffer[_length] = char(0);
}

template <size_t MaxSize>
const char *CFixedString<MaxSize>::c_str() const
{
	return _buffer;
}

template <size_t MaxSize>
const char *CFixedString<MaxSize>::data() const
{
	return _buffer;
}

template <size_t MaxSize>
char *CFixedString<MaxSize>::data()
{
	return _buffer;
}

template <size_t MaxSize>
size_t CFixedString<MaxSize>::size() const
{
	return _length;
}

template <size_t MaxSize>
size_t CFixedString<MaxSize>::capacity() const
{
	return MaxSize;
}

template <size_t MaxSize>
bool CFixedString<MaxSize>::empty() const
{
	return _length == 0;
}

template <size_t MaxSize>
void CFixedString<MaxSize>::clear()
{
	_buffer[0] = char(0);
	_length = 0;
}

template <size_t MaxSize>
char &CFixedString<MaxSize>::operator[](size_t index)
{
	return _buffer[index < _length ? index : _length - 1];
}

template <size_t MaxSize>
const char &CFixedString<MaxSize>::operator[](size_t index) const
{
	return _buffer[index < _length ? index : _length - 1];
}

template <size_t MaxSize>
CFixedString<MaxSize> &CFixedString<MaxSize>::operator=(const char *str)
{
	_length = 0;
	if (str)
	{
		while (str[_length] != char(0) && _length < MaxSize)
		{
			_buffer[_length] = str[_length];
			++_length;
		}
	}
	_buffer[_length] = char(0);
	return *this;
}

template <size_t MaxSize>
bool CFixedString<MaxSize>::operator==(const CFixedString &other) const
{
	if (_length != other._length)
		return false;
	for (size_t i = 0; i < _length; ++i)
	{
		if (_buffer[i] != other._buffer[i])
			return false;
	}
	return true;
}

template <size_t MaxSize>
bool CFixedString<MaxSize>::operator!=(const CFixedString &other) const
{
	return !(*this == other);
}

template <size_t MaxSize>
CFixedString<MaxSize> &CFixedString<MaxSize>::append(const char *str)
{
	if (!str)
		return *this;

	size_t i = 0;
	while (str[i] != char(0) && _length < MaxSize)
	{
		_buffer[_length++] = str[i++];
	}
	_buffer[_length] = char(0);
	return *this;
}

template <size_t MaxSize>
CFixedString<MaxSize> &CFixedString<MaxSize>::operator+=(const char *str)
{
	return append(str);
}

template <size_t MaxSize>
void CFixedString<MaxSize>::push_back(char ch)
{
	if (_length < MaxSize)
	{
		_buffer[_length++] = ch;
		_buffer[_length] = char(0);
	}
}

template <size_t MaxSize>
void CFixedString<MaxSize>::pop_back()
{
	if (_length > 0)
	{
		--_length;
		_buffer[_length] = char(0);
	}
}

template <size_t MaxSize>
size_t CFixedString<MaxSize>::find(char ch) const
{
	for (size_t i = 0; i < _length; ++i)
	{
		if (_buffer[i] == ch)
			return i;
	}
	return _length;
}

template <size_t MaxSize>
size_t CFixedString<MaxSize>::rfind(char ch) const
{
	for (size_t i = _length; i > 0; --i)
	{
		if (_buffer[i - 1] == ch)
			return i - 1;
	}
	return _length;
}

template <size_t MaxSize>
bool CFixedString<MaxSize>::contains(char ch) const
{
	return find(ch) != _length;
}

template <size_t MaxSize>
CFixedString<MaxSize> CFixedString<MaxSize>::substr(size_t pos, size_t count) const
{
	CFixedString<MaxSize> result;
	if (pos >= _length)
		return result;

	size_t end = (pos + count < _length) ? pos + count : _length;
	for (size_t i = pos; i < end && result._length < MaxSize; ++i)
	{
		result._buffer[result._length++] = _buffer[i];
	}
	result._buffer[result._length] = char(0);
	return result;
}

template <size_t MaxSize>
void CFixedString<MaxSize>::to_upper()
{
	for (size_t i = 0; i < _length; ++i)
	{
		if (_buffer[i] >= 'a' && _buffer[i] <= 'z')
			_buffer[i] -= 'a' - 'A';
	}
}

template <size_t MaxSize>
void CFixedString<MaxSize>::to_lower()
{
	for (size_t i = 0; i < _length; ++i)
	{
		if (_buffer[i] >= 'A' && _buffer[i] <= 'Z')
			_buffer[i] += 'a' - 'A';
	}
}

template <size_t MaxSize>
void CFixedString<MaxSize>::trim_left()
{
	size_t start = 0;
	while (start < _length && (_buffer[start] == ' ' || _buffer[start] == '\t'))
	{
		++start;
	}

	if (start > 0)
	{
		size_t new_len = _length - start;
		for (size_t i = 0; i < new_len; ++i)
		{
			_buffer[i] = _buffer[start + i];
		}
		_length = new_len;
		_buffer[_length] = char(0);
	}
}

template <size_t MaxSize>
void CFixedString<MaxSize>::trim_right()
{
	while (_length > 0 && (_buffer[_length - 1] == ' ' || _buffer[_length - 1] == '\t'))
	{
		--_length;
	}
	_buffer[_length] = char(0);
}

template <size_t MaxSize>
void CFixedString<MaxSize>::trim()
{
	trim_left();
	trim_right();
}

template <size_t MaxSize>
bool CFixedString<MaxSize>::starts_with(const char *prefix) const
{
	size_t i = 0;
	while (prefix[i] != char(0))
	{
		if (i >= _length || _buffer[i] != prefix[i])
			return false;
		++i;
	}
	return true;
}

template <size_t MaxSize>
bool CFixedString<MaxSize>::ends_with(const char *suffix) const
{
	size_t slen = 0;
	while (suffix[slen] != char(0))
		++slen;

	if (slen > _length)
		return false;

	size_t offset = _length - slen;
	for (size_t i = 0; i < slen; ++i)
	{
		if (_buffer[offset + i] != suffix[i])
			return false;
	}
	return true;
}

template <size_t MaxSize>
CFixedString<MaxSize> &CFixedString<MaxSize>::add_prefix(const char *str)
{
	if (!str)
		return *this;

	size_t prefix_len = 0;
	while (str[prefix_len] != char(0))
		++prefix_len;

	if (prefix_len == 0 || prefix_len + _length > MaxSize)
		return *this;

	for (size_t i = _length; i > 0; --i)
	{
		_buffer[i + prefix_len - 1] = _buffer[i - 1];
	}

	for (size_t i = 0; i < prefix_len; ++i)
	{
		_buffer[i] = str[i];
	}

	_length += prefix_len;
	_buffer[_length] = char(0);
	return *this;
}

template <size_t MaxSize>
CFixedString<MaxSize> &CFixedString<MaxSize>::add_suffix(const char *str)
{
	return append(str);
}

template <size_t MaxSize>
char *CFixedString<MaxSize>::begin()
{
	return _buffer;
}

template <size_t MaxSize>
char *CFixedString<MaxSize>::end()
{
	return _buffer + _length;
}

template <size_t MaxSize>
const char *CFixedString<MaxSize>::begin() const
{
	return _buffer;
}

template <size_t MaxSize>
const char *CFixedString<MaxSize>::end() const
{
	return _buffer + _length;
}

template <size_t MaxSize>
const char *CFixedString<MaxSize>::cbegin() const
{
	return _buffer;
}

template <size_t MaxSize>
const char *CFixedString<MaxSize>::cend() const
{
	return _buffer + _length;
}

using CFixedString64 = CFixedString<64>;
using CFixedString128 = CFixedString<128>;
using CFixedString256 = CFixedString<256>;
using CFixedString512 = CFixedString<512>;
using CFixedString1024 = CFixedString<1024>;

MEMORIA_END

#define CString Memoria::CUTF8String
#define CString32 Memoria::CFixedUTF8String<32>
#define CString64 Memoria::CFixedUTF8String<64>
#define CString128 Memoria::CFixedUTF8String<128>
#define CString256 Memoria::CFixedUTF8String<256>
#define CString512 Memoria::CFixedUTF8String<512>
#define CString1024 Memoria::CFixedUTF8String<1024>