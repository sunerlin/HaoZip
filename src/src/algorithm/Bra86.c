/* Bra86.c -- converter for x86 code (BCJ)
2008-08-17
Copyright (c) 1999-2008 Igor Pavlov
Read Bra.h for license options */

#include "algorithm/Bra.h"

#define Test86MSByte(b) ((b) == 0 || (b) == 0xFF)

const byte_t kMaskToAllowedStatus[8] = {1, 1, 1, 0, 1, 0, 0, 0};
const byte_t kMaskToBitNumber[8] = {0, 1, 2, 2, 3, 3, 3, 3};

size_t x86_Convert(byte_t *data, size_t size, uint32_t ip, uint32_t *state, int encoding)
{
  size_t bufferPos = 0, prevPosT;
  uint32_t prevMask = *state & 0x7;
  if (size < 5)
    return 0;
  ip += 5;
  prevPosT = (size_t)0 - 1;

  for (;;)
  {
    byte_t *p = data + bufferPos;
    byte_t *limit = data + size - 4;
    for (; p < limit; p++)
      if ((*p & 0xFE) == 0xE8)
        break;
    bufferPos = (size_t)(p - data);
    if (p >= limit)
      break;
    prevPosT = bufferPos - prevPosT;
    if (prevPosT > 3)
      prevMask = 0;
    else
    {
      prevMask = (prevMask << ((int)prevPosT - 1)) & 0x7;
      if (prevMask != 0)
      {
        byte_t b = p[4 - kMaskToBitNumber[prevMask]];
        if (!kMaskToAllowedStatus[prevMask] || Test86MSByte(b))
        {
          prevPosT = bufferPos;
          prevMask = ((prevMask << 1) & 0x7) | 1;
          bufferPos++;
          continue;
        }
      }
    }
    prevPosT = bufferPos;

    if (Test86MSByte(p[4]))
    {
      uint32_t src = ((uint32_t)p[4] << 24) | ((uint32_t)p[3] << 16) | ((uint32_t)p[2] << 8) | ((uint32_t)p[1]);
      uint32_t dest;
      for (;;)
      {
        byte_t b;
        int index;
        if (encoding)
          dest = (ip + (uint32_t)bufferPos) + src;
        else
          dest = src - (ip + (uint32_t)bufferPos);
        if (prevMask == 0)
          break;
        index = kMaskToBitNumber[prevMask] * 8;
        b = (byte_t)(dest >> (24 - index));
        if (!Test86MSByte(b))
          break;
        src = dest ^ ((1 << (32 - index)) - 1);
      }
      p[4] = (byte_t)(~(((dest >> 24) & 1) - 1));
      p[3] = (byte_t)(dest >> 16);
      p[2] = (byte_t)(dest >> 8);
      p[1] = (byte_t)dest;
      bufferPos += 5;
    }
    else
    {
      prevMask = ((prevMask << 1) & 0x7) | 1;
      bufferPos++;
    }
  }
  prevPosT = bufferPos - prevPosT;
  *state = ((prevPosT > 3) ? 0 : ((prevMask << ((int)prevPosT - 1)) & 0x7));
  return bufferPos;
}
