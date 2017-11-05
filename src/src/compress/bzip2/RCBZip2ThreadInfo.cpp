/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2ThreadInfo.h"
#include "compress/bwt/RCMtf8Encoder.h"
#include "compress/bzip2/RCBZip2CRC.h"
#include "common/RCAlloc.h"
#include "compress/bzip2/RCBZip2MsbfEncoderTemp.h"
#include "compress/bzip2/RCBZip2Defs.h"
#include "compress/bzip2/RCBZip2Encoder.h"
#include "algorithm/BwtSort.h"
#include "algorithm/HuffEnc.h"

#define NUM_HUFF_PASSES 4 

// it must be < kMaxHuffmanLen = 20

#define MAX_HUFFMAN_LEN_FOR_ENCODING 16

/////////////////////////////////////////////////////////////////
//RCBZip2ThreadInfo class implementation

BEGIN_NAMESPACE_RCZIP

RCBZip2ThreadInfo::RCBZip2ThreadInfo():
    m_block(NULL),
    m_optimizeNumTables(false),
    m_encoder(NULL),
    m_mtfArray(NULL),
    m_tempArray(NULL),
    m_blockSorterIndex(NULL),
    m_outStreamCurrent(NULL),
    m_numCrcs(0),
    m_blockIndex(0)
{
#ifdef COMPRESS_BZIP2_MT
    m_packSize = 0 ;
#endif
}

RCBZip2ThreadInfo::~RCBZip2ThreadInfo()
{
    Free();
}

void RCBZip2ThreadInfo::WriteBits2(uint32_t value, uint32_t numBits)
{
    m_outStreamCurrent->WriteBits(value, numBits);
}

void RCBZip2ThreadInfo::WriteByte2(byte_t b)
{
    WriteBits2(b , 8);
}

void RCBZip2ThreadInfo::WriteBit2(bool v)
{
    WriteBits2((v ? 1 : 0), 1);
}

void RCBZip2ThreadInfo::WriteCRC2(uint32_t v)
{
    for (int32_t i = 0; i < 4; i++)
    {
        WriteByte2(((byte_t)(v >> (24 - i * 8))));
    }
}

// blockSize > 0
void RCBZip2ThreadInfo::EncodeBlock(const byte_t *block, uint32_t blockSize)
{
    WriteBit2(false); // Randomised = false
    {
        uint32_t origPtr = BlockSort(m_blockSorterIndex, block, blockSize);
        m_blockSorterIndex[origPtr] = blockSize;
        WriteBits2(origPtr, RCBZip2Defs::s_kNumOrigBits);
    }

    RCMtf8Encoder mtf ;
    int32_t numInUse = 0;
    {
        bool inUse[256];
        bool inUse16[16];
        uint32_t i;
        for (i = 0; i < 256; i++)
        {
            inUse[i] = false;
        }
        for (i = 0; i < 16; i++)
        {
            inUse16[i] = false;
        }
        for (i = 0; i < blockSize; i++)
        {
            inUse[block[i]] = true;
        }
        for (i = 0; i < 256; i++)
        {
            if (inUse[i])
            {
                inUse16[i >> 4] = true;
                mtf[numInUse++] = (byte_t)i;
            }
        }
        for (i = 0; i < 16; i++)
        {
            WriteBit2(inUse16[i]);
        }
        for (i = 0; i < 256; i++)
        {
            if (inUse16[i >> 4])
            {
                WriteBit2(inUse[i]);
            }
        }
    }
    int32_t alphaSize = numInUse + 2;

    byte_t* mtfs = m_mtfArray;
    uint32_t mtfArraySize = 0;
    uint32_t symbolCounts[RCBZip2Defs::s_kMaxAlphaSize];
    {
        for (int32_t i = 0; i < RCBZip2Defs::s_kMaxAlphaSize; i++)
        {
            symbolCounts[i] = 0;
        }
    }

    {
        uint32_t rleSize = 0;
        uint32_t i = 0;
        const uint32_t *bsIndex = m_blockSorterIndex;
        block--;
        do
        {
            int32_t pos = mtf.FindAndMove(block[bsIndex[i]]);
            if (pos == 0)
            {
                rleSize++;
            }
            else
            {
                while (rleSize != 0)
                {
                    rleSize--;
                    mtfs[mtfArraySize++] = (byte_t)(rleSize & 1);
                    symbolCounts[rleSize & 1]++;
                    rleSize >>= 1;
                }
                if (pos >= 0xFE)
                {
                    mtfs[mtfArraySize++] = 0xFF;
                    mtfs[mtfArraySize++] = (byte_t)(pos - 0xFE);
                }
                else
                {
                    mtfs[mtfArraySize++] = (byte_t)(pos + 1);
                }
                symbolCounts[pos + 1]++;
            }
        }
        while (++i < blockSize);

        while (rleSize != 0)
        {
            rleSize--;
            mtfs[mtfArraySize++] = (byte_t)(rleSize & 1);
            symbolCounts[rleSize & 1]++;
            rleSize >>= 1;
        }

        if (alphaSize < 256)
        {
            mtfs[mtfArraySize++] = (byte_t)(alphaSize - 1);
        }
        else
        {
            mtfs[mtfArraySize++] = 0xFF;
            mtfs[mtfArraySize++] = (byte_t)(alphaSize - 256);
        }
        symbolCounts[alphaSize - 1]++;
    }

    uint32_t numSymbols = 0;
    {
        for (int32_t i = 0; i < RCBZip2Defs::s_kMaxAlphaSize; i++)
        {
            numSymbols += symbolCounts[i];
        }
    }

    int32_t bestNumTables = RCBZip2Defs::s_kNumTablesMin;
    uint32_t bestPrice = 0xFFFFFFFF;
    uint32_t startPos = m_outStreamCurrent->GetPos();
    byte_t startCurByte = m_outStreamCurrent->GetCurByte();
    for (int32_t nt = RCBZip2Defs::s_kNumTablesMin; nt <= RCBZip2Defs::s_kNumTablesMax + 1; nt++)
    {
        int32_t numTables = 0 ;
        if(m_optimizeNumTables)
        {
            m_outStreamCurrent->SetPos(startPos);
            m_outStreamCurrent->SetCurState((startPos & 7), startCurByte);
            if (nt <= RCBZip2Defs::s_kNumTablesMax)
            {
                numTables = nt;
            }
            else
            {
                numTables = bestNumTables;
            }
        }
        else
        {
            if (numSymbols < 200)
            {
                numTables = 2;
            }
            else if (numSymbols < 600)
            {
                numTables = 3;
            }
            else if (numSymbols < 1200)
            {
                numTables = 4;
            }
            else if (numSymbols < 2400)
            {
                numTables = 5;
            }
            else
            {
                numTables = 6;
            }
        }

        WriteBits2(numTables, RCBZip2Defs::s_kNumTablesBits);
        uint32_t numSelectors = (numSymbols + RCBZip2Defs::s_kGroupSize - 1) / RCBZip2Defs::s_kGroupSize;
        WriteBits2(numSelectors, RCBZip2Defs::s_kNumSelectorsBits);
        {
            uint32_t remFreq = numSymbols;
            int32_t gs = 0;
            int32_t t = numTables;
            do
            {
                uint32_t tFreq = remFreq / t;
                int32_t ge = gs;
                uint32_t aFreq = 0;
                while (aFreq < tFreq) //  && ge < alphaSize)
                {
                    aFreq += symbolCounts[ge++];
                }

                if (ge - 1 > gs && t != numTables && t != 1 && (((numTables - t) & 1) == 1))
                {
                    aFreq -= symbolCounts[--ge];
                }

                byte_t *lens = m_lens[t - 1];
                int32_t i = 0;
                do
                {
                    lens[i] = (i >= gs && i < ge) ? 0 : 1;
                }
                while (++i < alphaSize);
                gs = ge;
                remFreq -= aFreq;
            }
            while(--t != 0);
        }

        for (int32_t pass = 0; pass < NUM_HUFF_PASSES; pass++)
        {
            {
                int32_t t = 0;
                do
                {
                    memset(m_freqs[t], 0, sizeof(m_freqs[t]));
                }
                while(++t < numTables);
            }
            {
                uint32_t mtfPos = 0;
                uint32_t g = 0;
                do
                {
                    uint32_t symbols[RCBZip2Defs::s_kGroupSize];
                    int32_t i = 0;
                    do
                    {
                        uint32_t symbol = mtfs[mtfPos++];
                        if (symbol >= 0xFF)
                        {
                            symbol += mtfs[mtfPos++];
                        }
                        symbols[i] = symbol;
                    }
                    while (++i < RCBZip2Defs::s_kGroupSize && mtfPos < mtfArraySize);

                    uint32_t bestPrice = 0xFFFFFFFF;
                    int32_t t = 0;
                    do
                    {
                        const byte_t *lens = m_lens[t];
                        uint32_t price = 0;
                        int32_t j = 0;
                        do
                        {
                            price += lens[symbols[j]];
                        }
                        while (++j < i);
                        if (price < bestPrice)
                        {
                            m_selectors[g] = (byte_t)t;
                            bestPrice = price;
                        }
                    }
                    while(++t < numTables);
                    uint32_t *freqs = m_freqs[m_selectors[g++]];
                    int32_t j = 0;
                    do
                    {
                        freqs[symbols[j]]++;
                    }
                    while (++j < i);
                }
                while (mtfPos < mtfArraySize);
            }

            int32_t t = 0;
            do
            {
                uint32_t *freqs = m_freqs[t];
                int32_t i = 0;
                do
                {
                    if (freqs[i] == 0)
                    {
                        freqs[i] = 1;
                    }
                }
                while(++i < alphaSize);
                Huffman_Generate(freqs, m_codes[t], m_lens[t], RCBZip2Defs::s_kMaxAlphaSize, MAX_HUFFMAN_LEN_FOR_ENCODING);
            }
            while(++t < numTables);
        }

        {
            byte_t mtfSel[RCBZip2Defs::s_kNumTablesMax];
            {
                int32_t t = 0;
                do
                {
                    mtfSel[t] = (byte_t)t;
                }
                while(++t < numTables);
            }

            uint32_t i = 0;
            do
            {
                byte_t sel = m_selectors[i];
                int32_t pos;
                for (pos = 0; mtfSel[pos] != sel; pos++)
                {
                    WriteBit2(true);
                }
                WriteBit2(false);
                for (; pos > 0; pos--)
                {
                    mtfSel[pos] = mtfSel[pos - 1];
                }
                mtfSel[0] = sel;
            }
            while(++i < numSelectors);
        }

        {
            int32_t t = 0;
            do
            {
                const byte_t *lens = m_lens[t];
                uint32_t len = lens[0];
                WriteBits2(len, RCBZip2Defs::s_kNumLevelsBits);
                int32_t i = 0;
                do
                {
                    uint32_t level = lens[i];
                    while (len != level)
                    {
                        WriteBit2(true);
                        if (len < level)
                        {
                            WriteBit2(false);
                            len++;
                        }
                        else
                        {
                            WriteBit2(true);
                            len--;
                        }
                    }
                    WriteBit2(false);
                }
                while (++i < alphaSize);
            }
            while(++t < numTables);
        }

        {
            uint32_t groupSize = 0;
            uint32_t groupIndex = 0;
            const byte_t *lens = 0;
            const uint32_t *codes = 0;
            uint32_t mtfPos = 0;
            do
            {
                uint32_t symbol = mtfs[mtfPos++];
                if (symbol >= 0xFF)
                {
                    symbol += mtfs[mtfPos++];
                }
                if (groupSize == 0)
                {
                    groupSize = RCBZip2Defs::s_kGroupSize;
                    int32_t t = m_selectors[groupIndex++];
                    lens = m_lens[t];
                    codes = m_codes[t];
                }
                groupSize--;
                m_outStreamCurrent->WriteBits(codes[symbol], lens[symbol]);
            }
            while (mtfPos < mtfArraySize);
        }

        if (!m_optimizeNumTables)
        {
            break;
        }
        uint32_t price = m_outStreamCurrent->GetPos() - startPos;
        if (price <= bestPrice)
        {
            if (nt == RCBZip2Defs::s_kNumTablesMax)
            {
                break;
            }
            bestPrice = price;
            bestNumTables = nt;
        }
    }
}

// blockSize > 0
uint32_t RCBZip2ThreadInfo::EncodeBlockWithHeaders(const byte_t *block, uint32_t blockSize)
{
    WriteByte2(RCBZip2Defs::s_kBlockSig0);
    WriteByte2(RCBZip2Defs::s_kBlockSig1);
    WriteByte2(RCBZip2Defs::s_kBlockSig2);
    WriteByte2(RCBZip2Defs::s_kBlockSig3);
    WriteByte2(RCBZip2Defs::s_kBlockSig4);
    WriteByte2(RCBZip2Defs::s_kBlockSig5);

    RCBZip2CRC crc ;
    int32_t numReps = 0 ;
    byte_t prevByte = block[0];
    uint32_t i = 0;
    do
    {
        byte_t b = block[i];
        if (numReps == RCBZip2Defs::s_kRleModeRepSize)
        {
            for (; b > 0; b--)
            {
                crc.UpdateByte(prevByte);
            }
            numReps = 0;
            continue;
        }
        if (prevByte == b)
        {
            numReps++;
        }
        else
        {
            numReps = 1;
            prevByte = b;
        }
        crc.UpdateByte(b);
    }
    while (++i < blockSize);
    uint32_t crcRes = crc.GetDigest();
    WriteCRC2(crcRes);
    EncodeBlock(block, blockSize);
    return crcRes;
}

void RCBZip2ThreadInfo::EncodeBlock2(const byte_t *block, uint32_t blockSize, uint32_t numPasses)
{
    uint32_t numCrcs = m_numCrcs;
    bool needCompare = false ;

    uint32_t startBytePos = m_outStreamCurrent->GetBytePos();
    uint32_t startPos = m_outStreamCurrent->GetPos();
    byte_t startCurByte = m_outStreamCurrent->GetCurByte();
    byte_t endCurByte = 0;
    uint32_t endPos = 0;
    if (numPasses > 1 && blockSize >= (1 << 10))
    {
        uint32_t blockSize0 = blockSize / 2;
        for (;(block[blockSize0] == block[blockSize0 - 1] ||
            block[blockSize0 - 1] == block[blockSize0 - 2]) &&
            blockSize0 < blockSize; blockSize0++);
        if (blockSize0 < blockSize)
        {
            EncodeBlock2(block, blockSize0, numPasses - 1);
            EncodeBlock2(block + blockSize0, blockSize - blockSize0, numPasses - 1);
            endPos = m_outStreamCurrent->GetPos();
            endCurByte = m_outStreamCurrent->GetCurByte();
            if ((endPos & 7) > 0)
            {
                WriteBits2(0, 8 - (endPos & 7));
            }
            m_outStreamCurrent->SetCurState((startPos & 7), startCurByte);
            needCompare = true;
        }
    }

    uint32_t startBytePos2 = m_outStreamCurrent->GetBytePos();
    uint32_t startPos2 = m_outStreamCurrent->GetPos();
    uint32_t crcVal = EncodeBlockWithHeaders(block, blockSize);
    uint32_t endPos2 = m_outStreamCurrent->GetPos();

    if (needCompare)
    {
        uint32_t size2 = endPos2 - startPos2;
        if (size2 < endPos - startPos)
        {
            uint32_t numBytes = m_outStreamCurrent->GetBytePos() - startBytePos2;
            byte_t *buffer = m_outStreamCurrent->GetStream();
            for (uint32_t i = 0; i < numBytes; i++)
            {
                buffer[startBytePos + i] = buffer[startBytePos2 + i];
            }
            m_outStreamCurrent->SetPos(startPos + endPos2 - startPos2);
            m_numCrcs = numCrcs;
            m_crcs[m_numCrcs++] = crcVal;
        }
        else
        {
            m_outStreamCurrent->SetPos(endPos);
            m_outStreamCurrent->SetCurState((endPos & 7), endCurByte);
        }
    }
    else
    {
        m_numCrcs = numCrcs;
        m_crcs[m_numCrcs++] = crcVal;
    }
}

bool RCBZip2ThreadInfo::Alloc()
{
    if (m_blockSorterIndex == 0)
    {
        m_blockSorterIndex = (uint32_t *)RCAlloc::Instance().BigAlloc(BLOCK_SORT_BUF_SIZE(RCBZip2Defs::s_kBlockSizeMax) * sizeof(uint32_t));
        if (m_blockSorterIndex == 0)
        {
            return false;
        }
    }

    if (m_block == 0)
    {
        m_block = (byte_t *)RCAlloc::Instance().MidAlloc(RCBZip2Defs::s_kBlockSizeMax * 5 + RCBZip2Defs::s_kBlockSizeMax / 10 + (20 << 10));
        if (m_block == 0)
        {
            return false;
        }
        m_mtfArray = m_block + RCBZip2Defs::s_kBlockSizeMax;
        m_tempArray = m_mtfArray + RCBZip2Defs::s_kBlockSizeMax * 2 + 2;
    }
    return true;
}

void RCBZip2ThreadInfo::Free()
{
    RCAlloc::Instance().BigFree(m_blockSorterIndex);
    m_blockSorterIndex = 0;
    RCAlloc::Instance().MidFree(m_block);
    m_block = 0;
#ifdef COMPRESS_BZIP2_MT
    m_spThread.reset() ;
#endif
}

HResult RCBZip2ThreadInfo::EncodeBlock3(uint32_t blockSize)
{
    RCBZip2MsbfEncoderTemp outStreamTemp ;
    outStreamTemp.SetStream(m_tempArray);
    outStreamTemp.Init();
    m_outStreamCurrent = &outStreamTemp;

    m_numCrcs = 0;
    EncodeBlock2(m_block, blockSize, m_encoder->m_numPasses);

#ifdef COMPRESS_BZIP2_MT
    if (m_encoder->m_mtMode)
    {
        m_encoder->m_threadsInfo[m_blockIndex].m_canWriteEvent.Lock();
    }
#endif
    for (uint32_t i = 0; i < m_numCrcs; i++)
    {
        m_encoder->m_combinedCRC.Update(m_crcs[i]);
    }
    m_encoder->WriteBytes(m_tempArray, outStreamTemp.GetPos(), outStreamTemp.GetCurByte());
    HResult res = RC_S_OK;
        
#ifdef COMPRESS_BZIP2_MT
    if (m_encoder->m_mtMode)
    {
        uint32_t blockIndex = m_blockIndex + 1;
        if (blockIndex == m_encoder->m_numThreads)
        {
            blockIndex = 0;
        }

        if (m_encoder->m_progress)
        {
            uint64_t unpackSize = m_encoder->m_outStream.GetProcessedSize();
            res = m_encoder->m_progress->SetRatioInfo(m_packSize, unpackSize);
        }

        m_encoder->m_threadsInfo[blockIndex].m_canWriteEvent.Set();
    }
#endif
    return res;
}

#ifdef COMPRESS_BZIP2_MT

static void MFThread(void* threadCoderInfo)
{
  return ((RCBZip2ThreadInfo*)threadCoderInfo)->ThreadFunc() ;
}

#define RINOK_THREAD(x) { WRes __result_ = (x); if(__result_ != 0) return __result_; }

HResult RCBZip2ThreadInfo::Create()
{
    RINOK_THREAD(m_streamWasFinishedEvent.Create());
    RINOK_THREAD(m_waitingWasStartedEvent.Create());
    RINOK_THREAD(m_canWriteEvent.Create());
    m_spThread.reset( new RCThread(MFThread, this) ) ;
    if(m_spThread->Start())
    {
        return RC_S_OK ;
    }
    else
    {
        return RC_E_FAIL ;
    }
    return RC_S_OK;
}

void RCBZip2ThreadInfo::FinishStream(bool needLeave)
{
    m_encoder->m_streamWasFinished = true ;
    m_streamWasFinishedEvent.Set() ;
    if (needLeave)
    {
        m_encoder->m_critSection.UnLock() ;
    }
    m_encoder->m_canStartWaitingEvent.Lock() ;
    m_waitingWasStartedEvent.Set() ;
}

void RCBZip2ThreadInfo::ThreadFunc()
{
    for (;;)
    {
        m_encoder->m_canProcessEvent.Lock();
        m_encoder->m_critSection.Lock();
        if (m_encoder->m_closeThreads)
        {
            m_encoder->m_critSection.UnLock();
            return ;
        }
        if (m_encoder->m_streamWasFinished)
        {
            FinishStream(true);
            continue;
        }
        HResult res = RC_S_OK;
        bool needLeave = true;
        try
        {
            uint32_t blockSize = m_encoder->ReadRleBlock(m_block);
            m_packSize = m_encoder->m_inStream.GetProcessedSize();
            m_blockIndex = m_encoder->m_nextBlockIndex;
            if (++m_encoder->m_nextBlockIndex == m_encoder->m_numThreads)
            {
                m_encoder->m_nextBlockIndex = 0;
            }
            if (blockSize == 0)
            {
                FinishStream(true);
                continue;
            }
            m_encoder->m_critSection.UnLock();
            needLeave = false;
            res = EncodeBlock3(blockSize);
        }
        catch(HResult errorCode)
        {
            res = errorCode ;
            if (res != RC_S_OK)
            {
                res = RC_E_FAIL ; 
            }
        }
        catch(...)
        {
            res = RC_E_FAIL;
        }
        if (res != RC_S_OK)
        {
            m_encoder->m_result = res ;
            FinishStream(needLeave);
            continue;
        }
    }
}

#endif

END_NAMESPACE_RCZIP
