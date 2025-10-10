#include "memoria_ext_module.hpp"

#include "memoria_core_misc.hpp"
#include "memoria_core_search.hpp"
#include "memoria_core_write.hpp"
#include "memoria_core_windows.hpp"

#include "memoria_ext_patch.hpp"

#include "memoria_utils_assert.hpp"

#include "memoria_utils_secure.hpp"

#ifdef MEMORIA_USE_LAZYIMPORT
	#define GetModuleHandleA    LI_FN(GetModuleHandleA)
#endif

MEMORIA_BEGIN

CMemoryBlock::CMemoryBlock(const void *address, size_t size)
	: _address(address)
	, _size(size)
{

}

const char *CMemoryBlock::GetName() const
{
	return (_name && *_name) ? "" : _name;
}

void *CMemoryBlock::GetBase() const
{
	return const_cast<void *>(_address);
}

size_t CMemoryBlock::GetSize() const
{
	return _size;
}

void *CMemoryBlock::GetLastByte() const
{
	return _size ? PtrOffset(_address, _size - 1) : const_cast<void *>(_address);
}

CMemoryModule::CMemoryModule(const char *libname, size_t size) : CMemoryModule()
{
	if (!libname || !*libname)
		return;

	auto handle = GetModuleHandleA(libname);
	if (!handle)
		return;

	_address = handle;
	_size = size;
}

CMemoryModule::CMemoryModule(HMODULE handle, size_t size) : CMemoryModule()
{
	Assert(handle && size > 0);

	_address = handle;
	_size = size;
}

bool CMemoryModule::IsLoaded() const
{
	return (_address != nullptr);
}

std::unique_ptr<CMemoryBlock> CMemoryBlock::CreateFromAddress(const void *address, size_t size)
{
	return std::make_unique<CMemoryBlock>(address, size);
}

// TODO: calc offset for ref in FindReferences
size_t CMemoryBlock::HookRefAddr(const void *addr_target, const void *addr_hook, uint16_t opcode)
{
	auto refs = FindReferences(GetBase(), GetBase(), GetLastByte(), addr_target, opcode, true, true, false);

	for (auto &ref : refs)
	{
		CPatch *patch;

		if (ref.is_absolute)
			patch = Memoria::PatchPointer(ref.xref, addr_hook);
		else
			patch = Memoria::PatchRelative(ref.xref, addr_hook);

		Assert(patch);
	}

	return refs.size();
}

size_t CMemoryBlock::HookRefCall(const void *addr_target, const void *addr_hook)
{
	return HookRefAddr(addr_target, addr_hook, 0xE8);
}

size_t CMemoryBlock::HookRefJump(const void *addr_target, const void *addr_hook)
{
	return HookRefAddr(addr_target, addr_hook, 0xE9);
}

void CMemoryBlock::Sig(const CSignature &signature, SigCallbackFn cb, void *lpParam)
{
	void *output = nullptr;
	CSigHandle sig(this, &output);

	sig.FindSignature(signature);
	cb(sig, lpParam);
}

void CMemoryBlock::Sig(const char *signature, SigCallbackFn cb, void *lpParam)
{
	CSignature sig(signature);
	return Sig(sig, cb, lpParam);
}

void CMemoryBlock::Sig(SigCallbackFn cb, void *lpParam)
{
	void *output = nullptr;
	CSigHandle sig(this, &output);

	cb(sig, lpParam);
}

std::pair<void *, size_t> CMemoryModule::GetSectionInfo(eSection section)
{
	PIMAGE_SECTION_HEADER pSection;

	switch (section)
	{
	case eSection::Export:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_EXPORT);
		break;
	case eSection::Import:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_IMPORT);
		break;
	case eSection::Resource:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_RESOURCE);
		break;
	case eSection::Exception:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_EXCEPTION);
		break;
	case eSection::Security:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_SECURITY);
		break;
	case eSection::BaseReloc:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_BASERELOC);
		break;
	case eSection::Debug:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_DEBUG);
		break;
	case eSection::Architecture:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_ARCHITECTURE);
		break;
	case eSection::GlobalPtr:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_GLOBALPTR);
		break;
	case eSection::TLS:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_TLS);
		break;
	case eSection::LoadConfig:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
		break;
	case eSection::BoundImport:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
		break;
	case eSection::IAT:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_IAT);
		break;
	case eSection::DelayImport:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
		break;
	case eSection::CLR:
		pSection = GetSectionByIndex(GetHandle(), IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);
		break;
	case eSection::Reserved:
		pSection = GetSectionByIndex(GetHandle(), 15);
		break;
	default:
		pSection = nullptr;
		break;
	}

	if (!pSection)
		return {};

	return std::make_pair(PtrOffset(GetHandle(), pSection->VirtualAddress), pSection->Misc.VirtualSize);
}

std::unique_ptr<CMemoryBlock> CMemoryModule::GetSection(eSection section)
{
	auto [_ptr, _size] = GetSectionInfo(section);
	if (!_ptr)
		return {};

	return std::make_unique<CMemoryBlock>(_ptr, _size);
}

std::unique_ptr<CMemoryBlock> CMemoryModule::GetEntrySection()
{
	auto pSection = Memoria::GetEntrySection(GetHandle());
	if (!pSection)
		return {};

	return std::make_unique<CMemoryBlock>
		(PtrOffset(GetHandle(), pSection->VirtualAddress), pSection->Misc.VirtualSize);
}

bool CMemoryModule::SigSec(eSection directory, const CSignature &signature, SigCallbackFn cb, void *lpParam)
{
	auto [ptr, size] = GetSectionInfo(directory);
	if (!ptr)
		return false;

	CSigHandle sig(ptr, PtrOffset(ptr, size - 1));
	sig.FindSignature(signature);

	cb(sig, lpParam);

	return true;
}

bool CMemoryModule::SigSec(eSection directory, const char *signature, SigCallbackFn cb, void *lpParam)
{
	CSignature sig(signature);
	return SigSec(directory, sig, cb, lpParam);
}

bool CMemoryModule::SigSec(eSection directory, SigCallbackFn cb, void *lpParam)
{
	auto [ptr, size] = GetSectionInfo(directory);
	if (!ptr)
		return false;

	CSigHandle sig(ptr, PtrOffset(ptr, size - 1));
	cb(sig, lpParam);

	return true;
}

std::unique_ptr<CMemoryModule> CMemoryModule::CreateFromExecutable()
{
	return CreateFromHandle(NULL, 0);
}

std::unique_ptr<CMemoryModule> CMemoryModule::CreateFromLibrary(const char *libname, size_t size)
{
	HMODULE handle;

	if (!libname || !*libname)
		handle = GetExeBase();
	else
		handle = GetModuleHandleA(libname);

	if (handle == 0)
		return {};

	if (size == 0)
		size = GetModuleSize(handle);

	return std::make_unique<CMemoryModule>(libname, size);
}

std::unique_ptr<CMemoryModule> CMemoryModule::CreateFromHandle(HMODULE handle, size_t size)
{
	if (handle == 0)
		handle = GetExeBase();

	if (handle == 0)
		return {};

	if (size == 0)
		size = GetModuleSize(handle);

	return std::make_unique<CMemoryModule>(handle, size);
}

std::unique_ptr<CMemoryModule> CMemoryModule::CreateFromAddress(const void *address, size_t size)
{
	return std::make_unique<CMemoryModule>(reinterpret_cast<HMODULE>(const_cast<void *>(address)), size);
}

std::unique_ptr<CMemoryModule> CMemoryModule::CreateFromAddress(std::nullptr_t)
{
	return CMemoryModule::CreateFromHandle(NULL, 0);
}

MEMORIA_END