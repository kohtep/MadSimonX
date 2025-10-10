#include "memoria_utils_unicode.hpp"

#include <Windows.h>
#include <memory>

MEMORIA_BEGIN

CUTF8Char::CUTF8Char()
{
	memset(_bytes, 0, kMaxUtf8CharSize);
}

CUTF8Char::CUTF8Char(const char *data)
{
	if (!data || !*data)
	{
		memset(_bytes, 0, kMaxUtf8CharSize);
		return;
	}

	size_t length = Utf8SeqLength(data[0]);

	if (length > kMaxUtf8CharSize)
	{
		memset(_bytes, 0, kMaxUtf8CharSize);
		return;
	}

	memset(&_bytes[length], 0, kMaxUtf8CharSize - length);

	switch (length)
	{
	case 4:
		_bytes[3] = data[3];
		[[fallthrough]];
	case 3:
		_bytes[2] = data[2];
		[[fallthrough]];
	case 2:
		_bytes[1] = data[1];
		[[fallthrough]];
	case 1:
		_bytes[0] = data[0];
		[[fallthrough]];
	default:
		break;
	}
}

const char *CUTF8Char::data() const
{
	return _bytes;
}

size_t CUTF8Char::size() const
{
	return Utf8SeqLength(_bytes[0]);
}

bool CUTF8Char::empty() const
{
	return _bytes[0] == 0;
}

void CUTF8Char::clear()
{
	memset(_bytes, 0, kMaxUtf8CharSize);
}

bool CUTF8Char::operator==(const CUTF8Char &other) const
{
	size_t len = size();
	if (len != other.size())
	{
		return false;
	}

	for (size_t i = 0; i < len; ++i)
	{
		if (_bytes[i] != other._bytes[i])
		{
			return false;
		}
	}

	return true;
}

bool CUTF8Char::operator!=(const CUTF8Char &other) const
{
	return !(*this == other);
}

uint32_t CUTF8Char::to_codepoint() const
{
	size_t len = size();

	if (len == 1)
	{
		return static_cast<unsigned char>(_bytes[0]);
	}
	else if (len == 2)
	{
		return ((static_cast<unsigned char>(_bytes[0]) & 0x1F) << 6) |
			(static_cast<unsigned char>(_bytes[1]) & 0x3F);
	}
	else if (len == 3)
	{
		return ((static_cast<unsigned char>(_bytes[0]) & 0x0F) << 12) |
			((static_cast<unsigned char>(_bytes[1]) & 0x3F) << 6) |
			(static_cast<unsigned char>(_bytes[2]) & 0x3F);
	}
	else if (len == 4)
	{
		return ((static_cast<unsigned char>(_bytes[0]) & 0x07) << 18) |
			((static_cast<unsigned char>(_bytes[1]) & 0x3F) << 12) |
			((static_cast<unsigned char>(_bytes[2]) & 0x3F) << 6) |
			(static_cast<unsigned char>(_bytes[3]) & 0x3F);
	}

	return 0;
}

size_t CUTF8Char::write(char *buffer) const
{
	size_t len = size();
	for (size_t i = 0; i < len; ++i)
	{
		buffer[i] = _bytes[i];
	}
	return len;
}

CUChar::CUChar() = default;

CUChar::CUChar(uint32_t value)
	: _value(value)
{
}

uint32_t CUChar::value() const
{
	return _value;
}

void CUChar::set_value(uint32_t value)
{
	_value = value;
}

bool CUChar::empty() const
{
	return _value == 0;
}

void CUChar::clear()
{
	_value = 0;
}

bool CUChar::operator==(const CUChar &other) const
{
	return _value == other._value;
}

bool CUChar::operator!=(const CUChar &other) const
{
	return _value != other._value;
}

bool CUChar::is_valid() const
{
	return IsValidUnicode(_value);
}

size_t CUChar::encode_utf8(char *buffer) const
{
	return EncodeUtf8(_value, buffer);
}

size_t CUChar::encode_utf16(char *buffer) const
{
	return EncodeUtf16(_value, buffer);
}

size_t CUChar::encode_utf32(char *buffer) const
{
	return EncodeUtf32(_value, buffer);
}

size_t CUChar::decode_utf8(const char *buffer)
{
	return DecodeUtf8(buffer, _value);
}

size_t CUChar::decode_utf16(const char *buffer)
{
	return DecodeUtf16(buffer, _value);
}

size_t CUChar::decode_utf32(const char *buffer)
{
	return DecodeUtf32(buffer, _value);
}

static uint32_t UnicodeToUpper(uint32_t cp)
{
	//
	// ASCII
	//

	if (cp >= 'a' && cp <= 'z')
		return cp - 32;

	//
	// Latin with diacritics (partially)
	//

	if (cp >= 0x00E0 && cp <= 0x00F6)
		return cp - 0x20; // à–ö
	if (cp >= 0x00F8 && cp <= 0x00FE)
		return cp - 0x20; // ø–þ

	if (cp == 0x00FF)
		return 0x0178; // ÿ → Ÿ

	//
	// Cyrillic
	//

	if (cp >= 0x0430 && cp <= 0x044F)
		return cp - 0x20; // а–я → А–Я

	if (cp == 0x0451)
		return 0x0401; // ё → Ё

	return cp;
}

static uint32_t UnicodeToLower(uint32_t cp)
{
	//
	// ASCII
	//

	if (cp >= 'A' && cp <= 'Z')
		return cp + 32;

	//
	// Latin with diacritics (partially)
	//

	if (cp >= 0x00C0 && cp <= 0x00D6)
		return cp + 0x20; // À–Ö
	if (cp >= 0x00D8 && cp <= 0x00DE)
		return cp + 0x20; // Ø–Þ

	if (cp == 0x0178)
		return 0x00FF; // Ÿ → ÿ

	//
	// Cyrillic
	//

	if (cp >= 0x0410 && cp <= 0x042F)
		return cp + 0x20; // А–Я -> а–я

	if (cp == 0x0401)
		return 0x0451; // Ё -> ё

	return cp;
}

bool CUTF8StringBase::transform_case(eStringCase mode)
{
	if (!_buffer || _length == 0)
		return true;

	size_t offset = 0;

	while (offset < _length)
	{
		uint32_t codepoint;
		size_t original_len = DecodeUtf8(_buffer + offset, codepoint);
		if (original_len == 0)
			return false;

		uint32_t new_cp = codepoint;
		if (mode == eStringCase::Lower)
			new_cp = UnicodeToLower(codepoint);
		else if (mode == eStringCase::Upper)
			new_cp = UnicodeToUpper(codepoint);

		char encoded[kMaxUtf8CharSize];
		size_t new_len = EncodeUtf8(new_cp, encoded);
		if (new_len == 0)
			return false;

		ptrdiff_t diff = static_cast<ptrdiff_t>(new_len) - static_cast<ptrdiff_t>(original_len);

		if (diff > 0)
		{
			if (_length + diff >= _capacity)
				return false;

			memmove(_buffer + offset + new_len, _buffer + offset + original_len, _length - offset - original_len);
		}
		else if (diff < 0)
		{
			memmove(_buffer + offset + new_len, _buffer + offset + original_len, _length - offset - original_len);
		}

		for (size_t i = 0; i < new_len; ++i)
			_buffer[offset + i] = encoded[i];

		offset += new_len;
		_length += diff;
	}

	_buffer[_length] = '\0';
	return true;
}

bool CUTF8StringBase::ensure_capacity(size_t additional_size)
{
	if (_length + additional_size >= _capacity)
	{
		if (!_expandable)
			return false;

		size_t new_capacity = _capacity;
		while (_length + additional_size >= new_capacity)
			new_capacity += 256;

		char *buffer = reinterpret_cast<char *>(realloc(_buffer, new_capacity));
		if (!buffer)
			return false;

		_capacity = new_capacity;
		_buffer = buffer;
	}
	return true;
}

const char *CUTF8StringBase::c_str() const
{
	return _buffer ? _buffer : "";
}

const char *CUTF8StringBase::data() const
{
	return _buffer ? _buffer : "";
}

size_t CUTF8StringBase::size() const
{
	return _length;
}

bool CUTF8StringBase::empty() const
{
	return _length == 0;
}

void CUTF8StringBase::clear()
{
	if (_buffer)
	{
		memset(_buffer, 0, _length + 1);
		_length = 0;
	}
	else
	{
		_length = 0;
	}
}

bool CUTF8StringBase::push_back(uint32_t codepoint)
{
	if (!ensure_capacity(kMaxUtf8CharSize + 1)) // +1 for '\0'
		return false;

	char temp[kMaxUtf8CharSize];
	size_t char_len = EncodeUtf8(codepoint, temp);
	if (char_len == 0)
		return false;

	for (size_t i = 0; i < char_len; ++i)
		_buffer[_length + i] = temp[i];

	_length += char_len;
	_buffer[_length] = '\0';

	return true;
}

bool CUTF8StringBase::pop_back()
{
	if (_length == 0)
		return false;

	size_t i = _length;
	while (i > 0)
	{
		--i;
		unsigned char c = static_cast<unsigned char>(_buffer[i]);
		if ((c & 0xC0) != 0x80)
		{
			_length = i;
			_buffer[_length] = '\0';
			return true;
		}
	}
	return false;
}

bool CUTF8StringBase::from_string(const char *str)
{
	if (!str)
		return false;

	_length = 0;

	if (_expandable && _buffer)
	{
		free(_buffer);
		_buffer = nullptr;
		_capacity = 0;
	}

	if (!ensure_capacity(1))
		return false;

	size_t offset = 0;
	while (str[offset] != '\0')
	{
		size_t char_len = 0;
		if (!IsValidUtf8Char(str + offset, static_cast<size_t>(-1), &char_len))
		{
			clear();
			return false;
		}

		if (!ensure_capacity(char_len + 1)) // +1 for '\0'
		{
			clear();
			return false;
		}

		for (size_t i = 0; i < char_len; ++i)
			_buffer[_length + i] = str[offset + i];

		_length += char_len;
		offset += char_len;
	}

	_buffer[_length] = '\0';
	return true;
}

bool CUTF8StringBase::append(const char *str)
{
	if (!str)
		return false;

	size_t offset = 0;

	while (str[offset] != '\0')
	{
		size_t char_len;
		if (!IsValidUtf8Char(str + offset, static_cast<size_t>(-1), &char_len))
			return false;

		if (!ensure_capacity(char_len))
			return false;

		for (size_t i = 0; i < char_len; ++i)
			_buffer[_length + i] = str[offset + i];

		_length += char_len;
		offset += char_len;
	}

	_buffer[_length] = '\0';
	return true;
}

bool CUTF8StringBase::validate() const
{
	return IsValidUtf8String(_buffer, _length);
}

CUTF8String::CUTF8String()
{
	_buffer = nullptr;
	_length = 0;
	_capacity = 0;
	_expandable = true;
}

CUTF8String::CUTF8String(const char *text) : CUTF8String()
{
	if (!from_string(text))
		clear();
}

CUTF8String::CUTF8String(const char8_t *text) : CUTF8String()
{
	if (!from_string(reinterpret_cast<const char *>(text)))
		clear();
}

CUTF8String::~CUTF8String()
{
	free(_buffer);
}

CUTF8String::CUTF8String(const CUTF8String &other)
{
	_expandable = true;
	_length = other._length;
	_capacity = other._length + 1;

	_buffer = static_cast<char *>(malloc(_capacity));
	if (_buffer && other._buffer)
	{
		memcpy(_buffer, other._buffer, _length);
		_buffer[_length] = '\0';
	}
	else
	{
		_buffer = nullptr;
		_length = 0;
		_capacity = 0;
	}
}

CUTF8String &CUTF8String::operator=(const CUTF8String &other)
{
	if (this == &other)
		return *this;

	char *new_buffer = static_cast<char *>(malloc(other._length + 1));
	if (!new_buffer)
	{
		clear();
		return *this;
	}

	if (_buffer)
		free(_buffer);

	_buffer = new_buffer;
	_length = other._length;
	_capacity = other._length + 1;
	_expandable = true;

	memcpy(_buffer, other._buffer, _length);
	_buffer[_length] = '\0';

	return *this;
}

CUTF8String::CUTF8String(CUTF8String &&other) noexcept
{
	_buffer = other._buffer;
	_length = other._length;
	_capacity = other._capacity;
	_expandable = true;

	other._buffer = nullptr;
	other._length = 0;
	other._capacity = 0;
}

CUTF8String &CUTF8String::operator=(CUTF8String &&other) noexcept
{
	if (this == &other)
		return *this;

	if (_buffer)
		free(_buffer);

	_buffer = other._buffer;
	_length = other._length;
	_capacity = other._capacity;
	_expandable = true;

	other._buffer = nullptr;
	other._length = 0;
	other._capacity = 0;

	return *this;
}

CUTF8String &CUTF8String::operator=(const char *text)
{
	if (text == _buffer)
		return *this;

	if (!from_string(text))
		clear();

	return *this;
}

CUTF8String &CUTF8String::operator=(const char8_t *text)
{
	const char *ptr = reinterpret_cast<const char *>(text);
	if (ptr == _buffer)
		return *this;

	if (!from_string(ptr))
		clear();

	return *this;
}

static bool IsSurrogatePair(uint32_t value)
{
	return value >= 0xD800 && value <= 0xDFFF;
}

bool IsValidUnicode(uint32_t value)
{
	return value <= 0x10FFFF && !IsSurrogatePair(value);
}

bool IsValidUtf8Char(const char *utf8, size_t size, size_t *length)
{
	if (size == 0 || utf8 == nullptr)
		return false;

	unsigned char first = static_cast<unsigned char>(utf8[0]);
	size_t len = Utf8SeqLength(first);

	if (len == 0 || (size != static_cast<size_t>(-1) && len > size))
		return false;

	uint32_t codepoint;
	size_t decoded = DecodeUtf8(utf8, codepoint);
	if (decoded != len || !IsValidUnicode(codepoint))
		return false;

	if (length)
		*length = len;

	return true;
}

bool IsValidUtf16Char(const char *utf16, size_t size, size_t *length)
{
	if (!utf16 || (size < 2 && size != static_cast<size_t>(-1)))
		return false;

	uint32_t codepoint;
	size_t decoded = DecodeUtf16(utf16, codepoint);

	if (decoded == 0)
		return false;

	if (size != static_cast<size_t>(-1) && decoded > size)
		return false;

	if (!IsValidUnicode(codepoint))
		return false;

	if (length)
		*length = decoded;

	return true;
}

bool IsValidUtf32Char(const char *utf32, size_t size, size_t *length, bool bigEndian)
{
	if (!utf32 || (size != static_cast<size_t>(-1) && size < 4))
		return false;

	uint32_t codepoint;
	size_t decoded = DecodeUtf32(utf32, codepoint, bigEndian);

	if (!IsValidUnicode(codepoint))
		return false;

	if (length)
		*length = decoded;

	return true;
}

bool IsValidUtf8String(const char *utf8, size_t size)
{
	if (!utf8) return false;

	size_t offset = 0;

	if (size == static_cast<size_t>(-1))
	{
		while (utf8[offset] != '\0')
		{
			size_t len;
			if (!IsValidUtf8Char(utf8 + offset, static_cast<size_t>(-1), &len))
				return false;
			offset += len;
		}
		return true;
	}

	while (offset < size)
	{
		size_t len;
		if (!IsValidUtf8Char(utf8 + offset, size - offset, &len))
			return false;
		offset += len;
	}

	return true;
}

bool IsValidUtf16String(const char *utf16, size_t size)
{
	if (!utf16) return false;

	size_t offset = 0;

	if (size == static_cast<size_t>(-1))
	{
		while (!(utf16[offset] == 0 && utf16[offset + 1] == 0))
		{
			size_t len;
			if (!IsValidUtf16Char(utf16 + offset, static_cast<size_t>(-1), &len))
				return false;
			offset += len;
		}
		return true;
	}

	while (offset < size)
	{
		size_t len;
		if (!IsValidUtf16Char(utf16 + offset, size - offset, &len))
			return false;
		offset += len;
	}

	return true;
}

bool IsValidUtf32String(const char *utf32, size_t size, bool bigEndian)
{
	if (!utf32) return false;

	size_t offset = 0;

	if (size == static_cast<size_t>(-1))
	{
		while (!(utf32[offset] == 0 && utf32[offset + 1] == 0 &&
			utf32[offset + 2] == 0 && utf32[offset + 3] == 0))
		{
			size_t len;
			if (!IsValidUtf32Char(utf32 + offset, static_cast<size_t>(-1), &len, bigEndian))
				return false;
			offset += len;
		}
		return true;
	}

	while (offset < size)
	{
		size_t len;
		if (!IsValidUtf32Char(utf32 + offset, size - offset, &len, bigEndian))
			return false;
		offset += len;
	}

	return true;
}

uint32_t Utf8ToCodepoint(const uint8_t *bytes, size_t len)
{
	if (len == 1)
	{
		return static_cast<unsigned char>(bytes[0]);
	}
	else if (len == 2)
	{
		return ((static_cast<unsigned char>(bytes[0]) & 0x1F) << 6) |
			(static_cast<unsigned char>(bytes[1]) & 0x3F);
	}
	else if (len == 3)
	{
		return ((static_cast<unsigned char>(bytes[0]) & 0x0F) << 12) |
			((static_cast<unsigned char>(bytes[1]) & 0x3F) << 6) |
			(static_cast<unsigned char>(bytes[2]) & 0x3F);
	}
	else if (len == 4)
	{
		return ((static_cast<unsigned char>(bytes[0]) & 0x07) << 18) |
			((static_cast<unsigned char>(bytes[1]) & 0x3F) << 12) |
			((static_cast<unsigned char>(bytes[2]) & 0x3F) << 6) |
			(static_cast<unsigned char>(bytes[3]) & 0x3F);
	}

	return 0;
}

size_t Utf8SeqLength(unsigned char first)
{
	if ((first & 0x80) == 0x00) return 1;
	if ((first & 0xE0) == 0xC0) return 2;
	if ((first & 0xF0) == 0xE0) return 3;
	if ((first & 0xF8) == 0xF0) return 4;
	return 0;
}

size_t EncodeUtf8(uint32_t value, char *buffer)
{
	if (value <= 0x7F)
	{
		buffer[0] = static_cast<char>(value);
		return 1;
	}

	if (value <= 0x7FF)
	{
		buffer[0] = static_cast<char>(0xC0 | (value >> 6));
		buffer[1] = static_cast<char>(0x80 | (value & 0x3F));
		return 2;
	}

	if (value <= 0xFFFF)
	{
		buffer[0] = static_cast<char>(0xE0 | (value >> 12));
		buffer[1] = static_cast<char>(0x80 | ((value >> 6) & 0x3F));
		buffer[2] = static_cast<char>(0x80 | (value & 0x3F));
		return 3;
	}

	if (value <= 0x10FFFF)
	{
		buffer[0] = static_cast<char>(0xF0 | (value >> 18));
		buffer[1] = static_cast<char>(0x80 | ((value >> 12) & 0x3F));
		buffer[2] = static_cast<char>(0x80 | ((value >> 6) & 0x3F));
		buffer[3] = static_cast<char>(0x80 | (value & 0x3F));
		return 4;
	}

	return 0;
}

size_t EncodeUtf16(uint32_t value, char *buffer)
{
	if (value <= 0xFFFF)
	{
		buffer[0] = static_cast<char>(value >> 8);
		buffer[1] = static_cast<char>(value & 0xFF);
		return 2;
	}

	uint32_t cp = value - 0x10000;
	uint16_t high = 0xD800 + (cp >> 10);
	uint16_t low = 0xDC00 + (cp & 0x3FF);

	buffer[0] = static_cast<char>(high >> 8);
	buffer[1] = static_cast<char>(high & 0xFF);
	buffer[2] = static_cast<char>(low >> 8);
	buffer[3] = static_cast<char>(low & 0xFF);

	return 4;
}

size_t EncodeUtf32(uint32_t value, char *buffer, bool bigEndian)
{
	if (bigEndian)
	{
		buffer[0] = static_cast<char>((value >> 24) & 0xFF);
		buffer[1] = static_cast<char>((value >> 16) & 0xFF);
		buffer[2] = static_cast<char>((value >> 8) & 0xFF);
		buffer[3] = static_cast<char>(value & 0xFF);
	}
	else
	{
		buffer[3] = static_cast<char>((value >> 24) & 0xFF);
		buffer[2] = static_cast<char>((value >> 16) & 0xFF);
		buffer[1] = static_cast<char>((value >> 8) & 0xFF);
		buffer[0] = static_cast<char>(value & 0xFF);
	}

	return 4;
}

size_t DecodeUtf8(const char *buffer, uint32_t &value)
{
	unsigned char c = static_cast<unsigned char>(buffer[0]);

	if (c <= 0x7F)
	{
		value = c;
		return 1;
	}
	else if ((c & 0xE0) == 0xC0)
	{
		value = ((c & 0x1F) << 6) |
			(static_cast<unsigned char>(buffer[1]) & 0x3F);
		if (value < 0x80) return 0;  // Overlong
		return 2;
	}
	else if ((c & 0xF0) == 0xE0)
	{
		value = ((c & 0x0F) << 12) |
			((static_cast<unsigned char>(buffer[1]) & 0x3F) << 6) |
			(static_cast<unsigned char>(buffer[2]) & 0x3F);
		if (value < 0x800) return 0;  // Overlong
		return 3;
	}
	else if ((c & 0xF8) == 0xF0)
	{
		value = ((c & 0x07) << 18) |
			((static_cast<unsigned char>(buffer[1]) & 0x3F) << 12) |
			((static_cast<unsigned char>(buffer[2]) & 0x3F) << 6) |
			(static_cast<unsigned char>(buffer[3]) & 0x3F);
		if (value < 0x10000) return 0;  // Overlong
		return 4;
	}

	return 0;
}

size_t DecodeUtf16(const char *buffer, uint32_t &value)
{
	uint16_t word1 = (static_cast<uint8_t>(buffer[0]) << 8) | static_cast<uint8_t>(buffer[1]);

	if (word1 >= 0xD800 && word1 <= 0xDBFF)
	{
		uint16_t word2 = (static_cast<uint8_t>(buffer[2]) << 8) | static_cast<uint8_t>(buffer[3]);
		if (word2 < 0xDC00 || word2 > 0xDFFF)
			return 0;

		value = (((word1 - 0xD800) << 10) | (word2 - 0xDC00)) + 0x10000;
		return 4;
	}

	if (word1 >= 0xDC00 && word1 <= 0xDFFF)
		return 0;

	value = word1;
	return 2;
}

size_t DecodeUtf32(const char *buffer, uint32_t &value, bool bigEndian)
{
	if (bigEndian)
	{
		value = (static_cast<uint8_t>(buffer[0]) << 24) |
			(static_cast<uint8_t>(buffer[1]) << 16) |
			(static_cast<uint8_t>(buffer[2]) << 8) |
			static_cast<uint8_t>(buffer[3]);
	}
	else
	{
		value = (static_cast<uint8_t>(buffer[3]) << 24) |
			(static_cast<uint8_t>(buffer[2]) << 16) |
			(static_cast<uint8_t>(buffer[1]) << 8) |
			static_cast<uint8_t>(buffer[0]);
	}

	return 4;
}

MEMORIA_END