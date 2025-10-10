//
// memoria_utils_buffer.hpp
// 
// A simple buffer for convenient construction of binary data.
// 
// Primary usage in Memoria — generating trampolines for hooks,
// but it can be used anywhere.
//

#pragma once

#include "memoria_common.hpp"

#include <cstdint>
#include <cstddef>
#include <cstring>

MEMORIA_BEGIN

class CBuffer
{
public:
	enum class SeekDir { Begin, Current, End };

protected:
	uint8_t *_data;
	size_t _size;
	size_t _pos;

public:
	CBuffer(void *data, size_t size) : _data(reinterpret_cast<uint8_t *>(data)), _size(size), _pos(0) {}

	bool Clone(void *dest, bool is_code = false, ptrdiff_t offset = 0);

	uint8_t *GetData() const { return _data; }
	uint8_t *GetPointer() const { return _data ? &_data[_pos] : nullptr; }
	size_t GetCapacity() const { return _size; }
	size_t GetOffset() const { return _pos; }

	bool Seek(ptrdiff_t offset, SeekDir dir);
};

class CWriteBuffer : public CBuffer
{
public:
	CWriteBuffer(void *data, size_t size) : CBuffer(data, size) {}

	size_t GetSize() const { return _pos; }

	void WriteU8(uint8_t value);
	void WriteU16(uint16_t value);
	void WriteU32(uint32_t value);
	void WriteU64(uint64_t value);

	void WriteI8(int8_t value);
	void WriteI16(int16_t value);
	void WriteI32(int32_t value);
	void WriteI64(int64_t value);

	void WriteFloat(float value);
	void WriteDouble(double value);
	void WritePointer(const void *value);

	// Most of the time, the 'additional_offset' parameter can be set to 0. Alternative values are possible
	// when it is necessary to specify an additional offset. Example:
	//
	// CMP BYTE PTR [RIP+0x12345678], 0x7F
	// 80 3D [XX XX XX XX] 7F
	//
	// Where 'XX XX XX XX' is 0x12345678, followed by 0x7F. In this case, 'additional_offset' should be set to '2'.
	bool WriteRelative(const void *addr_target, const void *addr_value, ptrdiff_t additional_offset = 1);

	void WriteString(const char *str);
	void WriteData(const void *src, size_t size);
};

template <size_t size>
class CIndependentBuffer : public CWriteBuffer
{
private:
	uint8_t _buf[size];

public:
	CIndependentBuffer() : CWriteBuffer(&_buf, size) {}
};

using CIndependentBuffer64 = CIndependentBuffer<64>;
using CIndependentBuffer256 = CIndependentBuffer<256>;
using CIndependentBuffer1024 = CIndependentBuffer<1024>;

class CReadBuffer : public CBuffer
{
public:
	CReadBuffer(const void *data, size_t size) : CBuffer(const_cast<void *>(data), size) {}

	size_t GetPosition() const { return _pos; }

	uint8_t ReadU8();
	uint16_t ReadU16();
	uint32_t ReadU32();
	uint64_t ReadU64();

	int8_t ReadI8();
	int16_t ReadI16();
	int32_t ReadI32();
	int64_t ReadI64();

	const char *ReadString();
	void ReadData(void *dest, size_t size);

	bool IsEndOfBuffer() const;
};

MEMORIA_END