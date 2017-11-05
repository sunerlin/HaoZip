/* CpuArch.h
2009-08-11 : Igor Pavlov : Public domain */

#ifndef __CPU_ARCH_H
#define __CPU_ARCH_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
LITTLE_ENDIAN_UNALIGN means:
  1) CPU is LITTLE_ENDIAN
  2) it's allowed to make unaligned memory accesses
if LITTLE_ENDIAN_UNALIGN is not defined, it means that we don't know
about these properties of platform.
*/

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
#define MY_CPU_AMD64
#endif

#if defined(MY_CPU_AMD64) || defined(_M_IA64)
#define MY_CPU_64BIT
#endif

#if defined(_M_IX86) || defined(__i386__) || defined(MY_CPU_AMD64)
#define MY_CPU_X86_OR_AMD64
#endif

#if defined(MY_CPU_X86_OR_AMD64)
#define LITTLE_ENDIAN_UNALIGN
#endif

#ifdef LITTLE_ENDIAN_UNALIGN

#define GetUi16(p) (*(const uint16_t *)(p))
#define GetUi32(p) (*(const uint32_t *)(p))
#define GetUi64(p) (*(const uint64_t *)(p))
#define SetUi16(p, d) *(uint16_t *)(p) = (d);
#define SetUi32(p, d) *(uint32_t *)(p) = (d);

#else

#define GetUi16(p) (((const byte_t *)(p))[0] | ((uint16_t)((const byte_t *)(p))[1] << 8))

#define GetUi32(p) ( \
             ((const byte_t *)(p))[0]        | \
    ((uint32_t)((const byte_t *)(p))[1] <<  8) | \
    ((uint32_t)((const byte_t *)(p))[2] << 16) | \
    ((uint32_t)((const byte_t *)(p))[3] << 24))

#define GetUi64(p) (GetUi32(p) | ((uint64_t)GetUi32(((const byte_t *)(p)) + 4) << 32))
#define SetUi16(p, d) { uint32_t _x_ = (d); \
    ((byte_t *)(p))[0] = (byte_t)_x_; \
    ((byte_t *)(p))[1] = (byte_t)(_x_ >> 8); }

#define SetUi32(p, d) { uint32_t _x_ = (d); \
    ((byte_t *)(p))[0] = (byte_t)_x_; \
    ((byte_t *)(p))[1] = (byte_t)(_x_ >> 8); \
    ((byte_t *)(p))[2] = (byte_t)(_x_ >> 16); \
    ((byte_t *)(p))[3] = (byte_t)(_x_ >> 24); }

#endif

#if defined(LITTLE_ENDIAN_UNALIGN) && defined(_WIN64) && (_MSC_VER >= 1300)

#pragma intrinsic(_byteswap_ulong)
#pragma intrinsic(_byteswap_uint64)
#define GetBe32(p) _byteswap_ulong(*(const uint32_t *)(const byte_t *)(p))
#define GetBe64(p) _byteswap_uint64(*(const uint64_t *)(const byte_t *)(p))

#else

#define GetBe32(p) ( \
    ((uint32_t)((const byte_t *)(p))[0] << 24) | \
    ((uint32_t)((const byte_t *)(p))[1] << 16) | \
    ((uint32_t)((const byte_t *)(p))[2] <<  8) | \
             ((const byte_t *)(p))[3] )

#define GetBe64(p) (((uint64_t)GetBe32(p) << 32) | GetBe32(((const byte_t *)(p)) + 4))

#endif

#define GetBe16(p) (((uint16_t)((const byte_t *)(p))[0] << 8) | ((const byte_t *)(p))[1])

#ifdef __cplusplus
}
#endif
#endif
