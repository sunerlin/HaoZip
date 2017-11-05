/* Lzma2Enc.c -- LZMA2 Encoder
2009-05-26 : Igor Pavlov : Public domain */

/* #include <stdio.h> */
#include <string.h>

#include "algorithm/Lzma2Enc.h"

/* #define COMPRESS_MT */

#ifdef COMPRESS_MT
#include "algorithm/MtCoder.h"
#else
#define NUM_MT_CODER_THREADS_MAX 1
#endif

#define LZMA2_CONTROL_LZMA (1 << 7)
#define LZMA2_CONTROL_COPY_NO_RESET 2
#define LZMA2_CONTROL_COPY_RESET_DIC 1
#define LZMA2_CONTROL_EOF 0

#define LZMA2_LCLP_MAX 4

#define LZMA2_DIC_SIZE_FROM_PROP(p) (((uint32_t)2 | ((p) & 1)) << ((p) / 2 + 11))

#define LZMA2_PACK_SIZE_MAX (1 << 16)
#define LZMA2_COPY_CHUNK_SIZE LZMA2_PACK_SIZE_MAX
#define LZMA2_UNPACK_SIZE_MAX (1 << 21)
#define LZMA2_KEEP_WINDOW_SIZE LZMA2_UNPACK_SIZE_MAX

#define LZMA2_CHUNK_SIZE_COMPRESSED_MAX ((1 << 16) + 16)


#define PRF(x) /* x */

/* ---------- CLzma2EncInt ---------- */

typedef struct
{
    CLzmaEncHandle enc;
    uint64_t srcPos;
    byte_t props;
    Bool needInitState;
    Bool needInitProp;
} CLzma2EncInt;

static result_t Lzma2EncInt_Init(CLzma2EncInt *p, const CLzma2EncProps *props)
{
    byte_t propsEncoded[LZMA_PROPS_SIZE];
    size_t propsSize = LZMA_PROPS_SIZE;
    RINOK(LzmaEnc_SetProps(p->enc, &props->lzmaProps));
    RINOK(LzmaEnc_WriteProperties(p->enc, propsEncoded, &propsSize));
    p->srcPos = 0;
    p->props = propsEncoded[0];
    p->needInitState = True;
    p->needInitProp = True;
    return SZ_OK;
}

result_t LzmaEnc_PrepareForLzma2(CLzmaEncHandle pp, ISeqInStream *inStream, uint32_t keepWindowSize,
                                 ISzAlloc *alloc, ISzAlloc *allocBig);
result_t LzmaEnc_MemPrepare(CLzmaEncHandle pp, const byte_t *src, size_t srcLen,
                            uint32_t keepWindowSize, ISzAlloc *alloc, ISzAlloc *allocBig);
result_t LzmaEnc_CodeOneMemBlock(CLzmaEncHandle pp, Bool reInit,
                                 byte_t *dest, size_t *destLen, uint32_t desiredPackSize, uint32_t *unpackSize);
const byte_t *LzmaEnc_GetCurBuf(CLzmaEncHandle pp);
void LzmaEnc_Finish(CLzmaEncHandle pp);
void LzmaEnc_SaveState(CLzmaEncHandle pp);
void LzmaEnc_RestoreState(CLzmaEncHandle pp);


static result_t Lzma2EncInt_EncodeSubblock(CLzma2EncInt *p, byte_t *outBuf,
                                           size_t *packSizeRes, ISeqOutStream *outStream)
{
    size_t packSizeLimit = *packSizeRes;
    size_t packSize = packSizeLimit;
    uint32_t unpackSize = LZMA2_UNPACK_SIZE_MAX;
    unsigned lzHeaderSize = 5 + (p->needInitProp ? 1 : 0);
    Bool useCopyBlock;
    result_t res;

    *packSizeRes = 0;
    if (packSize < lzHeaderSize)
        return SZ_ERROR_OUTPUT_EOF;
    packSize -= lzHeaderSize;

    LzmaEnc_SaveState(p->enc);
    res = LzmaEnc_CodeOneMemBlock(p->enc, p->needInitState,
        outBuf + lzHeaderSize, &packSize, LZMA2_PACK_SIZE_MAX, &unpackSize);

    PRF(printf("\npackSize = %7d unpackSize = %7d  ", packSize, unpackSize));

    if (unpackSize == 0)
        return res;

    if (res == SZ_OK)
        useCopyBlock = (packSize + 2 >= unpackSize || packSize > (1 << 16));
    else
    {
        if (res != SZ_ERROR_OUTPUT_EOF)
            return res;
        res = SZ_OK;
        useCopyBlock = True;
    }

    if (useCopyBlock)
    {
        size_t destPos = 0;
        PRF(printf("################# COPY           "));
        while (unpackSize > 0)
        {
            uint32_t u = (unpackSize < LZMA2_COPY_CHUNK_SIZE) ? unpackSize : LZMA2_COPY_CHUNK_SIZE;
            if (packSizeLimit - destPos < u + 3)
                return SZ_ERROR_OUTPUT_EOF;
            outBuf[destPos++] = (byte_t)(p->srcPos == 0 ? LZMA2_CONTROL_COPY_RESET_DIC : LZMA2_CONTROL_COPY_NO_RESET);
            outBuf[destPos++] = (byte_t)((u - 1) >> 8);
            outBuf[destPos++] = (byte_t)(u - 1);
            memcpy(outBuf + destPos, LzmaEnc_GetCurBuf(p->enc) - unpackSize, u);
            unpackSize -= u;
            destPos += u;
            p->srcPos += u;
            if (outStream)
            {
                *packSizeRes += destPos;
                if (outStream->Write(outStream, outBuf, destPos) != destPos)
                    return SZ_ERROR_WRITE;
                destPos = 0;
            }
            else
                *packSizeRes = destPos;
            /* needInitState = True; */
        }
        LzmaEnc_RestoreState(p->enc);
        return SZ_OK;
    }
    {
        size_t destPos = 0;
        uint32_t u = unpackSize - 1;
        uint32_t pm = (uint32_t)(packSize - 1);
        unsigned mode = (p->srcPos == 0) ? 3 : (p->needInitState ? (p->needInitProp ? 2 : 1) : 0);

        PRF(printf("               "));

        outBuf[destPos++] = (byte_t)(LZMA2_CONTROL_LZMA | (mode << 5) | (u >> 16) & 0x1F);
        outBuf[destPos++] = (byte_t)(u >> 8);
        outBuf[destPos++] = (byte_t)u;
        outBuf[destPos++] = (byte_t)(pm >> 8);
        outBuf[destPos++] = (byte_t)pm;

        if (p->needInitProp)
            outBuf[destPos++] = p->props;

        p->needInitProp = False;
        p->needInitState = False;
        destPos += packSize;
        p->srcPos += unpackSize;

        if (outStream)
            if (outStream->Write(outStream, outBuf, destPos) != destPos)
                return SZ_ERROR_WRITE;
        *packSizeRes = destPos;
        return SZ_OK;
    }
}

/* ---------- Lzma2 Props ---------- */

void Lzma2EncProps_Init(CLzma2EncProps *p)
{
    LzmaEncProps_Init(&p->lzmaProps);
    p->numTotalThreads = -1;
    p->numBlockThreads = -1;
    p->blockSize = 0;
}


static result_t Progress(ICompressProgress *p, uint64_t inSize, uint64_t outSize)
{
    return (p && p->Progress(p, inSize, outSize) != SZ_OK) ? SZ_ERROR_PROGRESS : SZ_OK;
}

/* ---------- Lzma2 ---------- */

extern struct _CLzma2Enc;

typedef struct _CLzma2Enc
{
    byte_t propEncoded;
    CLzma2EncProps props;

    byte_t *outBuf;

    ISzAlloc *alloc;
    ISzAlloc *allocBig;

    CLzma2EncInt coders[NUM_MT_CODER_THREADS_MAX];

#ifdef COMPRESS_MT
    CMtCoder mtCoder;
#endif

} CLzma2Enc;


/* ---------- Lzma2EncThread ---------- */

static result_t Lzma2Enc_EncodeMt1(CLzma2EncInt *p, CLzma2Enc *mainEncoder,
                                   ISeqOutStream *outStream, ISeqInStream *inStream, ICompressProgress *progress)
{
    uint64_t packTotal = 0;
    result_t res = SZ_OK;

    if (mainEncoder->outBuf == 0)
    {
        mainEncoder->outBuf = IAlloc_Alloc(mainEncoder->alloc, LZMA2_CHUNK_SIZE_COMPRESSED_MAX);
        if (mainEncoder->outBuf == 0)
            return SZ_ERROR_MEM;
    }
    RINOK(Lzma2EncInt_Init(p, &mainEncoder->props));
    RINOK(LzmaEnc_PrepareForLzma2(p->enc, inStream, LZMA2_KEEP_WINDOW_SIZE,
        mainEncoder->alloc, mainEncoder->allocBig));
    for (;;)
    {
        size_t packSize = LZMA2_CHUNK_SIZE_COMPRESSED_MAX;
        res = Lzma2EncInt_EncodeSubblock(p, mainEncoder->outBuf, &packSize, outStream);
        if (res != SZ_OK)
            break;
        packTotal += packSize;
        res = Progress(progress, p->srcPos, packTotal);
        if (res != SZ_OK)
            break;
        if (packSize == 0)
            break;
    }
    LzmaEnc_Finish(p->enc);
    if (res == SZ_OK)
    {
        byte_t b = 0;
        if (outStream->Write(outStream, &b, 1) != 1)
            return SZ_ERROR_WRITE;
    }
    return res;
}

#ifdef COMPRESS_MT

typedef struct
{
    IMtCoderCallback funcTable;
    CLzma2Enc *lzma2Enc;
} CMtCallbackImp;

static result_t MtCallbackImp_Code(void *pp, unsigned index, byte_t *dest, size_t *destSize,
                                   const byte_t *src, size_t srcSize, int finished)
{
    CMtCallbackImp *imp = (CMtCallbackImp *)pp;
    CLzma2Enc *mainEncoder = imp->lzma2Enc;
    CLzma2EncInt *p = &mainEncoder->coders[index];

    result_t res = SZ_OK;
    {
        size_t destLim = *destSize;
        *destSize = 0;

        if (srcSize != 0)
        {
            RINOK(Lzma2EncInt_Init(p, &mainEncoder->props));

            RINOK(LzmaEnc_MemPrepare(p->enc, src, srcSize, LZMA2_KEEP_WINDOW_SIZE,
                mainEncoder->alloc, mainEncoder->allocBig));

            while (p->srcPos < srcSize)
            {
                size_t packSize = destLim - *destSize;
                res = Lzma2EncInt_EncodeSubblock(p, dest + *destSize, &packSize, NULL);
                if (res != SZ_OK)
                    break;
                *destSize += packSize;

                if (packSize == 0)
                {
                    res = SZ_ERROR_FAIL;
                    break;
                }

                if (MtProgress_Set(&mainEncoder->mtCoder.mtProgress, index, p->srcPos, *destSize) != SZ_OK)
                {
                    res = SZ_ERROR_PROGRESS;
                    break;
                }
            }
            LzmaEnc_Finish(p->enc);
            if (res != SZ_OK)
                return res;
        }
        if (finished)
        {
            if (*destSize == destLim)
                return SZ_ERROR_OUTPUT_EOF;
            dest[(*destSize)++] = 0;
        }
    }
    return res;
}

#endif

/* ---------- Lzma2Enc ---------- */

CLzma2EncHandle Lzma2Enc_Create(ISzAlloc *alloc, ISzAlloc *allocBig)
{
    CLzma2Enc *p = (CLzma2Enc *)alloc->Alloc(alloc, sizeof(CLzma2Enc));
    if (p == 0)
        return NULL;
    Lzma2EncProps_Init(&p->props);
    Lzma2EncProps_Normalize(&p->props);
    p->outBuf = 0;
    p->alloc = alloc;
    p->allocBig = allocBig;
    {
        unsigned i;
        for (i = 0; i < NUM_MT_CODER_THREADS_MAX; i++)
            p->coders[i].enc = 0;
    }
#ifdef COMPRESS_MT
    MtCoder_Construct(&p->mtCoder);
#endif

    return p;
}

void Lzma2Enc_Destroy(CLzma2EncHandle pp)
{
    CLzma2Enc *p = (CLzma2Enc *)pp;
    unsigned i;
    for (i = 0; i < NUM_MT_CODER_THREADS_MAX; i++)
    {
        CLzma2EncInt *t = &p->coders[i];
        if (t->enc)
        {
            LzmaEnc_Destroy(t->enc, p->alloc, p->allocBig);
            t->enc = 0;
        }
    }

#ifdef COMPRESS_MT
    MtCoder_Destruct(&p->mtCoder);
#endif

    IAlloc_Free(p->alloc, p->outBuf);
    IAlloc_Free(p->alloc, pp);
}

void Lzma2EncProps_Normalize(CLzma2EncProps *p)
{
    int t1, t1n, t2, t3;
    CLzmaEncProps lzmaProps = p->lzmaProps;

    LzmaEncProps_Normalize(&lzmaProps);

    t1 = p->lzmaProps.numThreads;
    t1n = lzmaProps.numThreads;
    t2 = p->numBlockThreads;
    t3 = p->numTotalThreads;

#ifdef COMPRESS_MT
    if (t2 > NUM_MT_CODER_THREADS_MAX)
        t2 = NUM_MT_CODER_THREADS_MAX;
#else
    t2 = 1;
#endif

    if (t3 <= 0)
    {
        if (t2 <= 0)
            t2 = 1;
        t3 = t1n * t2;
    }
    else
    {
        if (t2 <= 0)
        {
            t2 = t3 / t1n;
            if (t2 == 0)
            {
                t1 = 1;
                t2 = t3;
            }
        }
        else if (t1 <= 0)
        {
            t1 = t3 / t2;
            if (t1 == 0)
                t1 = 1;
        }
        else
            t3 = t1n * t2;
    }

    p->lzmaProps.numThreads = t1;
    p->numBlockThreads = t2;
    p->numTotalThreads = t3;
    LzmaEncProps_Normalize(&p->lzmaProps);

    if (p->blockSize == 0)
    {
        uint64_t blockSize = (uint64_t)lzmaProps.dictSize << 2;
        const uint32_t kMinSize = (uint32_t)1 << 20;
        const uint32_t kMaxSize = (uint32_t)1 << 28;
        if (blockSize < kMinSize) blockSize = kMinSize;
        if (blockSize > kMaxSize) blockSize = kMaxSize;
        if (blockSize < lzmaProps.dictSize)
            blockSize = lzmaProps.dictSize;
        p->blockSize = (size_t)blockSize;
    }
}

result_t Lzma2Enc_SetProps(CLzma2EncHandle pp, const CLzma2EncProps *props)
{
    CLzma2Enc *p = (CLzma2Enc *)pp;
    CLzmaEncProps lzmaProps = props->lzmaProps;
    LzmaEncProps_Normalize(&lzmaProps);
    if (lzmaProps.lc + lzmaProps.lp > LZMA2_LCLP_MAX)
        return SZ_ERROR_PARAM;
    p->props = *props;
    Lzma2EncProps_Normalize(&p->props);
    return SZ_OK;
}

byte_t Lzma2Enc_WriteProperties(CLzma2EncHandle pp)
{
    CLzma2Enc *p = (CLzma2Enc *)pp;
    unsigned i;
    uint32_t dicSize = LzmaEncProps_GetDictSize(&p->props.lzmaProps);
    for (i = 0; i < 40; i++)
        if (dicSize <= LZMA2_DIC_SIZE_FROM_PROP(i))
            break;
    return (byte_t)i;
}

result_t Lzma2Enc_Encode(CLzma2EncHandle pp,
                         ISeqOutStream *outStream, ISeqInStream *inStream, ICompressProgress *progress)
{
    CLzma2Enc *p = (CLzma2Enc *)pp;
    int i;

    for (i = 0; i < p->props.numBlockThreads; i++)
    {
        CLzma2EncInt *t = &p->coders[i];
        if (t->enc == NULL)
        {
            t->enc = LzmaEnc_Create(p->alloc);
            if (t->enc == NULL)
                return SZ_ERROR_MEM;
        }
    }

#ifdef COMPRESS_MT
    if (p->props.numBlockThreads <= 1)
#endif
        return Lzma2Enc_EncodeMt1(&p->coders[0], p, outStream, inStream, progress);

#ifdef COMPRESS_MT

    {
        CMtCallbackImp mtCallback;

        mtCallback.funcTable.Code = MtCallbackImp_Code;
        mtCallback.lzma2Enc = p;

        p->mtCoder.progress = progress;
        p->mtCoder.inStream = inStream;
        p->mtCoder.outStream = outStream;
        p->mtCoder.alloc = p->alloc;
        p->mtCoder.mtCallback = &mtCallback.funcTable;

        p->mtCoder.blockSize = p->props.blockSize;
        p->mtCoder.destBlockSize = p->props.blockSize + (p->props.blockSize >> 10) + 16;
        p->mtCoder.numThreads = p->props.numBlockThreads;

        return MtCoder_Code(&p->mtCoder);
    }
#endif
}
