/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar1Decoder.h"

/////////////////////////////////////////////////////////////////
//RCRar1Decoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCRar1DecoderReleaser
{
public:
    RCRar1DecoderReleaser(RCRar1Decoder *coder):
        m_coder(coder)
    {
    }
    ~RCRar1DecoderReleaser()
    {
        m_coder->ReleaseStreams();
    }

private:
    RCRar1Decoder* m_coder;
};

static const uint32_t s_kRar1HistorySize = (1 << 16) ;

static const uint32_t s_kRar1PosL1[]={0,0,0,2,3,5,7,11,16,20,24,32,32, 256};
static const uint32_t s_kRar1PosL2[]={0,0,0,0,5,7,9,13,18,22,26,34,36, 256};
static const uint32_t s_kRar1PosHf0[]={0,0,0,0,0,8,16,24,33,33,33,33,33, 257};
static const uint32_t s_kRar1PosHf1[]={0,0,0,0,0,0,4,44,60,76,80,80,127, 257};
static const uint32_t s_kRar1PosHf2[]={0,0,0,0,0,0,2,7,53,117,233, 257,0};
static const uint32_t s_kRar1PosHf3[]={0,0,0,0,0,0,0,2,16,218,251, 257,0};
static const uint32_t s_kRar1PosHf4[]={0,0,0,0,0,0,0,0,0,255, 257,0,0};

    
static const byte_t s_kRar1ShortLen1[]  = {1,3,4,4,5,6,7,8,8,4,4,5,6,6 };
static const byte_t s_kRar1ShortLen1a[] = {1,4,4,4,5,6,7,8,8,4,4,5,6,6,4 };
static const byte_t s_kRar1ShortLen2[]  = {2,3,3,3,4,4,5,6,6,4,4,5,6,6 };
static const byte_t s_kRar1ShortLen2a[] = {2,3,3,4,4,4,5,6,6,4,4,5,6,6,4 };
static const uint32_t s_kRar1ShortXor1[] = {0,0xa0,0xd0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff,0xc0,0x80,0x90,0x98,0x9c,0xb0};
static const uint32_t s_kRar1ShortXor2[] = {0,0x40,0x60,0xa0,0xd0,0xe0,0xf0,0xf8,0xfc,0xc0,0x80,0x90,0x98,0x9c,0xb0};

RCRar1Decoder::RCRar1Decoder():
    m_lastDist(0),
    m_lastLength(0),
    m_unpackSize(0),
    m_isSolid(false),
    m_flagBuf(0),
    m_avrPlc(0),
    m_avrPlcB(0),
    m_avrLn1(0),
    m_avrLn2(0),
    m_avrLn3(0),
    m_buf60(0),
    m_numHuf(0),
    m_stMode(0),
    m_lCount(0),
    m_flagsCnt(0),
    m_nhfb(0),
    m_nlzb(0),
    m_maxDist3(0)
{
}

RCRar1Decoder::~RCRar1Decoder()
{
}

uint32_t RCRar1Decoder::ReadBits(int32_t numBits)
{
    return m_inBitStream.ReadBits(numBits) ;
}

HResult RCRar1Decoder::CopyBlock(uint32_t distance, uint32_t len)
{
    m_unpackSize -= len;
    return m_outWindowStream.CopyBlock(distance, len) ? RC_S_OK : RC_S_FALSE;
}

uint32_t RCRar1Decoder::DecodeNum(const uint32_t* posTab)
{
    uint32_t startPos = 2;
    uint32_t num = m_inBitStream.GetValue(12);
    for (;;)
    {
        uint32_t cur = (posTab[startPos + 1] - posTab[startPos]) << (12 - startPos);
        if (num < cur)
        {
            break;
        }
        startPos++;
        num -= cur;
    }
    m_inBitStream.MovePos(startPos);
    return((num >> (12 - startPos)) + posTab[startPos]);
}

HResult RCRar1Decoder::ShortLZ()
{
    uint32_t len = 0 ;
    uint32_t saveLen = 0 ;
    uint32_t dist = 0 ;
    int32_t distancePlace = 0 ;
    const byte_t* kShortLen = NULL ;
    const uint32_t* kShortXor = NULL ;
    m_numHuf = 0 ;

    if(m_lCount == 2)
    {
        if (ReadBits(1))
        {
            return CopyBlock(m_lastDist, m_lastLength) ;
        }
        m_lCount = 0;
    }

    uint32_t bitField = m_inBitStream.GetValue(8) ;

    if (m_avrLn1 < 37)
    {
        kShortLen = m_buf60 ? s_kRar1ShortLen1a : s_kRar1ShortLen1;
        kShortXor = s_kRar1ShortXor1;
    }
    else
    {
        kShortLen = m_buf60 ? s_kRar1ShortLen2a : s_kRar1ShortLen2;
        kShortXor = s_kRar1ShortXor2;
    }

    for (len = 0; ((bitField ^ kShortXor[len]) & (~(0xff >> kShortLen[len]))) != 0; len++)
    {
    }
    m_inBitStream.MovePos(kShortLen[len]) ;

    if (len >= 9)
    {
        if (len == 9)
        {
            m_lCount++;
            return CopyBlock(m_lastDist, m_lastLength);
        }
        if (len == 14)
        {
            m_lCount = 0;
            len = DecodeNum(s_kRar1PosL2) + 5;
            dist = 0x8000 + ReadBits(15) - 1;
            m_lastLength = len;
            m_lastDist = dist;
            return CopyBlock(dist, len);
        }

        m_lCount = 0;
        saveLen = len;
        dist = m_repDists[(m_repDistPtr - (len - 9)) & 3];
        len = DecodeNum(s_kRar1PosL1) + 2;
        if (len == 0x101 && saveLen == 10)
        {
            m_buf60 ^= 1;
            return RC_S_OK;
        }
        if (dist >= 256)
        {
            len++;
        }
        if (dist >= m_maxDist3 - 1)
        {
            len++;
        }
    }
    else
    {
        m_lCount = 0;
        m_avrLn1 += len;
        m_avrLn1 -= m_avrLn1 >> 4;

        distancePlace = DecodeNum(s_kRar1PosHf2) & 0xff;
        dist = m_chSetA[distancePlace];
        if (--distancePlace != -1)
        {
            m_placeA[dist]--;
            uint32_t lastDistance = m_chSetA[distancePlace];
            m_placeA[lastDistance]++;
            m_chSetA[distancePlace + 1] = lastDistance;
            m_chSetA[distancePlace] = dist;
        }
        len += 2;
    }
    m_repDists[m_repDistPtr++] = dist;
    m_repDistPtr &= 3;
    m_lastLength = len;
    m_lastDist = dist;
    return CopyBlock(dist, len);
}

HResult RCRar1Decoder::LongLZ()
{
    uint32_t len = 0 ;
    uint32_t dist = 0 ;
    uint32_t distancePlace = 0 ;
    uint32_t newDistancePlace = 0 ;
    uint32_t oldAvr2 = 0 ;
    uint32_t oldAvr3 = 0 ;

    m_numHuf = 0;
    m_nlzb += 16;
    if (m_nlzb > 0xff)
    {
        m_nlzb = 0x90;
        m_nhfb >>= 1;
    }
    oldAvr2=m_avrLn2;

    if (m_avrLn2 >= 122)
    {
        len = DecodeNum(s_kRar1PosL2);
    }
    else if (m_avrLn2 >= 64)
    {
        len = DecodeNum(s_kRar1PosL1);
    }
    else
    {
        uint32_t bitField = m_inBitStream.GetValue(16);
        if (bitField < 0x100)
        {
            len = bitField;
            m_inBitStream.MovePos(16);
        }
        else
        {
            for (len = 0; ((bitField << len) & 0x8000) == 0; len++)
            {
            }
            m_inBitStream.MovePos(len + 1);
        }
    }

    m_avrLn2 += len;
    m_avrLn2 -= m_avrLn2 >> 5;

    if (m_avrPlcB > 0x28ff)
    {
        distancePlace = DecodeNum(s_kRar1PosHf2);
    }
    else if (m_avrPlcB > 0x6ff)
    {
        distancePlace = DecodeNum(s_kRar1PosHf1);
    }
    else
    {
        distancePlace = DecodeNum(s_kRar1PosHf0);
    }

    m_avrPlcB += distancePlace;
    m_avrPlcB -= m_avrPlcB >> 8;
    for (;;)
    {
        dist = m_chSetB[distancePlace & 0xff];
        newDistancePlace = m_NToPlB[dist++ & 0xff]++;
        if (!(dist & 0xff))
        {
            CorrHuff(m_chSetB,m_NToPlB);
        }
        else
        {
            break;
        }
    }

    m_chSetB[distancePlace] = m_chSetB[newDistancePlace];
    m_chSetB[newDistancePlace] = dist;

    dist = ((dist & 0xff00) >> 1) | ReadBits(7);

    oldAvr3 = m_avrLn3;
    if (len != 1 && len != 4)
    {
        if (len == 0 && dist <= m_maxDist3)
        {
            m_avrLn3++;
            m_avrLn3 -= m_avrLn3 >> 8;
        }
        else
        {
            if (m_avrLn3 > 0)
            {
                m_avrLn3--;
            }
        }
    }
    len += 3;
    if (dist >= m_maxDist3)
    {
        len++;
    }
    if (dist <= 256)
    {
        len += 8;
    }
    if (oldAvr3 > 0xb0 || m_avrPlc >= 0x2a00 && oldAvr2 < 0x40)
    {
        m_maxDist3 = 0x7f00;
    }
    else
    {
        m_maxDist3 = 0x2001;
    }
    m_repDists[m_repDistPtr++] = --dist;
    m_repDistPtr &= 3;
    m_lastLength = len;
    m_lastDist = dist;
    return CopyBlock(dist, len);
}

HResult RCRar1Decoder::HuffDecode()
{
    uint32_t curByte = 0 ;
    uint32_t newBytePlace = 0 ;
    uint32_t len = 0 ;
    uint32_t dist = 0 ;
    int32_t bytePlace = 0 ;

    if(m_avrPlc > 0x75ff)
    {
        bytePlace = DecodeNum(s_kRar1PosHf4);
    }
    else if (m_avrPlc > 0x5dff)
    {
        bytePlace = DecodeNum(s_kRar1PosHf3);
    }
    else if (m_avrPlc > 0x35ff)
    {
        bytePlace = DecodeNum(s_kRar1PosHf2);
    }
    else if (m_avrPlc > 0x0dff)
    {
        bytePlace = DecodeNum(s_kRar1PosHf1);
    }
    else
    {
        bytePlace = DecodeNum(s_kRar1PosHf0);
    }
    if (m_stMode)
    {
        if (--bytePlace == -1)
        {
            if (ReadBits(1))
            {
                m_numHuf = m_stMode = 0;
                return RC_S_OK;
            }
            else
            {
                len = (ReadBits(1)) ? 4 : 3;
                dist = DecodeNum(s_kRar1PosHf2);
                dist = (dist << 5) | ReadBits(5);
                return CopyBlock(dist - 1, len);
            }
        }
    }
    else if (m_numHuf++ >= 16 && m_flagsCnt == 0)
    {
        m_stMode = 1;
    }
    bytePlace &= 0xff;
    m_avrPlc += bytePlace;
    m_avrPlc -= m_avrPlc >> 8;
    m_nhfb+=16;
    if (m_nhfb > 0xff)
    {
        m_nhfb=0x90;
        m_nlzb >>= 1;
    }

    m_unpackSize --;
    m_outWindowStream.PutByte((byte_t)(m_chSet[bytePlace] >> 8));

    for (;;)
    {
        curByte = m_chSet[bytePlace];
        newBytePlace = m_NToPl[curByte++ & 0xff]++;
        if ((curByte & 0xff) > 0xa1)
        {
            CorrHuff(m_chSet, m_NToPl);
        }
        else
        {
            break;
        }
    }

    m_chSet[bytePlace] = m_chSet[newBytePlace];
    m_chSet[newBytePlace] = curByte;
    return RC_S_OK ;
}

void RCRar1Decoder::GetFlagsBuf()
{
    uint32_t flags = 0 ;
    uint32_t newFlagsPlace = 0 ;
    uint32_t flagsPlace = DecodeNum(s_kRar1PosHf2) ;

    for (;;)
    {
        flags = m_chSetC[flagsPlace];
        m_flagBuf = flags >> 8;
        newFlagsPlace = m_NToPlC[flags++ & 0xff]++;
        if ((flags & 0xff) != 0)
        {
            break;
        }
        CorrHuff(m_chSetC, m_NToPlC);
    }

    m_chSetC[flagsPlace] = m_chSetC[newFlagsPlace];
    m_chSetC[newFlagsPlace] = flags;
}

void RCRar1Decoder::InitData()
{
    if(!m_isSolid)
    {
        m_avrPlcB = m_avrLn1 = m_avrLn2 = m_avrLn3 = m_numHuf = m_buf60 = 0;
        m_avrPlc = 0x3500;
        m_maxDist3 = 0x2001;
        m_nhfb = m_nlzb = 0x80;
    }
    m_flagsCnt = 0 ;
    m_flagBuf = 0 ;
    m_stMode = 0 ;
    m_lCount = 0 ;
}

void RCRar1Decoder::InitHuff()
{
    for (uint32_t i = 0; i < 256; i++)
    {
        m_place[i] = m_placeA[i] = m_placeB[i] = i;
        m_placeC[i] = (~i + 1) & 0xff;
        m_chSet[i] = m_chSetB[i] = i << 8;
        m_chSetA[i] = i;
        m_chSetC[i] = ((~i + 1) & 0xff) << 8;
    }
    memset(m_NToPl, 0, sizeof(m_NToPl));
    memset(m_NToPlB, 0, sizeof(m_NToPlB));
    memset(m_NToPlC, 0, sizeof(m_NToPlC));
    CorrHuff(m_chSetB, m_NToPlB);
}

void RCRar1Decoder::CorrHuff(uint32_t* CharSet,uint32_t *NumToPlace)
{
    int32_t i = 0 ;
    for (i = 7; i >= 0; i--)
    {
        for (int32_t j = 0; j < 32; j++, CharSet++)
        {
            *CharSet = (*CharSet & ~0xff) | i;
        }
    }
    memset(NumToPlace, 0, sizeof(m_NToPl));
    for (i = 6; i >= 0; i--)
    {
        NumToPlace[i] = (7 - i) * 32;
    }
}

void RCRar1Decoder::InitStructures()
{
    for(int32_t i = 0; i < s_kNumRepDists; i++)
    {
        m_repDists[i] = 0;
    }
    m_repDistPtr = 0;
    m_lastLength = 0;
    m_lastDist = 0;
}

void RCRar1Decoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

HResult RCRar1Decoder::Code(ISequentialInStream* inStream,
                            ISequentialOutStream* outStream, 
                            const uint64_t* inSize, 
                            const uint64_t* outSize,
                            ICompressProgressInfo* progress)
{
    try
    {
        return CodeReal(inStream, outStream, inSize, outSize, progress);
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

HResult RCRar1Decoder::CodeReal(ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo*)
{
    if (inSize == NULL || outSize == NULL)
    {
        return RC_E_INVALIDARG ;
    }

    if (!m_outWindowStream.Create(s_kRar1HistorySize))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_inBitStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY;
    }

    m_unpackSize = (int64_t)*outSize;
    m_outWindowStream.SetStream(outStream);
    m_outWindowStream.Init(m_isSolid);
    m_inBitStream.SetStream(inStream);
    m_inBitStream.Init();

    RCRar1DecoderReleaser coderReleaser(this);
    InitData();
    if (!m_isSolid)
    {
        InitStructures();
        InitHuff();
    }
    if (m_unpackSize > 0)
    {
        GetFlagsBuf();
        m_flagsCnt = 8;
    }

    while (m_unpackSize > 0)
    {
        if (m_stMode)
        {
            HResult hr = HuffDecode() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            continue;
        }

        if (--m_flagsCnt < 0)
        {
            GetFlagsBuf();
            m_flagsCnt=7;
        }

        if (m_flagBuf & 0x80)
        {
            m_flagBuf <<= 1;
            if (m_nlzb > m_nhfb)
            {
                HResult hr = LongLZ() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            else
            {
                HResult hr = HuffDecode() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
        else
        {
            m_flagBuf <<= 1;
            if (--m_flagsCnt < 0)
            {
                GetFlagsBuf();
                m_flagsCnt = 7;
            }
            if (m_flagBuf & 0x80)
            {
                m_flagBuf <<= 1;
                if (m_nlzb > m_nhfb)
                {
                    HResult hr = HuffDecode() ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                }
                else
                {
                    HResult hr = LongLZ() ;
                    if(!IsSuccess(hr))
                    {
                        return hr ;
                    }
                }
            }
            else
            {
                m_flagBuf <<= 1;
                HResult hr = ShortLZ() ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
        }
    }
    if (m_unpackSize < 0)
    {
        return RC_S_FALSE;
    }
    return m_outWindowStream.Flush();
}

HResult RCRar1Decoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 1)
    {
        return RC_E_INVALIDARG ;
    }
    m_isSolid = (data[0] != 0) ;
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
