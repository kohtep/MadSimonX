#pragma once

#include "memoria_common.hpp"
#include "memoria_core_signature.hpp"
#include "memoria_core_hash.hpp"

#include "memoria_utils_optional.hpp"

#include <stdint.h>
#include <vcruntime.h>

MEMORIA_BEGIN

class CSigHandle
{
private:
	void **_output;

	const void *_mem_begin;
	const void *_mem_end;

#ifdef _DEBUG
	void *_output_debug;
#endif

	// when constructor 'output' is nullptr then '_internal_data' will be used as output
	void *_internal_data;

private:
	void SetPointerResult(const void *value, bool deref);

public:
	CSigHandle() = delete;
	CSigHandle(const void *mem_begin, const void *mem_end, void *output = nullptr);
	~CSigHandle();

	//
	// Peekers
	//

	Memoria::Optional<uint8_t>  PeekU8(ptrdiff_t offset = 0);
	Memoria::Optional<uint16_t> PeekU16(ptrdiff_t offset = 0);
	Memoria::Optional<uint32_t> PeekU24(ptrdiff_t offset = 0);
	Memoria::Optional<uint32_t> PeekU32(ptrdiff_t offset = 0);
	Memoria::Optional<uint64_t> PeekU64(ptrdiff_t offset = 0);

	Memoria::Optional<int8_t>  PeekI8(ptrdiff_t offset = 0);
	Memoria::Optional<int16_t> PeekI16(ptrdiff_t offset = 0);
	Memoria::Optional<int32_t> PeekI24(ptrdiff_t offset = 0);
	Memoria::Optional<int32_t> PeekI32(ptrdiff_t offset = 0);
	Memoria::Optional<int64_t> PeekI64(ptrdiff_t offset = 0);

	Memoria::Optional<float> PeekFloat(ptrdiff_t offset = 0);
	Memoria::Optional<double> PeekDouble(ptrdiff_t offset = 0);

	bool PeekAStr(char *out, size_t max_size, ptrdiff_t offset = 0);
	bool PeekWStr(wchar_t *out, size_t max_size, ptrdiff_t offset = 0);

	//
	// Search
	//

	CSigHandle &FindU8(uint8_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindU16(uint16_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindU24(uint8_t value[3], bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindU24(uint32_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindU32(uint32_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindU64(uint64_t value, bool backward = false, ptrdiff_t offset = 0);

	CSigHandle &FindI8(int8_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindI16(int16_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindI24(int8_t value[3], bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindI24(int32_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindI32(int32_t value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindI64(int64_t value, bool backward = false, ptrdiff_t offset = 0);

	CSigHandle &FindFloat(float value, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindDouble(double value, bool backward = false, ptrdiff_t offset = 0);

	CSigHandle &FindBlock(const void *data, size_t size, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindSignature(const CSignature &sig, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindSignature(const char *sig, bool backward = false, ptrdiff_t offset = 0);

	CSigHandle &FindReference(const void *data, uint16_t opcode, bool search_absolute, bool search_relative,
		bool backward = false, ptrdiff_t pre_offset = sizeof(int32_t), ptrdiff_t offset = 0);

	CSigHandle &FindAStr(const char *data, bool backward = false, ptrdiff_t offset = 0);
	CSigHandle &FindWStr(const wchar_t *data, bool backward = false, ptrdiff_t offset = 0);

	CSigHandle &FindRelative(uint16_t opcode = 0, size_t index = 0, bool backward = false, ptrdiff_t offset = 0);

	CSigHandle &Deref();

	CSigHandle &Rip();
	CSigHandle &Rip(ptrdiff_t offset);
	CSigHandle &Rip(ptrdiff_t pre_offset, ptrdiff_t post_offset);

	CSigHandle &PtrOffset(ptrdiff_t value);
	CSigHandle &Add(size_t value);
	CSigHandle &Sub(size_t value);

	CSigHandle &Align(size_t nValue = 0x10);

	//
	// Check
	//

	bool CheckU8(uint8_t value, ptrdiff_t offset = 0) const;
	bool CheckU16(uint16_t value, ptrdiff_t offset = 0) const;
	bool CheckU24(uint32_t value, ptrdiff_t offset = 0) const;
	bool CheckU24(uint8_t value[3], ptrdiff_t offset = 0) const;
	bool CheckU32(uint32_t value, ptrdiff_t offset = 0) const;
	bool CheckU64(uint64_t value, ptrdiff_t offset = 0) const;

	bool CheckI8(int8_t value, ptrdiff_t offset = 0) const;
	bool CheckI16(int16_t value, ptrdiff_t offset = 0) const;
	bool CheckI24(int32_t value, ptrdiff_t offset = 0) const;
	bool CheckI24(int8_t value[3], ptrdiff_t offset = 0) const;
	bool CheckI32(int32_t value, ptrdiff_t offset = 0) const;
	bool CheckI64(int64_t value, ptrdiff_t offset = 0) const;

	bool CheckFloat(float value, ptrdiff_t offset = 0) const;
	bool CheckDouble(double value, ptrdiff_t offset = 0) const;

	bool CheckAStr(const char *value, ptrdiff_t offset = 0) const;
	bool CheckWStr(const wchar_t *value, ptrdiff_t offset = 0) const;

	bool CheckSignature(const CSignature &value, ptrdiff_t offset = 0) const;
	bool CheckSignature(const char *value, ptrdiff_t offset = 0) const;

	//
	// Utility
	//

	CSigHandle Clone() const { return *this; }

	ptrdiff_t GetOffset() const;

	void Reset();
	void Invalidate();
	void ForceOutput(const void *value);
	void *CurrentOutput() const;

	bool IsValid() const;

	void *GetPointer() const { return (_output ? *_output : nullptr); }

	template <typename T> T As() { return reinterpret_cast<T>(GetPointer()); }
};

using SigCallbackFn = void (*)(CSigHandle &, void *lpParam);

//template <size_t MaxItems>
class CSignatureMgrImpl
{
protected:
	const static auto MaxItems = 64;

	enum class SignalCode
	{
		Invalid = 0,

		SameAsBase,
		Failed,
		Success,
	};

protected:
	struct SigMeta_t
	{
		fnv1a_t Tag = 0;
		const char *Name = nullptr;

		SigMeta_t() = default;
		SigMeta_t(fnv1a_t hash, const char *name)
			: Tag(hash), Name(name) {}
	};

	struct SigCmd_t
	{
		fnv1a_t Tag = 0;
		void **Pointer = nullptr;
		SigCallbackFn Callback = nullptr;

		SigCmd_t() = default;
		SigCmd_t(fnv1a_t hash, void *result, SigCallbackFn callback)
			: Tag(hash), Pointer(reinterpret_cast<void **>(result)), Callback(callback) {}
	};

	static bool SigMetaPredFn(const SigMeta_t &value, void *context)
	{
		return (value.Tag == reinterpret_cast<SigCmd_t *>(context)->Tag);
	}

	Memoria::FixedVector<SigMeta_t, MaxItems> _dictionary;
	Memoria::FixedVector<SigCmd_t, MaxItems> _sigs;

public:
	CSignatureMgrImpl() = default;

	virtual void OnSignal(SignalCode code, SigCmd_t *cmd, SigMeta_t *meta) {}

	template <FNV1a64_t... Hashes>
	void AddMeta(const char *name)
	{
		constexpr fnv1a_t combined = (Hashes.Hash ^ ...);
		_dictionary.emplace_back(combined, name);
	}

	template <FNV1a64_t... Hashes>
	void AddSignature(void *result, SigCallbackFn callback)
	{
		constexpr fnv1a_t combined = (Hashes.Hash ^ ...);
		_sigs.emplace_back(combined, result, callback);
	}

	void AddSignature(void *result, SigCallbackFn callback)
	{
		_sigs.emplace_back(0, result, callback);
	}

	void Run(const void *mem_begin, const void *mem_end)
	{
		for (SigCmd_t &sig : _sigs)
		{
			CSigHandle handle(mem_begin, mem_end);
			sig.Callback(handle, nullptr);
			*sig.Pointer = handle.GetPointer();

			SigMeta_t *meta;

			if (sig.Tag != 0 && !_dictionary.empty())
				meta = _dictionary.find_if(SigMetaPredFn, &sig);
			else
				meta = nullptr;

			if (handle.IsValid())
			{
				if (handle.GetPointer() == mem_begin)
				{
					OnSignal(SignalCode::SameAsBase, &sig, meta);
				}
				else
				{
					OnSignal(SignalCode::Success, &sig, meta);
				}
			}
			else
			{
				OnSignal(SignalCode::Failed, &sig, meta);
			}
		}
	}
};

using CSignatureMgr = CSignatureMgrImpl;//<32>;

MEMORIA_END