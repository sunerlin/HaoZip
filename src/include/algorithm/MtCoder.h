/* MtCoder.h -- Multi-thread Coder
2009-03-26 : Igor Pavlov : Public domain */

#ifndef __MT_CODER_H
#define __MT_CODER_H

#include "Threads.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct
    {
        CThread thread;
        CAutoResetEvent startEvent;
        CAutoResetEvent finishedEvent;
        int stop;

        THREAD_FUNC_TYPE func;
        LPVOID param;
        THREAD_FUNC_RET_TYPE res;
    } CLoopThread;

    void LoopThread_Construct(CLoopThread *p);
    void LoopThread_Close(CLoopThread *p);
    WRes LoopThread_Create(CLoopThread *p);
    WRes LoopThread_StopAndWait(CLoopThread *p);
    WRes LoopThread_StartSubThread(CLoopThread *p);
    WRes LoopThread_WaitSubThread(CLoopThread *p);

#ifdef COMPRESS_MT
#define NUM_MT_CODER_THREADS_MAX 32
#else
#define NUM_MT_CODER_THREADS_MAX 1
#endif

    typedef struct
    {
        uint64_t totalInSize;
        uint64_t totalOutSize;
        ICompressProgress *progress;
        result_t res;
        CCriticalSection cs;
        uint64_t inSizes[NUM_MT_CODER_THREADS_MAX];
        uint64_t outSizes[NUM_MT_CODER_THREADS_MAX];
    } CMtProgress;

    result_t MtProgress_Set(CMtProgress *p, unsigned index, uint64_t inSize, uint64_t outSize);

    struct _CMtCoder;

    typedef struct
    {
        struct _CMtCoder *mtCoder;
        byte_t *outBuf;
        size_t outBufSize;
        byte_t *inBuf;
        size_t inBufSize;
        unsigned index;
        CLoopThread thread;

        Bool stopReading;
        Bool stopWriting;
        CAutoResetEvent canRead;
        CAutoResetEvent canWrite;
    } CMtThread;

    typedef struct
    {
        result_t (*Code)(void *p,
                         unsigned index,
                         byte_t *dest,
                         size_t *destSize,
                         const byte_t *src,
                         size_t srcSize,
                         int finished);
    } IMtCoderCallback;

    typedef struct _CMtCoder
    {
        size_t blockSize;
        size_t destBlockSize;
        unsigned numThreads;

        ISeqInStream *inStream;
        ISeqOutStream *outStream;
        ICompressProgress *progress;
        ISzAlloc *alloc;

        IMtCoderCallback *mtCallback;
        CCriticalSection cs;
        result_t res;

        CMtProgress mtProgress;
        CMtThread threads[NUM_MT_CODER_THREADS_MAX];
    } CMtCoder;

    void MtCoder_Construct(CMtCoder* p);
    void MtCoder_Destruct(CMtCoder* p);
    result_t MtCoder_Code(CMtCoder *p);

#ifdef __cplusplus
}
#endif

#endif
