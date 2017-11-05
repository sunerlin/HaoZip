/* BwtSort.c -- BWT block sorting
2008-08-17
Igor Pavlov
Public domain */

#include "algorithm/BwtSort.h"
#include "algorithm/Sort.h"

/* #define BLOCK_SORT_USE_HEAP_SORT */

#define NO_INLINE MY_FAST_CALL

/* Don't change it !!! */
#define kNumHashBytes 2
#define kNumHashValues (1 << (kNumHashBytes * 8))

/* kNumRefBitsMax must be < (kNumHashBytes * 8) = 16 */
#define kNumRefBitsMax 12

#define BS_TEMP_SIZE kNumHashValues

#ifdef BLOCK_SORT_EXTERNAL_FLAGS

/* 32 Flags in uint32_t word */
#define kNumFlagsBits 5
#define kNumFlagsInWord (1 << kNumFlagsBits)
#define kFlagsMask (kNumFlagsInWord - 1)
#define kAllFlags 0xFFFFFFFF

#else

#define kNumBitsMax 20
#define kIndexMask ((1 << kNumBitsMax) - 1)
#define kNumExtraBits (32 - kNumBitsMax)
#define kNumExtra0Bits (kNumExtraBits - 2)
#define kNumExtra0Mask ((1 << kNumExtra0Bits) - 1)

#define SetFinishedGroupSize(p, size) \
  {  *(p) |= ((((size) - 1) & kNumExtra0Mask) << kNumBitsMax); \
    if ((size) > (1 << kNumExtra0Bits)) { \
    *(p) |= 0x40000000;  *((p) + 1) |= ((((size) - 1)>> kNumExtra0Bits) << kNumBitsMax); } } \

static void SetGroupSize(uint32_t *p, uint32_t size)
{
  if (--size == 0)
    return;
  *p |= 0x80000000 | ((size & kNumExtra0Mask) << kNumBitsMax);
  if (size >= (1 << kNumExtra0Bits))
  {
    *p |= 0x40000000;
    p[1] |= ((size >> kNumExtra0Bits) << kNumBitsMax);
  }
}

#endif

/*
SortGroup - is recursive Range-Sort function with HeapSort optimization for small blocks
  "range" is not real range. It's only for optimization.
returns: 1 - if there are groups, 0 - no more groups
*/

uint32_t NO_INLINE SortGroup(uint32_t BlockSize, uint32_t NumSortedBytes, uint32_t groupOffset, uint32_t groupSize, int NumRefBits, uint32_t *Indices
  #ifndef BLOCK_SORT_USE_HEAP_SORT
  , uint32_t left, uint32_t range
  #endif
  )
{
  uint32_t *ind2 = Indices + groupOffset;
  uint32_t *Groups;
  if (groupSize <= 1)
  {
    /*
    #ifndef BLOCK_SORT_EXTERNAL_FLAGS
    SetFinishedGroupSize(ind2, 1);
    #endif
    */
    return 0;
  }
  Groups = Indices + BlockSize + BS_TEMP_SIZE;
  if (groupSize <= ((uint32_t)1 << NumRefBits)
      #ifndef BLOCK_SORT_USE_HEAP_SORT
      && groupSize <= range
      #endif
      )
  {
    uint32_t *temp = Indices + BlockSize;
    uint32_t j;
    uint32_t mask, thereAreGroups, group, cg;
    {
      uint32_t gPrev;
      uint32_t gRes = 0;
      {
        uint32_t sp = ind2[0] + NumSortedBytes;
        if (sp >= BlockSize) sp -= BlockSize;
        gPrev = Groups[sp];
        temp[0] = (gPrev << NumRefBits);
      }
      
      for (j = 1; j < groupSize; j++)
      {
        uint32_t sp = ind2[j] + NumSortedBytes;
        uint32_t g;
        if (sp >= BlockSize) sp -= BlockSize;
        g = Groups[sp];
        temp[j] = (g << NumRefBits) | j;
        gRes |= (gPrev ^ g);
      }
      if (gRes == 0)
      {
        #ifndef BLOCK_SORT_EXTERNAL_FLAGS
        SetGroupSize(ind2, groupSize);
        #endif
        return 1;
      }
    }
    
    HeapSort(temp, groupSize);
    mask = ((1 << NumRefBits) - 1);
    thereAreGroups = 0;
    
    group = groupOffset;
    cg = (temp[0] >> NumRefBits);
    temp[0] = ind2[temp[0] & mask];

    {
    #ifdef BLOCK_SORT_EXTERNAL_FLAGS
    uint32_t *Flags = Groups + BlockSize;
    #else
    uint32_t prevGroupStart = 0;
    #endif
    
    for (j = 1; j < groupSize; j++)
    {
      uint32_t val = temp[j];
      uint32_t cgCur = (val >> NumRefBits);
      
      if (cgCur != cg)
      {
        cg = cgCur;
        group = groupOffset + j;

        #ifdef BLOCK_SORT_EXTERNAL_FLAGS
        {
        uint32_t t = group - 1;
        Flags[t >> kNumFlagsBits] &= ~(1 << (t & kFlagsMask));
        }
        #else
        SetGroupSize(temp + prevGroupStart, j - prevGroupStart);
        prevGroupStart = j;
        #endif
      }
      else
        thereAreGroups = 1;
      {
      uint32_t ind = ind2[val & mask];
      temp[j] = ind;
      Groups[ind] = group;
      }
    }

    #ifndef BLOCK_SORT_EXTERNAL_FLAGS
    SetGroupSize(temp + prevGroupStart, j - prevGroupStart);
    #endif
    }

    for (j = 0; j < groupSize; j++)
      ind2[j] = temp[j];
    return thereAreGroups;
  }

  /* Check that all strings are in one group (cannot sort) */
  {
    uint32_t group, j;
    uint32_t sp = ind2[0] + NumSortedBytes; if (sp >= BlockSize) sp -= BlockSize;
    group = Groups[sp];
    for (j = 1; j < groupSize; j++)
    {
      sp = ind2[j] + NumSortedBytes; if (sp >= BlockSize) sp -= BlockSize;
      if (Groups[sp] != group)
        break;
    }
    if (j == groupSize)
    {
      #ifndef BLOCK_SORT_EXTERNAL_FLAGS
      SetGroupSize(ind2, groupSize);
      #endif
      return 1;
    }
  }

  #ifndef BLOCK_SORT_USE_HEAP_SORT
  {
  /* ---------- Range Sort ---------- */
  uint32_t i;
  uint32_t mid;
  for (;;)
  {
    uint32_t j;
    if (range <= 1)
    {
      #ifndef BLOCK_SORT_EXTERNAL_FLAGS
      SetGroupSize(ind2, groupSize);
      #endif
      return 1;
    }
    mid = left + ((range + 1) >> 1);
    j = groupSize;
    i = 0;
    do
    {
      uint32_t sp = ind2[i] + NumSortedBytes; if (sp >= BlockSize) sp -= BlockSize;
      if (Groups[sp] >= mid)
      {
        for (j--; j > i; j--)
        {
          sp = ind2[j] + NumSortedBytes; if (sp >= BlockSize) sp -= BlockSize;
          if (Groups[sp] < mid)
          {
            uint32_t temp = ind2[i]; ind2[i] = ind2[j]; ind2[j] = temp;
            break;
          }
        }
        if (i >= j)
          break;
      }
    }
    while (++i < j);
    if (i == 0)
    {
      range = range - (mid - left);
      left = mid;
    }
    else if (i == groupSize)
      range = (mid - left);
    else
      break;
  }

  #ifdef BLOCK_SORT_EXTERNAL_FLAGS
  {
    uint32_t t = (groupOffset + i - 1);
    uint32_t *Flags = Groups + BlockSize;
    Flags[t >> kNumFlagsBits] &= ~(1 << (t & kFlagsMask));
  }
  #endif

  {
    uint32_t j;
    for (j = i; j < groupSize; j++)
      Groups[ind2[j]] = groupOffset + i;
  }

  {
  uint32_t res = SortGroup(BlockSize, NumSortedBytes, groupOffset, i, NumRefBits, Indices, left, mid - left);
  return res | SortGroup(BlockSize, NumSortedBytes, groupOffset + i, groupSize - i, NumRefBits, Indices, mid, range - (mid - left));
  }

  }

  #else

  /* ---------- Heap Sort ---------- */

  {
    uint32_t j;
    for (j = 0; j < groupSize; j++)
    {
      uint32_t sp = ind2[j] + NumSortedBytes; if (sp >= BlockSize) sp -= BlockSize;
      ind2[j] = sp;
    }

    HeapSortRef(ind2, Groups, groupSize);

    /* Write Flags */
    {
    uint32_t sp = ind2[0];
    uint32_t group = Groups[sp];

    #ifdef BLOCK_SORT_EXTERNAL_FLAGS
    uint32_t *Flags = Groups + BlockSize;
    #else
    uint32_t prevGroupStart = 0;
    #endif

    for (j = 1; j < groupSize; j++)
    {
      sp = ind2[j];
      if (Groups[sp] != group)
      {
        group = Groups[sp];
        #ifdef BLOCK_SORT_EXTERNAL_FLAGS
        {
        uint32_t t = groupOffset + j - 1;
        Flags[t >> kNumFlagsBits] &= ~(1 << (t & kFlagsMask));
        }
        #else
        SetGroupSize(ind2 + prevGroupStart, j - prevGroupStart);
        prevGroupStart = j;
        #endif
      }
    }

    #ifndef BLOCK_SORT_EXTERNAL_FLAGS
    SetGroupSize(ind2 + prevGroupStart, j - prevGroupStart);
    #endif
    }
    {
    /* Write new Groups values and Check that there are groups */
    uint32_t thereAreGroups = 0;
    for (j = 0; j < groupSize; j++)
    {
      uint32_t group = groupOffset + j;
      #ifndef BLOCK_SORT_EXTERNAL_FLAGS
      uint32_t subGroupSize = ((ind2[j] & ~0xC0000000) >> kNumBitsMax);
      if ((ind2[j] & 0x40000000) != 0)
        subGroupSize += ((ind2[j + 1] >> kNumBitsMax) << kNumExtra0Bits);
      subGroupSize++;
      for (;;)
      {
        uint32_t original = ind2[j];
        uint32_t sp = original & kIndexMask;
        if (sp < NumSortedBytes) sp += BlockSize; sp -= NumSortedBytes;
        ind2[j] = sp | (original & ~kIndexMask);
        Groups[sp] = group;
        if (--subGroupSize == 0)
          break;
        j++;
        thereAreGroups = 1;
      }
      #else
      uint32_t *Flags = Groups + BlockSize;
      for (;;)
      {
        uint32_t sp = ind2[j]; if (sp < NumSortedBytes) sp += BlockSize; sp -= NumSortedBytes;
        ind2[j] = sp;
        Groups[sp] = group;
        if ((Flags[(groupOffset + j) >> kNumFlagsBits] & (1 << ((groupOffset + j) & kFlagsMask))) == 0)
          break;
        j++;
        thereAreGroups = 1;
      }
      #endif
    }
    return thereAreGroups;
    }
  }
  #endif
}

/* conditions: blockSize > 0 */
uint32_t BlockSort(uint32_t *Indices, const byte_t *data, uint32_t blockSize)
{
  uint32_t *counters = Indices + blockSize;
  uint32_t i;
  uint32_t *Groups;
  #ifdef BLOCK_SORT_EXTERNAL_FLAGS
  uint32_t *Flags;
  #endif

  /* Radix-Sort for 2 bytes */
  for (i = 0; i < kNumHashValues; i++)
    counters[i] = 0;
  for (i = 0; i < blockSize - 1; i++)
    counters[((uint32_t)data[i] << 8) | data[i + 1]]++;
  counters[((uint32_t)data[i] << 8) | data[0]]++;

  Groups = counters + BS_TEMP_SIZE;
  #ifdef BLOCK_SORT_EXTERNAL_FLAGS
  Flags = Groups + blockSize;
    {
      uint32_t numWords = (blockSize + kFlagsMask) >> kNumFlagsBits;
      for (i = 0; i < numWords; i++)
        Flags[i] = kAllFlags;
    }
  #endif

  {
    uint32_t sum = 0;
    for (i = 0; i < kNumHashValues; i++)
    {
      uint32_t groupSize = counters[i];
      if (groupSize > 0)
      {
        #ifdef BLOCK_SORT_EXTERNAL_FLAGS
        uint32_t t = sum + groupSize - 1;
        Flags[t >> kNumFlagsBits] &= ~(1 << (t & kFlagsMask));
        #endif
        sum += groupSize;
      }
      counters[i] = sum - groupSize;
    }

    for (i = 0; i < blockSize - 1; i++)
      Groups[i] = counters[((uint32_t)data[i] << 8) | data[i + 1]];
    Groups[i] = counters[((uint32_t)data[i] << 8) | data[0]];

    for (i = 0; i < blockSize - 1; i++)
      Indices[counters[((uint32_t)data[i] << 8) | data[i + 1]]++] = i;
    Indices[counters[((uint32_t)data[i] << 8) | data[0]]++] = i;
    
    #ifndef BLOCK_SORT_EXTERNAL_FLAGS
    {
    uint32_t prev = 0;
    for (i = 0; i < kNumHashValues; i++)
    {
      uint32_t prevGroupSize = counters[i] - prev;
      if (prevGroupSize == 0)
        continue;
      SetGroupSize(Indices + prev, prevGroupSize);
      prev = counters[i];
    }
    }
    #endif
  }

  {
  int NumRefBits;
  uint32_t NumSortedBytes;
  for (NumRefBits = 0; ((blockSize - 1) >> NumRefBits) != 0; NumRefBits++);
  NumRefBits = 32 - NumRefBits;
  if (NumRefBits > kNumRefBitsMax)
    NumRefBits = kNumRefBitsMax;

  for (NumSortedBytes = kNumHashBytes; ; NumSortedBytes <<= 1)
  {
    #ifndef BLOCK_SORT_EXTERNAL_FLAGS
    uint32_t finishedGroupSize = 0;
    #endif
    uint32_t newLimit = 0;
    for (i = 0; i < blockSize;)
    {
      uint32_t groupSize;
      #ifdef BLOCK_SORT_EXTERNAL_FLAGS

      if ((Flags[i >> kNumFlagsBits] & (1 << (i & kFlagsMask))) == 0)
      {
        i++;
        continue;
      }
      for (groupSize = 1;
        (Flags[(i + groupSize) >> kNumFlagsBits] & (1 << ((i + groupSize) & kFlagsMask))) != 0;
        groupSize++);
      
      groupSize++;

      #else

      groupSize = ((Indices[i] & ~0xC0000000) >> kNumBitsMax);
      {
      Bool finishedGroup = ((Indices[i] & 0x80000000) == 0);
      if ((Indices[i] & 0x40000000) != 0)
      {
        groupSize += ((Indices[i + 1] >> kNumBitsMax) << kNumExtra0Bits);
        Indices[i + 1] &= kIndexMask;
      }
      Indices[i] &= kIndexMask;
      groupSize++;
      if (finishedGroup || groupSize == 1)
      {
        Indices[i - finishedGroupSize] &= kIndexMask;
        if (finishedGroupSize > 1)
          Indices[i - finishedGroupSize + 1] &= kIndexMask;
        {
        uint32_t newGroupSize = groupSize + finishedGroupSize;
        SetFinishedGroupSize(Indices + i - finishedGroupSize, newGroupSize);
        finishedGroupSize = newGroupSize;
        }
        i += groupSize;
        continue;
      }
      finishedGroupSize = 0;
      }

      #endif
      
      if (NumSortedBytes >= blockSize)
      {
        uint32_t j;
        for (j = 0; j < groupSize; j++)
        {
          uint32_t t = (i + j);
          /* Flags[t >> kNumFlagsBits] &= ~(1 << (t & kFlagsMask)); */
          Groups[Indices[t]] = t;
        }
      }
      else
        if (SortGroup(blockSize, NumSortedBytes, i, groupSize, NumRefBits, Indices
          #ifndef BLOCK_SORT_USE_HEAP_SORT
          , 0, blockSize
          #endif
          ) != 0)
          newLimit = i + groupSize;
      i += groupSize;
    }
    if (newLimit == 0)
      break;
  }
  }
  #ifndef BLOCK_SORT_EXTERNAL_FLAGS
  for (i = 0; i < blockSize;)
  {
    uint32_t groupSize = ((Indices[i] & ~0xC0000000) >> kNumBitsMax);
    if ((Indices[i] & 0x40000000) != 0)
    {
      groupSize += ((Indices[i + 1] >> kNumBitsMax) << kNumExtra0Bits);
      Indices[i + 1] &= kIndexMask;
    }
    Indices[i] &= kIndexMask;
    groupSize++;
    i += groupSize;
  }
  #endif
  return Groups[0];
}

