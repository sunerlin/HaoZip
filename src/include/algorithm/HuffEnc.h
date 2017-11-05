/* HuffEnc.h -- Huffman encoding
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __HUFF_ENC_H
#define __HUFF_ENC_H

#include "algorithm/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Conditions:
  num <= 1024 = 2 ^ NUM_BITS
  Sum(freqs) < 4M = 2 ^ (32 - NUM_BITS)
  maxLen <= 16 = kMaxLen
  Num_Items(p) >= HUFFMAN_TEMP_SIZE(num)
*/
 
void Huffman_Generate(const uint32_t *freqs, uint32_t *p, byte_t *lens, uint32_t num, uint32_t maxLen);

#ifdef __cplusplus
}
#endif

#endif
