/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2Decoder.h"
#include "compress/bwt/RCMtf8Decoder.h"
#include "compress/bzip2/RCBZip2CRC.h"
#include "interface/RCMethodDefs.h"

/////////////////////////////////////////////////////////////////
//RCBZip2Decoder class implementation

BEGIN_NAMESPACE_RCZIP

#define BZIP2_DECODER_BUFFER_SIZE   (1 << 17)
#define BZIP2_MAX_THREAD_NIM        4

static int16_t s_kBZip2DecoderRandNums[512] = 
{
   619, 720, 127, 481, 931, 816, 813, 233, 566, 247,
   985, 724, 205, 454, 863, 491, 741, 242, 949, 214,
   733, 859, 335, 708, 621, 574, 73, 654, 730, 472,
   419, 436, 278, 496, 867, 210, 399, 680, 480, 51,
   878, 465, 811, 169, 869, 675, 611, 697, 867, 561,
   862, 687, 507, 283, 482, 129, 807, 591, 733, 623,
   150, 238, 59, 379, 684, 877, 625, 169, 643, 105,
   170, 607, 520, 932, 727, 476, 693, 425, 174, 647,
   73, 122, 335, 530, 442, 853, 695, 249, 445, 515,
   909, 545, 703, 919, 874, 474, 882, 500, 594, 612,
   641, 801, 220, 162, 819, 984, 589, 513, 495, 799,
   161, 604, 958, 533, 221, 400, 386, 867, 600, 782,
   382, 596, 414, 171, 516, 375, 682, 485, 911, 276,
   98, 553, 163, 354, 666, 933, 424, 341, 533, 870,
   227, 730, 475, 186, 263, 647, 537, 686, 600, 224,
   469, 68, 770, 919, 190, 373, 294, 822, 808, 206,
   184, 943, 795, 384, 383, 461, 404, 758, 839, 887,
   715, 67, 618, 276, 204, 918, 873, 777, 604, 560,
   951, 160, 578, 722, 79, 804, 96, 409, 713, 940,
   652, 934, 970, 447, 318, 353, 859, 672, 112, 785,
   645, 863, 803, 350, 139, 93, 354, 99, 820, 908,
   609, 772, 154, 274, 580, 184, 79, 626, 630, 742,
   653, 282, 762, 623, 680, 81, 927, 626, 789, 125,
   411, 521, 938, 300, 821, 78, 343, 175, 128, 250,
   170, 774, 972, 275, 999, 639, 495, 78, 352, 126,
   857, 956, 358, 619, 580, 124, 737, 594, 701, 612,
   669, 112, 134, 694, 363, 992, 809, 743, 168, 974,
   944, 375, 748, 52, 600, 747, 642, 182, 862, 81,
   344, 805, 988, 739, 511, 655, 814, 334, 249, 515,
   897, 955, 664, 981, 649, 113, 974, 459, 893, 228,
   433, 837, 553, 268, 926, 240, 102, 654, 459, 51,
   686, 754, 806, 760, 493, 403, 415, 394, 687, 700,
   946, 670, 656, 610, 738, 392, 760, 799, 887, 653,
   978, 321, 576, 617, 626, 502, 894, 679, 243, 440,
   680, 879, 194, 572, 640, 724, 926, 56, 204, 700,
   707, 151, 457, 449, 797, 195, 791, 558, 945, 679,
   297, 59, 87, 824, 713, 663, 412, 693, 342, 606,
   134, 108, 571, 364, 631, 212, 174, 643, 304, 329,
   343, 97, 430, 751, 497, 314, 983, 374, 822, 928,
   140, 206, 73, 263, 980, 736, 876, 478, 430, 305,
   170, 514, 364, 692, 829, 82, 855, 953, 676, 246,
   369, 970, 294, 750, 807, 827, 150, 790, 288, 923,
   804, 378, 215, 828, 592, 281, 565, 555, 710, 82,
   896, 831, 547, 261, 524, 462, 293, 465, 502, 56,
   661, 821, 976, 991, 658, 869, 905, 758, 745, 193,
   768, 550, 608, 933, 378, 286, 215, 979, 792, 961,
   61, 688, 793, 644, 986, 403, 106, 366, 905, 644,
   372, 567, 466, 434, 645, 210, 389, 550, 919, 135,
   780, 773, 635, 389, 707, 100, 626, 958, 165, 504,
   920, 176, 193, 713, 857, 265, 203, 50, 668, 108,
   645, 990, 626, 197, 510, 357, 358, 850, 858, 364,
   936, 638
};

class RCBZip2DecoderFlusher
{
public:
    RCBZip2DecoderFlusher(RCBZip2Decoder *coder, bool releaseInStream):
        _coder(coder), 
        m_needFlush(true),
        m_releaseInStream(releaseInStream)
    {
    }
    ~RCBZip2DecoderFlusher()
    {
        if (m_needFlush)
        {
            _coder->Flush();
        }
        _coder->ReleaseStreams(m_releaseInStream);
    }
    
public:
    bool m_needFlush ;
    bool m_releaseInStream ;
    
private:
    RCBZip2Decoder* _coder;
};

RCBZip2Decoder::RCBZip2Decoder()
{
#ifdef COMPRESS_BZIP2_MT
    m_states = 0 ;
    m_numThreadsPrev = 0 ;
    m_numThreads = 1 ;
#endif;
    m_needInStreamInit = true ;
    m_isIszBZip2 = false;
}

RCBZip2Decoder::~RCBZip2Decoder()
{
#ifdef COMPRESS_BZIP2_MT
    Free() ;
#endif
}

uint32_t RCBZip2Decoder::ReadBits(int32_t numBits)
{
    return m_inStream.ReadBits(numBits);
}

byte_t RCBZip2Decoder::ReadByte()
{
    return (byte_t)ReadBits(8);
}

bool RCBZip2Decoder::ReadBit()
{
    return ReadBits(1) != 0 ;
}

uint32_t RCBZip2Decoder::ReadCRC()
{
    uint32_t crc = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        crc <<= 8;
        crc |= ReadByte();
    }
    return crc;
}

HResult RCBZip2Decoder::DecodeFile(bool& isBZ, ICompressProgressInfo* progress)
{
#ifdef COMPRESS_BZIP2_MT
    m_progress = progress;
    HResult hr = Create() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    for (uint32_t t = 0; t < m_numThreads; t++)
    {
        RCBZip2DecoderState& s = m_states[t] ;
        if (!s.Alloc())
        {
            return RC_E_OUTOFMEMORY ;
        }
        if (m_isMtMode)
        {
            HResult hr = s.m_streamWasFinishedEvent.Reset() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            hr = s.m_waitingWasStartedEvent.Reset() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            hr = s.m_canWriteEvent.Reset() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
#else
    if (!m_states[0].Alloc())
    {
        return RC_E_OUTOFMEMORY ;
    }
#endif

    isBZ = false;
    byte_t s[6] ;
    int32_t i = 0 ;
    for (i = 0; i < 4; i++)
    {
        s[i] = ReadByte();
    }

    byte_t sig[3] = {RCBZip2Defs::s_kArSig0, RCBZip2Defs::s_kArSig1, RCBZip2Defs::s_kArSig2};
    if (m_isIszBZip2)
    {
        sig[0] = RCBZip2Defs::s_kArIszSig0;
        sig[1] = RCBZip2Defs::s_kArIszSig1;
        sig[2] = RCBZip2Defs::s_kArIszSig2;
    }

    if (s[0] != sig[0] ||
        s[1] != sig[1] ||
        s[2] != sig[2] ||
        s[3] <= RCBZip2Defs::s_kArSig3 ||
        s[3] > RCBZip2Defs::s_kArSig3 + RCBZip2Defs::s_kBlockSizeMultMax)
    {
        return RC_S_OK;
    }
    isBZ = true;
    uint32_t dicSize = (uint32_t)(s[3] - RCBZip2Defs::s_kArSig3) * RCBZip2Defs::s_kBlockSizeStep;

    m_combinedCRC.Init() ;
#ifdef COMPRESS_BZIP2_MT
    if (m_isMtMode)
    {
        m_nextBlockIndex = 0;
        m_streamWasFinished1 = false ;
        m_streamWasFinished2 = false ;
        m_isCloseThreads = false;
        m_canStartWaitingEvent.Reset();
        m_states[0].m_canWriteEvent.Set();
        m_blockSizeMax = dicSize;
        m_result1 = RC_S_OK ;
        m_result2 = RC_S_OK ;
        m_canProcessEvent.Set();
        uint32_t t;
        for (t = 0; t < m_numThreads; t++)
        {
            m_states[t].m_streamWasFinishedEvent.Lock();
        }
        m_canProcessEvent.Reset();
        m_canStartWaitingEvent.Set();
        for (t = 0; t < m_numThreads; t++)
        {
            m_states[t].m_waitingWasStartedEvent.Lock();
        }
        m_canStartWaitingEvent.Reset();
        if(!IsSuccess(m_result2))
        {
            return m_result2 ;
        }
        if(!IsSuccess(m_result1))
        {
            return m_result1 ;
        }
    }
    else
#endif
    {
        RCBZip2DecoderState& state = m_states[0] ;
        for (;;)
        {
            HResult hr = SetRatioProgress(m_inStream.GetProcessedSize());
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            bool wasFinished = false ;
            uint32_t crc;
            hr = ReadSignatures(wasFinished, crc) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (wasFinished)
            {
                return RC_S_OK ;
            }

            uint32_t blockSize = 0 ;
            uint32_t origPtr = 0 ;
            bool randMode = false ;
            hr = ReadBlock( &m_inStream, state.m_counters, dicSize,
                            m_selectors, m_huffmanDecoders,
                            &blockSize, &origPtr, &randMode) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            DecodeBlock1(state.m_counters, blockSize);
            if ((randMode ?
                            DecodeBlock2Rand(state.m_counters + 256, blockSize, origPtr, m_outStream) :
                            DecodeBlock2(state.m_counters + 256, blockSize, origPtr, m_outStream)) != crc)
            {
                return RC_S_FALSE;
            }
        }
    }
    return SetRatioProgress(m_inStream.GetProcessedSize());
}

HResult RCBZip2Decoder::ReadSignatures(bool& wasFinished, uint32_t& crc)
{
    wasFinished = false;
    byte_t s[6];
    for (int32_t i = 0; i < 6; i++)
    {
        s[i] = ReadByte();
    }
    crc = ReadCRC();
    if (s[0] == RCBZip2Defs::s_kFinSig0)
    {
        if (s[1] != RCBZip2Defs::s_kFinSig1 ||
            s[2] != RCBZip2Defs::s_kFinSig2 ||
            s[3] != RCBZip2Defs::s_kFinSig3 ||
            s[4] != RCBZip2Defs::s_kFinSig4 ||
            s[5] != RCBZip2Defs::s_kFinSig5)
            return RC_S_FALSE;

        wasFinished = true;
        return (crc == m_combinedCRC.GetDigest()) ? RC_S_OK : RC_S_FALSE;
    }
    if (s[0] != RCBZip2Defs::s_kBlockSig0 ||
        s[1] != RCBZip2Defs::s_kBlockSig1 ||
        s[2] != RCBZip2Defs::s_kBlockSig2 ||
        s[3] != RCBZip2Defs::s_kBlockSig3 ||
        s[4] != RCBZip2Defs::s_kBlockSig4 ||
        s[5] != RCBZip2Defs::s_kBlockSig5)
    {
        return RC_S_FALSE;
    }
    m_combinedCRC.Update(crc);
    return RC_S_OK;
}

HResult RCBZip2Decoder::Flush()
{
    return m_outStream.Flush();
}

void RCBZip2Decoder::ReleaseStreams(bool releaseInStream)
{
    if(releaseInStream)
    {
        m_inStream.ReleaseStream();
    }
    m_outStream.ReleaseStream();
}

HResult RCBZip2Decoder::Code(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* /*inSize*/, 
                             const uint64_t* /*outSize*/,
                             ICompressProgressInfo* progress)
{
    try
    {
        m_needInStreamInit = true ;
        bool isBZ = false ;
        HResult hr = CodeReal(inStream, outStream, isBZ, progress) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        return isBZ ? RC_S_OK : S_FALSE;
    }
    catch(HResult errorCode)
    {
        return errorCode ;
    }
    catch(...)
    {
        return RC_E_FAIL ;
    }
}

HResult RCBZip2Decoder::CodeReal(ISequentialInStream* inStream,
                                 ISequentialOutStream* outStream, 
                                 bool& isBZ,
                                 ICompressProgressInfo* progress)
{
    isBZ = false ;
    if (!m_inStream.Create(BZIP2_DECODER_BUFFER_SIZE))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_outStream.Create(BZIP2_DECODER_BUFFER_SIZE))
    {
        return RC_E_OUTOFMEMORY;
    }
    if(inStream)
    {
        m_inStream.SetStream(inStream);
    }
    RCBZip2DecoderFlusher flusher(this, inStream != NULL);
    if(m_needInStreamInit)
    {
        m_inStream.Init();
        m_needInStreamInit = false ;
    }
    m_inStart = m_inStream.GetProcessedSize() ;
    m_inStream.AlignToByte() ;
  
    m_outStream.SetStream(outStream);
    m_outStream.Init();

    HResult hr = DecodeFile(isBZ, progress) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    flusher.m_needFlush = false ;
    return Flush() ;
}

HResult RCBZip2Decoder::GetInStreamProcessedSize(uint64_t& size)
{
    size = m_inStream.GetProcessedSize();
    return RC_S_OK ;
}

uint32_t RCBZip2Decoder::ReadBits(RCBitmDecoder<RCInBuffer>* inStream, int32_t num)
{
    return inStream->ReadBits(num) ;
}

uint32_t RCBZip2Decoder::ReadBit(RCBitmDecoder<RCInBuffer>* inStream)
{
    return inStream->ReadBits(1) ;
}

HResult RCBZip2Decoder::ReadBlock(RCBitmDecoder<RCInBuffer>* inStream,
                                  uint32_t* charCounters, 
                                  uint32_t blockSizeMax, 
                                  byte_t* selectors, 
                                  RCBZip2HuffmanDecoder* huffmanDecoders,
                                  uint32_t* blockSizeRes, 
                                  uint32_t* origPtrRes, 
                                  bool* randRes)
{
    *randRes = ReadBit(inStream) ? true : false;
    *origPtrRes = ReadBits(inStream, RCBZip2Defs::s_kNumOrigBits);

    // in original code it compares OrigPtr to (uint32_t)(10 + blockSizeMax)) : why ?
    if (*origPtrRes >= blockSizeMax)
    {
        return RC_S_FALSE;
    }

    RCMtf8Decoder mtf;
    mtf.StartInit();

    int32_t numInUse = 0;
    {
        byte_t inUse16[16];
        int32_t i = 0 ;
        for (i = 0; i < 16; i++)
        {
            inUse16[i] = (byte_t)ReadBit(inStream);
        }
        for (i = 0; i < 256; i++)
        {
            if (inUse16[i >> 4])
            {
                if (ReadBit(inStream))
                {
                    mtf.Add(numInUse++, (byte_t)i);
                }
            }            
        }
        if (numInUse == 0)
        {
            return RC_S_FALSE;
        }
    }
    int32_t alphaSize = numInUse + 2;
    int32_t numTables = ReadBits(inStream, RCBZip2Defs::s_kNumTablesBits);
    if (numTables < RCBZip2Defs::s_kNumTablesMin || numTables > RCBZip2Defs::s_kNumTablesMax)
    {
        return RC_S_FALSE;
    }

    uint32_t numSelectors = ReadBits(inStream, RCBZip2Defs::s_kNumSelectorsBits);
    if (numSelectors < 1 || numSelectors > RCBZip2Defs::s_kNumSelectorsMax)
    {
        return RC_S_FALSE;
    }

    {
        byte_t mtfPos[RCBZip2Defs::s_kNumTablesMax];
        int32_t t = 0;
        do
        {
            mtfPos[t] = (byte_t)t;
        }while(++t < numTables);
        uint32_t i = 0;
        do
        {
            int32_t j = 0;
            while (ReadBit(inStream))
            {
                if (++j >= numTables)
                {
                    return RC_S_FALSE;
                }
            }
            byte_t tmp = mtfPos[j];
            for (;j > 0; j--)
            {
                mtfPos[j] = mtfPos[j - 1];
            }
            selectors[i] = mtfPos[0] = tmp;
        }
        while(++i < numSelectors);
    }

    int32_t t = 0;
    do
    {
        byte_t lens[RCBZip2Defs::s_kMaxAlphaSize];
        int32_t len = (int32_t)ReadBits(inStream, RCBZip2Defs::s_kNumLevelsBits);
        int32_t i = 0 ;
        for (i = 0; i < alphaSize; i++)
        {
            for (;;)
            {
                if (len < 1 || len > RCBZip2Defs::s_kMaxHuffmanLen)
                {
                    return RC_S_FALSE ;
                }
                if (!ReadBit(inStream))
                {
                    break;
                }
                len += 1 - (int32_t)(ReadBit(inStream) << 1);
            }
            lens[i] = (byte_t)len;
        }
        for (; i < RCBZip2Defs::s_kMaxAlphaSize; i++)
        {
            lens[i] = 0;
        }
        if(!huffmanDecoders[t].SetCodeLengths(lens))
        {
            return RC_S_FALSE;
        }
    }while(++t < numTables);

    {
        for (int32_t i = 0; i < 256; i++)
        {
            charCounters[i] = 0;
        }
    }

    uint32_t blockSize = 0;
    {
        uint32_t groupIndex = 0;
        uint32_t groupSize = 0;
        RCBZip2HuffmanDecoder *huffmanDecoder = 0;
        int32_t runPower = 0;
        uint32_t runCounter = 0;
        for (;;)
        {
            if (groupSize == 0)
            {
                if (groupIndex >= numSelectors)
                {
                    return RC_S_FALSE;
                }
                groupSize = RCBZip2Defs::s_kGroupSize;
                huffmanDecoder = &huffmanDecoders[selectors[groupIndex++]];
            }
            groupSize--;
            uint32_t nextSym = huffmanDecoder->DecodeSymbol(inStream);
            if (nextSym < 2)
            {
                runCounter += ((uint32_t)(nextSym + 1) << runPower++);
                if (blockSizeMax - blockSize < runCounter)
                {
                    return RC_S_FALSE;
                }
                continue;
            }
            if (runCounter != 0)
            {
                uint32_t b = (uint32_t)mtf.GetHead();
                charCounters[b] += runCounter;
                do
                {
                    charCounters[256 + blockSize++] = b;
                }while(--runCounter != 0);
                runPower = 0;
            }
            if (nextSym <= (uint32_t)numInUse)
            {
                uint32_t b = (uint32_t)mtf.GetAndMove((int32_t)nextSym - 1);
                if (blockSize >= blockSizeMax)
                {
                    return RC_S_FALSE;
                }
                charCounters[b]++;
                charCounters[256 + blockSize++] = b;
            }
            else if (nextSym == (uint32_t)numInUse + 1)
            {
                break;
            }
            else
            {
                return RC_S_FALSE;
            }
        }
    }
    *blockSizeRes = blockSize;
    return (*origPtrRes < blockSize) ? RC_S_OK : RC_S_FALSE;
}

void RCBZip2Decoder::DecodeBlock1(uint32_t* charCounters, uint32_t blockSize)
{
    {
        uint32_t sum = 0;
        for(uint32_t i = 0; i < 256; i++)
        {
            sum += charCounters[i];
            charCounters[i] = sum - charCounters[i];
        }
    }

    uint32_t *tt = charCounters + 256;
    // Compute the T^(-1) vector
    uint32_t i = 0;
    do
    {
        tt[charCounters[tt[i] & 0xFF]++] |= (i << 8);
    }
    while(++i < blockSize);
}

uint32_t RCBZip2Decoder::DecodeBlock2(const uint32_t* tt, uint32_t blockSize, uint32_t origPtr, RCOutBuffer& outStream)
{
    RCBZip2CRC crc;
    // it's for speed optimization: prefetch & prevByte_init;
    uint32_t tPos = tt[tt[origPtr] >> 8];
    uint32_t prevByte = (uint32_t)(tPos & 0xFF);
    int32_t numReps = 0 ;
    do
    {
        uint32_t b = (uint32_t)(tPos & 0xFF);
        tPos = tt[tPos >> 8];
        if (numReps == RCBZip2Defs::s_kRleModeRepSize)
        {
            for (; b > 0; b--)
            {
                crc.UpdateByte(prevByte);
                outStream.WriteByte((byte_t)prevByte);
            }
            numReps = 0;
            continue;
        }
        if (b != prevByte)
        {
            numReps = 0;
        }
        numReps++;
        prevByte = b;
        crc.UpdateByte(b);
        outStream.WriteByte((byte_t)b);
    }
    while(--blockSize != 0);
    return crc.GetDigest();
}

uint32_t RCBZip2Decoder::DecodeBlock2Rand(const uint32_t* tt, uint32_t blockSize, uint32_t origPtr, RCOutBuffer& outStream)
{
    RCBZip2CRC crc;  
    uint32_t randIndex = 1;
    uint32_t randToGo = s_kBZip2DecoderRandNums[0] - 2;

    int32_t numReps = 0 ;

    // it's for speed optimization: prefetch & prevByte_init;
    uint32_t tPos = tt[tt[origPtr] >> 8];
    uint32_t prevByte = (uint32_t)(tPos & 0xFF);

    do
    {
        uint32_t b = (uint32_t)(tPos & 0xFF);
        tPos = tt[tPos >> 8];
        {
            if (randToGo == 0)
            {
                b ^= 1;
                randToGo = s_kBZip2DecoderRandNums[randIndex++];
                randIndex &= 0x1FF;
            }
            randToGo--;
        }

        if (numReps == RCBZip2Defs::s_kRleModeRepSize)
        {
            for (; b > 0; b--)
            {
                crc.UpdateByte(prevByte);
                outStream.WriteByte((byte_t)prevByte);
            }
            numReps = 0;
            continue;
        }
        if (b != prevByte)
        {
            numReps = 0;
        }
        numReps++;
        prevByte = b;
        crc.UpdateByte(b);
        outStream.WriteByte((byte_t)b);
    }
    while(--blockSize != 0);
    return crc.GetDigest();
}

HResult RCBZip2Decoder::CodeResume(ISequentialOutStream* outStream, bool& isBZ, ICompressProgressInfo* progress)
{
    try
    {
        return CodeReal(NULL, outStream, isBZ, progress) ;
    }
    catch(HResult errorCode)
    {
        return errorCode ;
    }
    catch(...)
    {
        return RC_E_FAIL ;
    }
}

HResult RCBZip2Decoder::SetInStream(ISequentialInStream* inStream)
{
    m_inStream.SetStream(inStream); 
    return RC_S_OK;
}

HResult RCBZip2Decoder::ReleaseInStream()
{
    m_inStream.ReleaseStream() ;
    return RC_S_OK ;
}

uint64_t RCBZip2Decoder::GetInputProcessedSize()
{
    return m_inStream.GetProcessedSize() ;
}

HResult RCBZip2Decoder::SetRatioProgress(uint64_t packSize)
{
#ifdef COMPRESS_BZIP2_MT
    if (!m_progress)
    {
        return RC_S_OK ;
    }
    packSize -= m_inStart;
    uint64_t unpackSize = m_outStream.GetProcessedSize() ;
    return m_progress->SetRatioInfo(packSize, unpackSize) ;
#else
    return RC_S_OK ;
#endif
}

#ifdef COMPRESS_BZIP2_MT

HResult RCBZip2Decoder::Create()
{
    WRes res = m_canProcessEvent.CreateIfNotCreated() ;
    if(res != 0)
    {
        return res ;
    }
    res = m_canStartWaitingEvent.CreateIfNotCreated() ;
    if(res != 0)
    {
        return res ;
    }
    if (m_states != 0 && m_numThreadsPrev == m_numThreads)
    {
        return RC_S_OK ;
    }
    Free();
    m_isMtMode = (m_numThreads > 1);
    m_numThreadsPrev = m_numThreads;
    try
    {
        m_states = new RCBZip2DecoderState[m_numThreads];
        if (m_states == 0)
        {
            return RC_E_OUTOFMEMORY ;
        }
    }
    catch(...)
    {
        return RC_E_OUTOFMEMORY;
    }
    for (uint32_t t = 0; t < m_numThreads; t++)
    {
        RCBZip2DecoderState& ti = m_states[t];
        ti.m_decoder = this;
        if (m_isMtMode)
        {
            HResult res = ti.Create();
            if (res != RC_S_OK)
            {
                m_numThreads = t;
                Free();
                return res;
            }
        }
    }
    return RC_S_OK ;
}

void RCBZip2Decoder::Free()
{
    if (!m_states)
    {
        return;
    }
    m_isCloseThreads = true;
    m_canProcessEvent.Set();
    for (uint32_t t = 0; t < m_numThreads; t++)
    {
        RCBZip2DecoderState& s = m_states[t] ;
        if (m_isMtMode && (s.m_spThread != NULL) && s.m_spThread->IsRunning() )
        {
            s.m_spThread->Join() ;
            s.m_spThread.reset() ;
        }
        s.Free();
    }
    delete [] m_states ;
    m_states = 0 ;
}

HResult RCBZip2Decoder::SetNumberOfThreads(uint32_t numThreads)
{
    m_numThreads = numThreads;
    if (m_numThreads < 1)
    {
        m_numThreads = 1;
    }
    if (m_numThreads > BZIP2_MAX_THREAD_NIM)
    {
        m_numThreads = BZIP2_MAX_THREAD_NIM ;
    }
    return RC_S_OK ;
}

#endif

HResult RCBZip2Decoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& pair = *pos ;
        if(pair.first == RCMethodProperty::ID_SET_ISZ_BIZP2)
        {
            if(IsBooleanType(pair.second))
            {
                m_isIszBZip2 = GetBooleanValue(pair.second) ;
            }
            else
            {
                return RC_E_INVALIDARG ;
            }
        }
        else
        {
            return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
