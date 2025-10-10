#include "memoria_core_hook.hpp"

#include "memoria_utils_buffer.hpp"
#include "memoria_utils_assert.hpp"

#include "memoria_core_write.hpp"
#include "memoria_core_misc.hpp"

#include "hde32.h"
#include "hde64.h"

#include "memoria_utils_list.hpp"

MEMORIA_BEGIN

static size_t WriteJumpRel32(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU8(0xE9);                          // JMP rel32
	buf.WriteRelative(addr_target, addr_value, 1); // rel32

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WriteCallRel32(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU8(0xE8);                          // CALL rel32
	buf.WriteRelative(addr_target, addr_value, 1); // rel32

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WritePushRet32(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU8(0x68);            // PUSH imm32
	buf.WritePointer(addr_value); // imm32
	buf.WriteU8(0xC3);            // RET

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WriteJumpAbs32(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU8(0xB8);            // MOV EAX, imm32
	buf.WritePointer(addr_value); // imm32
	buf.WriteU16(0xE0FF);         // JMP EAX

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WriteJumpMem32(void *addr_target, const void *addr_value)
{
	AssertMsg(false, "WriteJumpMem is not supported in x32 mode.");
	return 0;
}

static size_t WriteJumpRel64(void *addr_target, const void *addr_value)
{
	if (addr_value && !IsIn32BitRange(addr_target, addr_value))
		return false;

	CIndependentBuffer64 buf;

	buf.WriteU8(0xE9);                          // JMP
	buf.WriteRelative(addr_target, addr_value, 1); // rel32

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WriteCallRel64(void *addr_target, const void *addr_value)
{
	if (addr_value && !IsIn32BitRange(addr_target, addr_value))
		return false;

	CIndependentBuffer64 buf;

	buf.WriteU8(0xE8);                          // CALL
	buf.WriteRelative(addr_target, addr_value, 1); // rel32

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WritePushRet64(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU8(0x48);            // REX.W
	buf.WriteU8(0xB8);            // MOV RAX, IMM64
	buf.WritePointer(addr_value); // IMM64
	buf.WriteU8(0x50);            // PUSH RAX
	buf.WriteU8(0xC3);            // RET

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WriteJumpAbs64(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU8(0x48);            // REX.W
	buf.WriteU8(0xB8);            // MOV RAX, IMM64
	buf.WritePointer(addr_value); // IMM64
	buf.WriteU16(0xE0FF);         // JMP RAX

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

static size_t WriteJumpMem64(void *addr_target, const void *addr_value)
{
	CIndependentBuffer64 buf;

	buf.WriteU16(0x25FF);         // JMP
	buf.WriteU32(0);              // 0
	buf.WritePointer(addr_value); // DQ IMM64

	if (addr_value && !buf.Clone(addr_target, true))
		return 0;

	return buf.GetSize();
}

MEMORIA_END

MEMORIA_BEGIN

size_t CalculateInstructionSize32(const void *addr, ptrdiff_t offset)
{
	hde32s hs;
	return hde32_disasm(reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(addr) + offset), &hs);
}

size_t CalculateInstructionBoundary32(const void *addr, size_t min_size)
{
	size_t size = 0;

	while (true)
	{
		size += CalculateInstructionSize32(addr, size);

		if (size >= min_size)
			return size;
	}
}

bool WriteHook32(void *addr_target, const void *addr_value, eInvokeMethod method)
{
	switch (method)
	{

	case eInvokeMethod::JumpRel:
		return WriteJumpRel32(addr_target, addr_value) != 0;

	case eInvokeMethod::CallRel:
		return WriteCallRel32(addr_target, addr_value) != 0;

	case eInvokeMethod::PushRet:
		return WritePushRet32(addr_target, addr_value) != 0;

	case eInvokeMethod::JumpAbs:
		return WriteJumpAbs32(addr_target, addr_value) != 0;

	case eInvokeMethod::JumpMem:
		return WriteJumpMem32(addr_target, addr_value) != 0;

	default:
		return false;

	}
}

size_t CalculateHookSize32(void *addr_target, eInvokeMethod method)
{
	switch (method)
	{

	case eInvokeMethod::JumpRel:
		return WriteJumpRel32(addr_target, nullptr);

	case eInvokeMethod::CallRel:
		return WriteCallRel32(addr_target, nullptr);

	case eInvokeMethod::PushRet:
		return WritePushRet32(addr_target, nullptr);

	case eInvokeMethod::JumpAbs:
		return WriteJumpAbs32(addr_target, nullptr);

	case eInvokeMethod::JumpMem:
		return WriteJumpMem32(addr_target, nullptr);

	default:
		return 0;

	}
}

size_t CalculateInstructionSize64(const void *addr, ptrdiff_t offset)
{
	hde64s hs;
	return hde64_disasm(reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(addr) + offset), &hs);
}

size_t CalculateInstructionBoundary64(const void *addr, size_t min_size)
{
	size_t size = 0;

	while (true)
	{
		size += CalculateInstructionSize64(addr, size);

		if (size >= min_size)
			return size;
	}
}

bool WriteHook64(void *addr_target, const void *addr_value, eInvokeMethod method)
{
	switch (method)
	{

	case eInvokeMethod::JumpRel:
		return WriteJumpRel64(addr_target, addr_value);

	case eInvokeMethod::CallRel:
		return WriteCallRel64(addr_target, addr_value);

	case eInvokeMethod::PushRet:
		return WritePushRet64(addr_target, addr_value);

	case eInvokeMethod::JumpAbs:
		return WriteJumpAbs64(addr_target, addr_value);

	case eInvokeMethod::JumpMem:
		return WriteJumpMem64(addr_target, addr_value);

	default:
		return false;

	}
}

size_t CalculateHookSize64(void *addr_target, eInvokeMethod method)
{
	switch (method)
	{

	case eInvokeMethod::JumpRel:
		return WriteJumpRel64(addr_target, nullptr);

	case eInvokeMethod::CallRel:
		return WriteCallRel64(addr_target, nullptr);

	case eInvokeMethod::PushRet:
		return WritePushRet64(addr_target, nullptr);

	case eInvokeMethod::JumpAbs:
		return WriteJumpAbs64(addr_target, nullptr);

	case eInvokeMethod::JumpMem:
		return WriteJumpMem64(addr_target, nullptr);

	default:
		return 0;

	}
}

MEMORIA_END

MEMORIA_BEGIN

CTrampolineBase::CTrampolineBase(CHookMgr *manager, void *target, const void *hook, bool is_x64, uint8_t size, eInvokeMethod method)
	: _manager(manager)
	, _pointer(target)
	, _hook(hook)
	, _size(size)
	, _method(method)
	, _x64(is_x64)
{

}

CTrampoline::CTrampoline(CHookMgr *manager, void *target, const void *hook, bool is_x64, uint8_t size, eInvokeMethod method)
	: CTrampolineBase(manager, target, hook, is_x64, size, method)
{
#ifdef _DEBUG
	std::memset(_backup, 0x90, sizeof(_backup));
#endif

	CWriteBuffer buf(GetJmpHook(), 5);
	buf.WriteU8(0xE9);
	buf.WriteRelative(_hook, GetJmpHook());

	memcpy(GetOriginal(), target, size);

	auto _backup_new = PtrOffset(GetOriginal(), size);
	auto _target_new = PtrOffset(target, size);

	// funny ternary stuff
	((_x64) ? (WriteHook64) : (WriteHook32))(_backup_new, _target_new, eInvokeMethod::JumpRel);
}

CTrampoline::~CTrampoline()
{

}

bool CTrampoline::IsActive()
{
	return memcmp(_pointer, const_cast<const void *>(GetOriginal()), _size) != 0;
}

bool CTrampoline::Hook()
{
	if (IsActive())
		return false;

	// funny ternary stuff again
	return ((_x64) ? (WriteHook64) : (WriteHook32))(_pointer, _hook, _method);
}

bool CTrampoline::Unhook()
{
	if (!IsActive())
		return false;

	return WriteMemory(_pointer, _backup, _size);
}

CHookMgr::~CHookMgr()
{
	Free(_data);
}

CHookMgr::CHookMgr(const void *addr_nearest, size_t max_hooks)
{
	_max_hooks = max_hooks;
	_hooks = 0;

	_data = AllocFar(addr_nearest, _max_hooks * sizeof(CTrampoline), true, true, true);
}

CTrampoline *CHookMgr::Allocate(void *target, const void *hook, bool is_x64, eInvokeMethod method)
{
	if (_hooks >= _max_hooks)
		return nullptr;

	if (_data == nullptr)
		return nullptr;

	uintptr_t base = reinterpret_cast<uintptr_t>(_data);
	size_t offset = sizeof(CTrampoline) * _hooks;

	CTrampoline *result = reinterpret_cast<CTrampoline *>(base + offset);
	++_hooks;

	size_t size;

	if (is_x64)
	{
		size = CalculateHookSize64(target, method);
		size = CalculateInstructionBoundary64(target, size);
	}
	else
	{
		size = CalculateHookSize32(target, method);
		size = CalculateInstructionBoundary32(target, size);
	}

	std::construct_at(result, this, target, hook, is_x64, static_cast<uint8_t>(size), method);
	return result;
}

bool CHookMgr::IsNear(const void *addr) const
{
	return IsIn32BitRange(_data, addr);
}

static Memoria::List<CHookMgr> gTrampolineMgrs;

static CHookMgr *FindNearestTrampolineMgr(const void *addr)
{
	for (auto &mgr : gTrampolineMgrs)
	{
		if (mgr.IsNear(addr))
			return &mgr;
	}

	gTrampolineMgrs.emplace_back(addr);
	return &gTrampolineMgrs.back();
}

static bool HookInternal(void *target, const void *hook, void *trampoline, bool is_x64, eInvokeMethod method)
{
	auto mgr = FindNearestTrampolineMgr(target);
	if (!mgr)
		return false;

	CTrampoline *tmp = mgr->Allocate(target, hook, is_x64, method);

	if (!tmp->Hook())
		return false;

	if (trampoline)
		*reinterpret_cast<void **>(trampoline) = tmp->GetOriginal();

	return true;
}

bool Hook(void *target, const void *hook, void *trampoline)
{
	return HookInternal(target, hook, trampoline, IsX64(), eInvokeMethod::JumpRel);
}

bool Hook32(void *target, const void *hook, void *trampoline, eInvokeMethod method)
{
	return HookInternal(target, hook, trampoline, false, method);
}

bool Hook64(void *target, const void *hook, void *trampoline, eInvokeMethod method)
{
	return HookInternal(target, hook, trampoline, true, method);
}

MEMORIA_END

MEMORIA_BEGIN

CShadowVTable::CShadowVTable(void *instance)
{
	Assert(instance != nullptr);

	auto _instance = (void ***)instance;
	void **_vmt_org = *_instance;

	size_t methodCount = 0;
	while (_vmt_org[methodCount])
		methodCount++;

	std::construct_at(this, instance, methodCount);
}

CShadowVTable::CShadowVTable(void *instance, size_t methodCount)
{
	Assert(instance != nullptr);
	Assert(methodCount > 0);

	this->_instance = reinterpret_cast<Class *>(instance);
	void **_vmt_org = this->_instance->vtable;

	_vmt_shadow = std::make_unique<void *[]>(methodCount);

	memcpy(_vmt_shadow.get(), _vmt_org, methodCount * sizeof(void *));
	std::construct_at(&_vmt_original, instance);

	this->_instance->vtable = _vmt_shadow.get();
}

CShadowVTable::~CShadowVTable()
{
	_instance->vtable = _vmt_original.value();
}

void CShadowVTable::Hook(size_t index, const void *callback)
{
	Memoria::WritePointer(&(_vmt_shadow.get()[index]), callback);
}

MEMORIA_END