/* BraIA64.c -- converter for IA-64 code
2008-08-05
Copyright (c) 1999-2008 Igor Pavlov
Read Bra.h for license options */

#include "algorithm/Bra.h"

static const byte_t kBranchTable[32] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  4, 4, 6, 6, 0, 0, 7, 7,
  4, 4, 0, 0, 4, 4, 0, 0
};

size_t IA64_Convert(byte_t *data, size_t size, uint32_t ip, int encoding)
{
  size_t i;
  if (size < 16)
    return 0;
  size -= 16;
  for (i = 0; i <= size; i += 16)
  {
    uint32_t instrTemplate = data[i] & 0x1F;
    uint32_t mask = kBranchTable[instrTemplate];
    uint32_t bitPos = 5;
    int slot;
    for (slot = 0; slot < 3; slot++, bitPos += 41)
    {
      uint32_t bytePos, bitRes;
      uint64_t instruction, instNorm;
      int j;
      if (((mask >> slot) & 1) == 0)
        continue;
      bytePos = (bitPos >> 3);
      bitRes = bitPos & 0x7;
      instruction = 0;
      for (j = 0; j < 6; j++)
        instruction += (uint64_t)data[i + j + bytePos] << (8 * j);

      instNorm = instruction >> bitRes;
      if (((instNorm >> 37) & 0xF) == 0x5 && ((instNorm >> 9) & 0x7) == 0)
      {
        uint32_t src = (uint32_t)((instNorm >> 13) & 0xFFFFF);
        uint32_t dest;
        src |= ((uint32_t)(instNorm >> 36) & 1) << 20;
        
        src <<= 4;
        
        if (encoding)
          dest = ip + (uint32_t)i + src;
        else
          dest = src - (ip + (uint32_t)i);
        
        dest >>= 4;
        
        instNorm &= ~((uint64_t)(0x8FFFFF) << 13);
        instNorm |= ((uint64_t)(dest & 0xFFFFF) << 13);
        instNorm |= ((uint64_t)(dest & 0x100000) << (36 - 20));
        
        instruction &= (1 << bitRes) - 1;
        instruction |= (instNorm << bitRes);
        for (j = 0; j < 6; j++)
          data[i + j + bytePos] = (byte_t)(instruction >> (8 * j));
      }
    }
  }
  return i;
}
