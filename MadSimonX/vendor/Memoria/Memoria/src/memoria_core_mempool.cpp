#include "memoria_core_mempool.hpp"

#include "memoria_core_misc.hpp"
#include "memoria_utils_assert.hpp"
#include "memoria_utils_list.hpp"

#include <Windows.h>

MEMORIA_BEGIN

class CMemoryChunk
{
public:
	// true : VirtualAlloc
	// false: HeapAlloc
	bool _is_virtual = false;

	void *_chunk = nullptr;

public:
	CMemoryChunk() = default;
	CMemoryChunk(void *chunk, bool is_virtual) : _chunk(chunk), _is_virtual(is_virtual) {}

	~CMemoryChunk()
	{ 
		bool freed = false;

		if (_chunk)
		{
			if (_is_virtual)
				freed = VirtualFree(_chunk, 0, MEM_RELEASE) != FALSE;
			else
				freed = HeapFree(GetProcessHeap(), 0, _chunk) != FALSE;
		}

		Assert(freed);
	}

	bool IsVirtual() const { return _is_virtual; }
	void *GetPointer() const { return _chunk; }
};

static Memoria::List<CMemoryChunk> AllocatedChunks;

static DWORD CreateVirtualFlags(bool bExecutable, bool bReadable, bool bWritable)
{
	DWORD flags{};

	if (bExecutable)
	{
		if (bReadable && bWritable)
		{
			flags = PAGE_EXECUTE_READWRITE;
		}
		else if (bReadable && !bWritable)
		{
			flags = PAGE_EXECUTE_READ;
		}
		else
		{
			flags = PAGE_EXECUTE;
		}
	}
	else
	{
		if (bReadable && bWritable)
		{
			flags = PAGE_READWRITE;
		}
		else if (bReadable && !bWritable)
		{
			flags = PAGE_READONLY;
		}
		else
		{
			flags = PAGE_NOACCESS;
		}
	}

	return flags;
}

static void *AllocEx(const void *addr_source, size_t size, bool is_executable, bool is_readable, bool is_writable)
{
	Assert(size != 0);

	if (size == 0)
		return nullptr;

	void *result;
	bool is_virtual;

	if (size >= 4096)
	{
		DWORD flags = CreateVirtualFlags(is_executable, is_readable, is_writable);
		DWORD type = addr_source ? MEM_RESERVE | MEM_COMMIT : MEM_COMMIT;

		result = VirtualAlloc(const_cast<LPVOID>(addr_source), size, type, flags);
		is_virtual = true;
	}
	else
	{
		result = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
		is_virtual = false;
	}

	if (!result)
		return nullptr;

	AllocatedChunks.emplace_front(result, is_virtual);
	return result;
}

bool Free(void *addr)
{
	for (auto it = AllocatedChunks.begin(); it != AllocatedChunks.end(); ++it)
	{
		if (it->GetPointer() == addr)
		{
			AllocatedChunks.erase(it);
			return true;
		}
	}

	return false;
}

bool FreeAll()
{
	AllocatedChunks.clear();
	return true;
}

void *New(size_t size)
{
	return AllocEx(nullptr, size, false, true, true);
}

void *Alloc(size_t size, bool is_executable, bool is_readable, bool is_writable)
{
	return AllocEx(nullptr, size, is_executable, is_readable, is_writable);
}

void *AllocNear(const void *addr_source, size_t size, bool is_executable, bool is_readable, bool is_writable)
{
	if (!addr_source)
		return nullptr;

	size = Align(size, 4096);

	uintptr_t addr = reinterpret_cast<uintptr_t>(addr_source);

#ifdef MEMORIA_32BIT
	uintptr_t max_addr = 0x7FFFFFFF;
#else
	uintptr_t max_addr = addr + 0x7FFFFFFF;
#endif

	while (addr < max_addr)
	{
		void *mem = AllocEx(reinterpret_cast<void *>(addr), size, is_executable, is_readable, is_writable);

		if (mem)
			return mem;

		addr += size;
	}

	return nullptr;
}

void *AllocFar(const void *addr_source, size_t size, bool is_executable, bool is_readable, bool is_writable)
{
	if (!addr_source)
		return nullptr;

	size = Align(size, 4096);

#ifdef MEMORIA_32BIT
	uintptr_t addr = 0x7FFFFFFF;
#else
	uintptr_t addr = reinterpret_cast<uintptr_t>(addr_source) + 0x7FFFFFFF;
#endif

	addr = (addr - size) & ~15;
	uintptr_t min_addr = addr_source ? reinterpret_cast<uintptr_t>(addr_source) : 0;

	while (addr > min_addr)
	{
		void *mem = AllocEx(reinterpret_cast<void *>(addr), size, is_executable, is_readable, is_writable);

		if (mem)
			return mem;

		addr = (addr - size) & ~15;
	}

	return nullptr;
}

MEMORIA_END