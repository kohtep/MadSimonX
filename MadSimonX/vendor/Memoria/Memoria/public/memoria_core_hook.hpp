#pragma once

#include "memoria_common.hpp"
#include "memoria_utils_assert.hpp"

#include <stdint.h>
#include <memory> // std::unique_ptr

MEMORIA_BEGIN

class CHookMgr;

enum class eInvokeMethod : ::uint8_t
{
	// jmp rel32
	JumpRel,

	// call rel32
	CallRel,

	// push imm32/imm64
	// ret
	PushRet,

	// mov eax/rax, imm32/imm64
	// jmp eax/rax
	JumpAbs,

	// jmp [rip+0] ; FF 25 00 00 00 00
	// DQ imm64
	//
	// This method is only for x64!
	JumpMem
};

#pragma pack(push, 1)
class CTrampolineBase
{
	friend class CHookMgr;

	CTrampolineBase(const CTrampolineBase &) = delete;
	CTrampolineBase &operator=(const CTrampolineBase &) = delete;

protected:
	// Pointer to hook owner.
	CHookMgr *_manager;

	// Pointer to the original code.
	void *_pointer;

	// Pointer to the hook.
	const void *_hook;

	// Original code size.
	uint8_t _size;

	bool _x64;

	eInvokeMethod _method;

public:
	CTrampolineBase() = delete;

	CTrampolineBase(CHookMgr *manager, void *target, const void *hook, bool is_x64, uint8_t size, eInvokeMethod method);
};

class CTrampoline : public CTrampolineBase
{
	CTrampoline(const CTrampoline &) = delete;
	CTrampoline &operator=(const CTrampoline &) = delete;

private:
	// Original code backup.
	//
	// This field must be guaranteed to be safe for execution, meaning that the object of this class
	// should be placed inside the CHookMgr class and is not intended for external use.
	uint8_t _backup[64 - sizeof(CTrampolineBase)];

public:
	CTrampoline() = delete;
	CTrampoline(CHookMgr *manager, void *target, const void *hook, bool is_x64, uint8_t size, eInvokeMethod method);
	~CTrampoline();

	bool IsActive();

	bool Hook();
	bool Unhook();

	void *GetJmpHook() { return reinterpret_cast<void *>(&_backup[0]); }
	void *GetOriginal() { return reinterpret_cast<void *>(&_backup[5]); }

	__forceinline void operator()()
	{
		using fn_ptr_t = void(*)();
		auto fn = reinterpret_cast<fn_ptr_t>(GetOriginal());
		return fn();
	}
};

static_assert(sizeof(CTrampoline) == 64);

template <typename ret_t = void, typename... args_t>
class CTrampolineEx : public CTrampoline
{
public:
	CTrampolineEx(CHookMgr *manager, void *code, uint8_t size, eInvokeMethod method)
		: CTrampoline(manager, code, size, method)
	{
	}

	__forceinline ret_t operator()(args_t... args)
	{
		using fn_ptr_t = ret_t(*)(args_t...);
		auto fn = reinterpret_cast<fn_ptr_t>(GetOriginal());
		return fn(std::forward<args_t>(args)...);
	}
};

#pragma pack(pop)

class CHookMgr
{
private:
	CHookMgr(const CHookMgr &) = delete;
	CHookMgr &operator=(const CHookMgr &) = delete;

private:
	void *_data = nullptr;

	size_t _hooks = 0;
	size_t _max_hooks = 0;

public:
	CHookMgr() = default;
	CHookMgr(const void *addr_nearest, size_t max_hooks = 64);
	~CHookMgr();

	CTrampoline *Allocate(void *target, const void *hook, bool is_x64, eInvokeMethod method);

	bool IsNear(const void *addr) const;
};

extern size_t CalculateInstructionSize32(const void *addr, ptrdiff_t offset = 0);
extern size_t CalculateInstructionSize64(const void *addr, ptrdiff_t offset = 0);

extern size_t CalculateInstructionBoundary32(const void *addr, size_t min_size);
extern size_t CalculateInstructionBoundary64(const void *addr, size_t min_size);

extern bool WriteHook32(void *addr_target, const void *addr_value, eInvokeMethod method);
extern bool WriteHook64(void *addr_target, const void *addr_value, eInvokeMethod method);

extern size_t CalculateHookSize32(void *addr_target, eInvokeMethod method);
extern size_t CalculateHookSize64(void *addr_target, eInvokeMethod method);

extern bool Hook32(void *target, const void *hook, void *trampoline, eInvokeMethod method = eInvokeMethod::JumpRel);
extern bool Hook64(void *target, const void *hook, void *trampoline, eInvokeMethod method = eInvokeMethod::JumpRel);

extern bool Hook(void *target, const void *hook, void *trampoline = nullptr);

MEMORIA_END

MEMORIA_BEGIN

class CVTable
{
private:
	void **_vtable = {};

public:
	CVTable() = default;
	CVTable(void *instance) : _vtable(*reinterpret_cast<void ***>(instance)) {}

	auto value() const { return _vtable; }

	template <typename Ret = void, typename... Args> Ret Invoke(size_t index, Args... args);
	template <typename Fn, typename... Args> auto InvokeFn(size_t index, Args... args);
};

template <typename Ret, typename... Args>
Ret CVTable::Invoke(size_t index, Args... args)
{
	Assert(index >= 0);

	auto method = reinterpret_cast<void *>(_vtable[index]);
	auto func = reinterpret_cast<Ret(*)(Args...)>(_vtable[index]);

	return func(args...);
}

template <typename Fn, typename... Args>
auto CVTable::InvokeFn(size_t index, Args... args)
{
	Assert(index >= 0);

	return ((Fn)_vtable[index])(args...);
}

class CShadowVTable
{
	struct Class
	{
		void **vtable;
	};

private:
	CShadowVTable() = delete;
	CShadowVTable(const CShadowVTable &) = delete;
	CShadowVTable &operator=(const CShadowVTable &) = delete;

	void Hook(size_t index, const void *callback);

	CVTable _vmt_original = {};

	Class *_instance = {};
	std::unique_ptr<void *[]> _vmt_shadow = {};

public:
	CShadowVTable(void *instance);
	CShadowVTable(void *instance, size_t methodCount);
	~CShadowVTable();

	template <typename T> void Hook(size_t index, const T hook);

	template <typename Ret = void, typename... Args> Ret Invoke(size_t index, Args... args);
	template <typename Fn, typename... Args> auto InvokeFn(size_t index, Args... args);
};

template <typename T>
void CShadowVTable::Hook(size_t index, T hook)
{
	Assert(index >= 0);
	Assert(hook != nullptr);

	Hook(index, (void *)hook);
}

template <typename Ret, typename... Args>
Ret CShadowVTable::Invoke(size_t index, Args... args)
{
	return _vmt_original.Invoke<Ret>(index, std::forward<Args>(args)...);
}

template <typename Fn, typename... Args>
auto CShadowVTable::InvokeFn(size_t index, Args... args)
{
	return _vmt_original.InvokeFn<Fn>(index, std::forward<Args>(args)...);
}

MEMORIA_END