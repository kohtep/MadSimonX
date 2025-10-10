/*
 * hde_common.h
 * Copyright (c) 2025, Aleksandr B.
 * All rights reserved.
 *
 */

#ifndef _HDE_COMMON_H_
#define _HDE_COMMON_H_

typedef signed char        hde_int8_t;
typedef short              hde_int16_t;
typedef int                hde_int32_t;
typedef long long          hde_int64_t;
typedef unsigned char      hde_uint8_t;
typedef unsigned short     hde_uint16_t;
typedef unsigned int       hde_uint32_t;
typedef unsigned long long hde_uint64_t;

#define PREFIX_SEGMENT_CS   0x2e
#define PREFIX_SEGMENT_SS   0x36
#define PREFIX_SEGMENT_DS   0x3e
#define PREFIX_SEGMENT_ES   0x26
#define PREFIX_SEGMENT_FS   0x64
#define PREFIX_SEGMENT_GS   0x65
#define PREFIX_LOCK         0xf0
#define PREFIX_REPNZ        0xf2
#define PREFIX_REPX         0xf3
#define PREFIX_OPERAND_SIZE 0x66
#define PREFIX_ADDRESS_SIZE 0x67

#endif /* _HDE_COMMON_H_ */