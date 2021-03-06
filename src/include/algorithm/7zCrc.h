/* 7zCrc.h -- CRC32 calculation
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __7Z_CRC_H
#define __7Z_CRC_H

#include <stddef.h>

#include "algorithm/Types.h"

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t g_CrcTable[];

void MY_FAST_CALL CrcGenerateTable(void);

#define CRC_INIT_VAL 0xFFFFFFFF
#define CRC_GET_DIGEST(crc) ((crc) ^ 0xFFFFFFFF)
#define CRC_UPDATE_BYTE(crc, b) (g_CrcTable[((crc) ^ (b)) & 0xFF] ^ ((crc) >> 8))

uint32_t MY_FAST_CALL CrcUpdate(uint32_t crc, const void *data, size_t size);
uint32_t MY_FAST_CALL CrcCalc(const void *data, size_t size);

#ifdef __cplusplus
}
#endif
#endif
