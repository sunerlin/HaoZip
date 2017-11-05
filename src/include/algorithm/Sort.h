/* Sort.h -- Sort functions
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __7Z_SORT_H
#define __7Z_SORT_H

#include "algorithm/Types.h"

#ifdef __cplusplus
extern "C" {
#endif
void HeapSort(uint32_t *p, uint32_t size);
/* void HeapSortRef(uint32_t *p, uint32_t *vals, uint32_t size); */

#ifdef __cplusplus
}
#endif
#endif
