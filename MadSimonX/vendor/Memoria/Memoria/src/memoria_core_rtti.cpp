#include "memoria_core_rtti.hpp"

#include "memoria_core_options.hpp"
#include "memoria_core_misc.hpp"
#include "memoria_core_errors.hpp"
#include "memoria_core_search.hpp"

MEMORIA_BEGIN

// Structure that represents the RTTI type descriptor
struct RTTITypeDescriptor
{
	// Always points to type_info's VFTable
	void **VFTable;
	// ?
	void *Spare;
	// Class Name
	char Name[1];
};

// Structure for Pointer-to-member displacement info
struct PtrToMember
{
	// VFTable offset (if PMD.PDisp is -1)
	int MDisp;
	// VBTable offset (-1: VFTable is at displacement PMD.MDisp is -1)
	int PDisp;
	// Displacement of the base class VFTable pointer inside the VBTable
	int VDisp;
};

// Structure for the base class descriptor
struct RTTIBaseClassDescriptor
{
	// TypeDescriptor of this base class
	RTTITypeDescriptor *TypeDescriptor;
	// Number of direct bases of this base class
	unsigned long NumContainedBases;
	// Pointer-to-member displacement info
	PtrToMember Where;
	// Flags, usually 0
	unsigned long Attributes;
};

// Structure for the base class array
struct RTTIBaseClassArray
{
	// Array of base class descriptors
	RTTIBaseClassDescriptor *ArrayOfBaseClassDescriptors[1];
};

// Structure for the class hierarchy descriptor
struct RTTIClassHierarchyDescriptor
{
	// Always 0?
	unsigned long Signature;
	// Bit 0 - multiple inheritance; Bit 1 - virtual inheritance
	unsigned long Attributes;
	// Number of base classes. Count includes the class itself
	unsigned long NumBaseClasses;
	// Array of base class descriptors
	RTTIBaseClassArray *BaseClassArray;
};

// Structure for the complete object locator
struct RTTICompleteObjectLocator
{
	// Always 0?
	unsigned long Signature;
	// Offset of VFTable within the class
	unsigned long Offset;
	// Constructor displacement offset
	unsigned long CDOffset;
	// Class Information
	RTTITypeDescriptor *TypeDescriptor;
	// Class Hierarchy information
	RTTIClassHierarchyDescriptor *ClassDescriptor;
};

//static const char *rtti_headers[] =
//{
//	// Class type identifier
//	".?AV",
//
//	// Struct type identifier
//	".?AU",
//
//	// Pointer to element or array
//	".PEA",
//
//	// Pointer to function with specific signature
//	".P6AX",
//
//	// General pointer to function
//	".P6A",
//};

void *GetRTTIDescriptor(const void *addr_start, const void *addr_min, const void *addr_max, const char *rtti_name)
{
	if (IsSafeModeActive())
	{
		if (!IsMemoryValid(addr_start))
		{
			SetError(ME_INVALID_MEMORY);
			return nullptr;
		}

		if (!rtti_name || !*rtti_name)
		{
			SetError(ME_INVALID_ARGUMENT);
			return nullptr;
		}
	}

	constexpr unsigned long CLASS_SIGNATURE = 'VA?.';

	char name[1024];
	size_t name_len;
	bool is_full_name;

	strcpy_s(name, rtti_name);

	if (*(uint32_t *)name == CLASS_SIGNATURE)
	{
		is_full_name = true;
		name_len = 0;
	}
	else
	{
		is_full_name = false;
		strcat_s(name, "@");
		name_len = strlen(name);
	}

	void *p = const_cast<void *>(addr_start);
	RTTITypeDescriptor *typeDescriptor;

	do
	{
		p = FindU32(p, addr_min, addr_max, CLASS_SIGNATURE, false, sizeof(CLASS_SIGNATURE));
		if (!p)
		{
			SetError(ME_NOT_FOUND);
			return nullptr;
		}

		if (IsMemoryExecutable(p)) // ignore code sections that can contain '.?AV' tag
			continue;

		void *p2 = p;
		p2 = PtrRewind(p2, 4);
		p2 = PtrRewind(p2, sizeof(void *) * 2);
		typeDescriptor = static_cast<RTTITypeDescriptor *>(p2);

		if (is_full_name)
		{
			if (_stricmp(typeDescriptor->Name, name) == 0)
				return typeDescriptor;
		}
		else
		{
			if (_strnicmp(&(typeDescriptor->Name[4]), name, name_len) == 0)
				return typeDescriptor;
		}
	} while (true);

	return nullptr;
}

void **GetVTableForDescriptor(const void *addr_start, const void *addr_min, const void *addr_max, const void *rtti_descriptor)
{
	auto refs = FindReferences(addr_start, addr_min, addr_max, rtti_descriptor, 0, true, true, false, false, 4, 0);
	
	for (auto &ref : refs)
	{
		RTTICompleteObjectLocator *l = (RTTICompleteObjectLocator *)((char *)ref.xref - sizeof(unsigned long) * 3);

		if (l->Signature == 0 && l->Offset == 0 && l->CDOffset == 0)
		{
			if (auto vmt = FindReference(addr_start, addr_min, addr_max, l, 0, true, true))
			{
				return (void **)PtrOffset(vmt, sizeof(void *));
			}
		}
	}

	return nullptr;
}

void **GetVTableForClass(const void *addr_start, const void *addr_min, const void *addr_max, const char *rtti_name)
{
	auto desc = GetRTTIDescriptor(addr_start, addr_min, addr_max, rtti_name);
	if (!desc)
		return nullptr;

	return GetVTableForDescriptor(addr_start, addr_min, addr_max, desc);
}

MEMORIA_END