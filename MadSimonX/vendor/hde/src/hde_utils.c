/*
 * hde_utils.c
 * Copyright (c) 2025, Aleksandr B.
 * All rights reserved.
 *
 */

#include "hde_utils.h"

//
// Custom implementation to eliminate the need for including `string.h`.
//
// ******************************************************************************************
// 
// Important observation: the `dst` variable must have the `volatile` modifier,
// otherwise the compiler will attempt to insert a `memset` by any means necessary,
// even if it is not actually needed.
//
// I encountered the following generated assembly code in the OBJ file inside
// the `hde64_disasm` function:
//
//   .text$mn:000000000000023A 48 8D 3D 67 08 00 00        lea     rdi, __ImageBase
//
// When jumping to `__ImageBase`, I ended up in the `Externs` section, where I saw this:
//
//   UNDEF:0000000000000AA0 00 00 00 00 00 00 00 00     extrn memset:near
//   UNDEF:0000000000000AA8 00                          extrn __ImageBase
//
// I have absolutely no understanding of why this happens. The OBJ file does not contain
// any references to `memset`, yet it is still present. This is definitely related to
// optimization techniques, which the `volatile` modifier disables.
// 
// This issue is relevant for:
// Microsoft (R) C/C++ Optimizing Compiler Version 19.43.34809
//
void *hde_memset(void *_Dst, int _Val, unsigned int _Size)
{
	volatile unsigned char *dst = (unsigned char *)_Dst;
	unsigned char val = (unsigned char)_Val;

	for (unsigned int i = 0; i < _Size; ++i)
		dst[i] = val;

	return _Dst;
}