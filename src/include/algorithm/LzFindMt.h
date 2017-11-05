/* LzFindMt.h -- multithreaded Match finder for LZ algorithms
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __LZ_FIND_MT_H
#define __LZ_FIND_MT_H

#include "algorithm/Threads.h"
#include "algorithm/LzFind.h"
#ifdef __cplusplus
extern "C" {
#endif

#define kMtHashBlockSize (1 << 13)
#define kMtHashNumBlocks (1 << 3)
#define kMtHashNumBlocksMask (kMtHashNumBlocks - 1)

#define kMtBtBlockSize (1 << 14)
#define kMtBtNumBlocks (1 << 6)
#define kMtBtNumBlocksMask (kMtBtNumBlocks - 1)

typedef struct _CMtSync
{
  Bool wasCreated;
  Bool needStart;
  Bool exit;
  Bool stopWriting;

  CThread thread;
  CAutoResetEvent canStart;
  CAutoResetEvent wasStarted;
  CAutoResetEvent wasStopped;
  CSemaphore freeSemaphore;
  CSemaphore filledSemaphore;
  Bool csWasInitialized;
  Bool csWasEntered;
  CCriticalSection cs;
  uint32_t numProcessedBlocks;
} CMtSync;

typedef uint32_t * (*Mf_Mix_Matches)(void *p, uint32_t matchMinPos, uint32_t *distances);

/* kMtCacheLineDummy must be >= size_of_CPU_cache_line */
#define kMtCacheLineDummy 128

typedef void (*Mf_GetHeads)(const byte_t *buffer, uint32_t pos,
  uint32_t *hash, uint32_t hashMask, uint32_t *heads, uint32_t numHeads, const uint32_t *crc);

typedef struct _CMatchFinderMt
{
  /* LZ */
  const byte_t *pointerToCurPos;
  uint32_t *btBuf;
  uint32_t btBufPos;
  uint32_t btBufPosLimit;
  uint32_t lzPos;
  uint32_t btNumAvailBytes;

  uint32_t *hash;
  uint32_t fixedHashSize;
  uint32_t historySize;
  const uint32_t *crc;

  Mf_Mix_Matches MixMatchesFunc;
  
  /* LZ + BT */
  CMtSync btSync;
  byte_t btDummy[kMtCacheLineDummy];

  /* BT */
  uint32_t *hashBuf;
  uint32_t hashBufPos;
  uint32_t hashBufPosLimit;
  uint32_t hashNumAvail;

  CLzRef *son;
  uint32_t matchMaxLen;
  uint32_t numHashBytes;
  uint32_t pos;
  byte_t *buffer;
  uint32_t cyclicBufferPos;
  uint32_t cyclicBufferSize; /* it must be historySize + 1 */
  uint32_t cutValue;

  /* BT + Hash */
  CMtSync hashSync;
  /* byte_t hashDummy[kMtCacheLineDummy]; */
  
  /* Hash */
  Mf_GetHeads GetHeadsFunc;
  CMatchFinder *MatchFinder;
} CMatchFinderMt;

void MatchFinderMt_Construct(CMatchFinderMt *p);
void MatchFinderMt_Destruct(CMatchFinderMt *p, ISzAlloc *alloc);
result_t MatchFinderMt_Create(CMatchFinderMt *p, uint32_t historySize, uint32_t keepAddBufferBefore,
    uint32_t matchMaxLen, uint32_t keepAddBufferAfter, ISzAlloc *alloc);
void MatchFinderMt_CreateVTable(CMatchFinderMt *p, IMatchFinder *vTable);
void MatchFinderMt_ReleaseStream(CMatchFinderMt *p);

#ifdef __cplusplus
}
#endif
#endif
