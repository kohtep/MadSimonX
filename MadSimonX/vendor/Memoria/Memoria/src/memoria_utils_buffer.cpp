#include "memoria_utils_buffer.hpp"

#include "memoria_core_write.hpp"
#include "memoria_core_misc.hpp"

MEMORIA_BEGIN

bool CBuffer::Seek(ptrdiff_t offset, SeekDir dir)
{
	ptrdiff_t new_pos = 0;

	switch (dir)
	{
	case SeekDir::Begin:
		new_pos = offset;
		break;

	case SeekDir::Current:
		new_pos = static_cast<ptrdiff_t>(_pos) + offset;
		break;

	case SeekDir::End:
		new_pos = static_cast<ptrdiff_t>(_size) + offset;
		break;
	}

	if (new_pos < 0 || static_cast<size_t>(new_pos) > _size)
		return false;

	_pos = static_cast<size_t>(new_pos);
	return true;
}

bool CBuffer::Clone(void *dest, bool is_code, ptrdiff_t offset)
{
	if (!_data || _pos == 0)
		return false;

	if (is_code)
	{
		return WriteMemory(dest, _data, _pos, offset);
	}
	else
	{
		memcpy(PtrOffset(dest, offset), _data, _pos);
		return true;
	}
}

void CWriteBuffer::WriteU8(uint8_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteU16(uint16_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteU32(uint32_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteU64(uint64_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteI8(int8_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteI16(int16_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteI32(int32_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteI64(int64_t value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteFloat(float value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WriteDouble(double value)
{
	WriteData(&value, sizeof(value));
}

void CWriteBuffer::WritePointer(const void *value)
{
	WriteData(&value, sizeof(value));
}

bool CWriteBuffer::WriteRelative(const void *addr_target, const void *addr_value, ptrdiff_t additional_offset)
{
	if (addr_value && !IsIn32BitRange(addr_target, addr_value))
		return false;

#pragma warning(suppress : 4244) // conversion from '__int64' to 'uint32_t', possible loss of data
	uint32_t rel = (uint8_t *)addr_value - ((uint8_t *)addr_target + sizeof(int32_t) + additional_offset);

	WriteU32(rel);

	return true;
}

void CWriteBuffer::WriteString(const char *str)
{
	if (!str) 
		return;

	size_t len = strlen(str);
	WriteData(str, len + 1);
}

void CWriteBuffer::WriteData(const void *src, size_t size)
{
	if (_pos + size > _size)
		return;

	memcpy(&_data[_pos], src, size);
	_pos += size;
}

uint8_t CReadBuffer::ReadU8()
{
	return (_pos < _size) ? _data[_pos++] : 0;
}

uint16_t CReadBuffer::ReadU16()
{
	if (_pos + 2 > _size)
		return 0;

	uint16_t value;
	memcpy(&value, &_data[_pos], sizeof(value));
	_pos += 2;

	return value;
}

uint32_t CReadBuffer::ReadU32()
{
	if (_pos + 4 > _size)
		return 0;

	uint32_t value;
	memcpy(&value, &_data[_pos], sizeof(value));
	_pos += 4;

	return value;
}

uint64_t CReadBuffer::ReadU64()
{
	if (_pos + 8 > _size)
		return 0;

	uint64_t value;
	memcpy(&value, &_data[_pos], sizeof(value));
	_pos += 8;

	return value;
}

int8_t CReadBuffer::ReadI8()
{
	return static_cast<int8_t>(ReadU8());
}

int16_t CReadBuffer::ReadI16()
{
	return static_cast<int16_t>(ReadU16());
}

int32_t CReadBuffer::ReadI32()
{
	return static_cast<int32_t>(ReadU32());
}

int64_t CReadBuffer::ReadI64()
{
	return static_cast<int64_t>(ReadU64());
}

const char *CReadBuffer::ReadString()
{
	size_t startPos = _pos;
	while (_pos < _size && _data[_pos] != '\0')
	{
		++_pos;
	}

	if (_pos < _size)
	{
		++_pos;
	}

	return reinterpret_cast<const char *>(&_data[startPos]);
}

void CReadBuffer::ReadData(void *dest, size_t size)
{
	if (_pos + size > _size)
		return;

	memcpy(dest, &_data[_pos], size);
	_pos += size;
}

bool CReadBuffer::IsEndOfBuffer() const
{
	return _pos >= _size;
}

MEMORIA_END