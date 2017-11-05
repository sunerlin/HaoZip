/* HuffEnc.c -- functions for Huffman encoding
2008-08-05
Igor Pavlov
Public domain */

#include "algorithm/HuffEnc.h"
#include "algorithm/Sort.h"

#define kMaxLen 16
#define NUM_BITS 10
#define MASK ((1 << NUM_BITS) - 1)

#define NUM_COUNTERS 64

#define HUFFMAN_SPEED_OPT

void Huffman_Generate(const uint32_t *freqs, uint32_t *p, byte_t *lens, uint32_t numSymbols, uint32_t maxLen)
{
  uint32_t num = 0;
  /* if (maxLen > 10) maxLen = 10; */
  {
    uint32_t i;
    
    #ifdef HUFFMAN_SPEED_OPT
    
    uint32_t counters[NUM_COUNTERS];
    for (i = 0; i < NUM_COUNTERS; i++)
      counters[i] = 0;
    for (i = 0; i < numSymbols; i++)
    {
      uint32_t freq = freqs[i];
      counters[(freq < NUM_COUNTERS - 1) ? freq : NUM_COUNTERS - 1]++;
    }
 
    for (i = 1; i < NUM_COUNTERS; i++)
    {
      uint32_t temp = counters[i];
      counters[i] = num;
      num += temp;
    }

    for (i = 0; i < numSymbols; i++)
    {
      uint32_t freq = freqs[i];
      if (freq == 0)
        lens[i] = 0;
      else
        p[counters[((freq < NUM_COUNTERS - 1) ? freq : NUM_COUNTERS - 1)]++] = i | (freq << NUM_BITS);
    }
    counters[0] = 0;
    HeapSort(p + counters[NUM_COUNTERS - 2], counters[NUM_COUNTERS - 1] - counters[NUM_COUNTERS - 2]);
    
    #else

    for (i = 0; i < numSymbols; i++)
    {
      uint32_t freq = freqs[i];
      if (freq == 0)
        lens[i] = 0;
      else
        p[num++] = i | (freq << NUM_BITS);
    }
    HeapSort(p, num);

    #endif
  }

  if (num < 2)
  {
    int minCode = 0;
    int maxCode = 1;
    if (num == 1)
    {
      maxCode = (int)(p[0] & MASK);
      if (maxCode == 0)
        maxCode++;
    }
    p[minCode] = 0;
    p[maxCode] = 1;
    lens[minCode] = lens[maxCode] = 1;
    return;
  }
  
  {
    uint32_t b, e, i;
  
    i = b = e = 0;
    do
    {
      uint32_t n, m, freq;
      n = (i != num && (b == e || (p[i] >> NUM_BITS) <= (p[b] >> NUM_BITS))) ? i++ : b++;
      freq = (p[n] & ~MASK);
      p[n] = (p[n] & MASK) | (e << NUM_BITS);
      m = (i != num && (b == e || (p[i] >> NUM_BITS) <= (p[b] >> NUM_BITS))) ? i++ : b++;
      freq += (p[m] & ~MASK);
      p[m] = (p[m] & MASK) | (e << NUM_BITS);
      p[e] = (p[e] & MASK) | freq;
      e++;
    }
    while (num - e > 1);
    
    {
      uint32_t lenCounters[kMaxLen + 1];
      for (i = 0; i <= kMaxLen; i++)
        lenCounters[i] = 0;
      
      p[--e] &= MASK;
      lenCounters[1] = 2;
      while (e > 0)
      {
        uint32_t len = (p[p[--e] >> NUM_BITS] >> NUM_BITS) + 1;
        p[e] = (p[e] & MASK) | (len << NUM_BITS);
        if (len >= maxLen)
          for (len = maxLen - 1; lenCounters[len] == 0; len--);
        lenCounters[len]--;
        lenCounters[len + 1] += 2;
      }
      
      {
        uint32_t len;
        i = 0;
        for (len = maxLen; len != 0; len--)
        {
          uint32_t num;
          for (num = lenCounters[len]; num != 0; num--)
            lens[p[i++] & MASK] = (byte_t)len;
        }
      }
      
      {
        uint32_t nextCodes[kMaxLen + 1];
        {
          uint32_t code = 0;
          uint32_t len;
          for (len = 1; len <= kMaxLen; len++)
            nextCodes[len] = code = (code + lenCounters[len - 1]) << 1;
        }
        /* if (code + lenCounters[kMaxLen] - 1 != (1 << kMaxLen) - 1) throw 1; */

        {
          uint32_t i;
          for (i = 0; i < numSymbols; i++)
            p[i] = nextCodes[lens[i]]++;
        }
      }
    }
  }
}
