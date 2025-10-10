/*
 * Hacker Disassembler Engine 64
 * Copyright (c) 2008-2009, Vyacheslav Patkov.
 * All rights reserved.
 *
 * hde64.h: C/C++ header file
 *
 */

#ifndef _HDE64_H_
#define _HDE64_H_

#include "hde_common.h"

#define F64_MODRM         0x00000001
#define F64_SIB           0x00000002
#define F64_IMM8          0x00000004
#define F64_IMM16         0x00000008
#define F64_IMM32         0x00000010
#define F64_IMM64         0x00000020
#define F64_DISP8         0x00000040
#define F64_DISP16        0x00000080
#define F64_DISP32        0x00000100
#define F64_RELATIVE      0x00000200
#define F64_ERROR         0x00001000
#define F64_ERROR_OPCODE  0x00002000
#define F64_ERROR_LENGTH  0x00004000
#define F64_ERROR_LOCK    0x00008000
#define F64_ERROR_OPERAND 0x00010000
#define F64_PREFIX_REPNZ  0x01000000
#define F64_PREFIX_REPX   0x02000000
#define F64_PREFIX_REP    0x03000000
#define F64_PREFIX_66     0x04000000
#define F64_PREFIX_67     0x08000000
#define F64_PREFIX_LOCK   0x10000000
#define F64_PREFIX_SEG    0x20000000
#define F64_PREFIX_REX    0x40000000
#define F64_PREFIX_ANY    0x7f000000

#pragma pack(push,1)

typedef struct
{
	hde_uint8_t len;
	hde_uint8_t p_rep;
	hde_uint8_t p_lock;
	hde_uint8_t p_seg;
	hde_uint8_t p_66;
	hde_uint8_t p_67;
	hde_uint8_t rex;
	hde_uint8_t rex_w;
	hde_uint8_t rex_r;
	hde_uint8_t rex_x;
	hde_uint8_t rex_b;
	hde_uint8_t opcode;
	hde_uint8_t opcode2;
	hde_uint8_t modrm;
	hde_uint8_t modrm_mod;
	hde_uint8_t modrm_reg;
	hde_uint8_t modrm_rm;
	hde_uint8_t sib;
	hde_uint8_t sib_scale;
	hde_uint8_t sib_index;
	hde_uint8_t sib_base;

	union
	{
		hde_uint8_t imm8;
		hde_uint16_t imm16;
		hde_uint32_t imm32;
		hde_uint64_t imm64;
	} imm;

	union
	{
		hde_uint8_t disp8;
		hde_uint16_t disp16;
		hde_uint32_t disp32;
	} disp;

	hde_uint32_t flags;
} hde64s;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

/* __cdecl */
unsigned int hde64_disasm(const void *code, hde64s *hs);

#ifdef __cplusplus
}
#endif

#endif /* _HDE64_H_ */
