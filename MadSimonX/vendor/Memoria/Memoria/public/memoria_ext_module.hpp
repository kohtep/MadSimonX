#pragma once

#include "memoria_common.hpp"

#include "memoria_ext_sig.hpp"
#include "memoria_utils_list.hpp"

#include <memory>
#include <stdint.h>
#include <Windows.h>

MEMORIA_BEGIN

class CMemoryBlock
{
private:
	CMemoryBlock(const CMemoryBlock &) = delete;
	CMemoryBlock &operator=(const CMemoryBlock &) = delete;

protected:
	char *_name = {};

	const void *_address = {};
	size_t _size = {};

public:
	CMemoryBlock() = default;
	CMemoryBlock(const void *address, size_t size);

	const char *GetName() const;
	void *GetBase() const;
	size_t GetSize() const;
	void *GetLastByte() const;

	// Hooks
	// use 0 opcode value to hook all addresses

	size_t HookRefAddr(const void *addr_target, const void *addr_hook, uint16_t opcode = 0);
	size_t HookRefCall(const void *addr_target, const void *addr_hook); // 0xE8 CALLS only
	size_t HookRefJump(const void *addr_target, const void *addr_hook); // 0xE9 JUMPS only

	//
	// Signaturing
	//

	void Sig(const CSignature &signature, SigCallbackFn cb, void *lpParam);
	void Sig(const char *signature, SigCallbackFn cb, void *lpParam);
	void Sig(SigCallbackFn cb, void *lpParam);

	//
	// Static builders
	//

	static std::unique_ptr<CMemoryBlock> CreateFromAddress(const void *address, size_t size = 0);
};

enum class eSection : uint8_t
{
	Export,        // .rdata / .edata
	Import,        // .rdata / .idata
	Resource,      // .rsrc
	Exception,     // .pdata
	Security,      // [cannot be mapped]
	BaseReloc,     // .reloc
	Debug,         // .rdata / .debug
	Architecture,  // .rdata
	GlobalPtr,     // .data
	TLS,           // .tls / .data
	LoadConfig,    // .rdata / .data
	BoundImport,   // .rdata
	IAT,           // .idata
	DelayImport,   // .didat
	CLR,           // .clr / .text
	Reserved,      // -
};

class CMemoryModule : public CMemoryBlock
{
private:
	CMemoryModule(const CMemoryModule &) = delete;
	CMemoryModule &operator=(const CMemoryModule &) = delete;

	inline HMODULE GetHandle() const { return reinterpret_cast<HMODULE>(const_cast<void *>(_address)); }
	std::pair<void *, size_t> GetSectionInfo(eSection section);

public:
	CMemoryModule() = default;
	CMemoryModule(const char *libname, size_t size);
	CMemoryModule(HMODULE handle, size_t size);

	bool IsLoaded() const;

	std::unique_ptr<CMemoryBlock> GetSection(eSection section);
	std::unique_ptr<CMemoryBlock> GetEntrySection();

	//
	// Signaturing
	//

	bool SigSec(eSection directory, const CSignature &signature, SigCallbackFn cb, void *lpParam);
	bool SigSec(eSection directory, const char *signature, SigCallbackFn cb, void *lpParam);
	bool SigSec(eSection directory, SigCallbackFn cb, void *lpParam);

	//
	// Static builders
	//

	static std::unique_ptr<CMemoryModule> CreateFromExecutable();
	static std::unique_ptr<CMemoryModule> CreateFromLibrary(const char *libname, size_t size = 0);
	static std::unique_ptr<CMemoryModule> CreateFromHandle(HMODULE handle, size_t size = 0);
	static std::unique_ptr<CMemoryModule> CreateFromAddress(const void *address, size_t size = 0);
	static std::unique_ptr<CMemoryModule> CreateFromAddress(std::nullptr_t);
};

MEMORIA_END