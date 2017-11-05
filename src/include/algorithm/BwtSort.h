/* BwtSort.h -- BWT block sorting
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __BWT_SORT_H
#define __BWT_SORT_H

#include "algorithm/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* use BLOCK_SORT_EXTERNAL_FLAGS if blockSize can be > 1M */
/* #define BLOCK_SORT_EXTERNAL_FLAGS */

#ifdef BLOCK_SORT_EXTERNAL_FLAGS
#define BLOCK_SORT_EXTERNAL_SIZE(blockSize) ((((blockSize) + 31) >> 5))
#else
#define BLOCK_SORT_EXTERNAL_SIZE(blockSize) 0
#endif

#define BLOCK_SORT_BUF_SIZE(blockSize) ((blockSize) * 2 + BLOCK_SORT_EXTERNAL_SIZE(blockSize) + (1 << 16))

uint32_t BlockSort(uint32_t *indices, const byte_t *data, uint32_t blockSize);

#ifdef __cplusplus
}
#endif

#endif
