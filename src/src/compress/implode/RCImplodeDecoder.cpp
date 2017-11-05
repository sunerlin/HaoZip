/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/implode/RCImplodeDecoder.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////
//RCImplodeDecoder class implementation

BEGIN_NAMESPACE_RCZIP

static const int kNumDistanceLowDirectBitsForBigDict = 7;
static const int kNumDistanceLowDirectBitsForSmallDict = 6;
static const int kNumBitsInByte = 8;
static const int kLevelStructuresNumberAdditionalValue = 1;
static const int kNumLevelStructureLevelBits = 4;
static const int kLevelStructureLevelAdditionalValue = 1;
static const int kNumLevelStructureRepNumberBits = 4;
static const int kLevelStructureRepNumberAdditionalValue = 1;
static const int kLiteralTableSize = (1 << kNumBitsInByte);
static const int kDistanceTableSize = 64;
static const int kLengthTableSize = 64;
static const uint32_t kHistorySize = (1 << std::max(kNumDistanceLowDirectBitsForBigDict,
                                                    kNumDistanceLowDirectBitsForSmallDict)) * kDistanceTableSize; // = 8 KB;
static const int kNumAdditionalLengthBits = 8;
static const uint32_t kMatchMinLenWhenLiteralsOn = 3;
static const uint32_t kMatchMinLenWhenLiteralsOff = 2;
static const uint32_t kMatchMinLenMax = std::max(kMatchMinLenWhenLiteralsOn,
                                                 kMatchMinLenWhenLiteralsOff);  // 3
enum
{
    kMatchId = 0
};

class RCImplodeCoderReleaser
{
public:
    RCImplodeCoderReleaser(RCImplodeDecoder* coder):
        m_Coder(coder)
    {
    }
    ~RCImplodeCoderReleaser()
    {
        m_Coder->ReleaseStreams();
    }
    
private:
    RCImplodeDecoder* m_Coder ;
};

RCImplodeDecoder::RCImplodeDecoder():
    m_literalDecoder(kLiteralTableSize),
    m_lengthDecoder(kLengthTableSize),
    m_distanceDecoder(kDistanceTableSize),
    m_bigDictionaryOn(false),
    m_literalsOn(false),
    m_numDistanceLowDirectBits(0),
    m_minMatchLength(0)
{
}

RCImplodeDecoder::~RCImplodeDecoder()
{
}

bool RCImplodeDecoder::ReadLevelItems(RCImplodeHuffmanDecoder& decoder, byte_t* levels, int32_t numLevelItems)
{
    int32_t numCodedStructures = m_inBitStream.ReadBits(kNumBitsInByte) +
                                 kLevelStructuresNumberAdditionalValue ;
    int32_t currentIndex = 0 ;
    for(int32_t i = 0; i < numCodedStructures; i++)
    {
        int32_t level = m_inBitStream.ReadBits(kNumLevelStructureLevelBits) +
                        kLevelStructureLevelAdditionalValue;
        int32_t rep = m_inBitStream.ReadBits(kNumLevelStructureRepNumberBits) +
                      kLevelStructureRepNumberAdditionalValue;
        if (currentIndex + rep > numLevelItems)
        {
            _ThrowCode(RC_E_ReadStreamError) ;
        }
        for(int32_t j = 0; j < rep; j++)
        {
            levels[currentIndex++] = (byte_t)level;
        }
    }
    if (currentIndex != numLevelItems)
    {
        return false;
    }
    return decoder.SetCodeLengths(levels);
}

bool RCImplodeDecoder::ReadTables()
{
    if (m_literalsOn)
    {
        byte_t literalLevels[kLiteralTableSize];
        if (!ReadLevelItems(m_literalDecoder, literalLevels, kLiteralTableSize))
        {
            return false;
        }
    }
    byte_t lengthLevels[kLengthTableSize];
    if (!ReadLevelItems(m_lengthDecoder, lengthLevels, kLengthTableSize))
    {
        return false;
    }
    byte_t distanceLevels[kDistanceTableSize];
    return ReadLevelItems(m_distanceDecoder, distanceLevels, kDistanceTableSize);
}

HResult RCImplodeDecoder::Code(ISequentialInStream* inStream,
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

HResult RCImplodeDecoder::CodeReal(ISequentialInStream* inStream,
                                   ISequentialOutStream* outStream, 
                                   const uint64_t* inSize, 
                                   const uint64_t* outSize,
                                   ICompressProgressInfo* progress)
{
    if (!m_inBitStream.Create(1 << 20))
    {
        return RC_E_OUTOFMEMORY ;
    }
    if (!m_outWindowStream.Create(kHistorySize))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (outSize == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    uint64_t pos = 0 ;
    uint64_t unPackSize = *outSize ;

    m_outWindowStream.SetStream(outStream);
    m_outWindowStream.Init(false);
    m_inBitStream.SetStream(inStream);
    m_inBitStream.Init();
    RCImplodeCoderReleaser coderReleaser(this);

    if (!ReadTables())
    {
        return RC_S_FALSE ;
    }

    while(pos < unPackSize)
    {
        if (progress != NULL && pos % (1 << 16) == 0)
        {
            uint64_t packSize = m_inBitStream.GetProcessedSize();
            HResult hr = progress->SetRatioInfo(packSize, pos) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        if(m_inBitStream.ReadBits(1) == kMatchId) // match
        {
            uint32_t lowDistBits = m_inBitStream.ReadBits(m_numDistanceLowDirectBits);
            uint32_t distance = m_distanceDecoder.DecodeSymbol(&m_inBitStream);
            if (distance >= kDistanceTableSize)
            {
                return RC_S_FALSE;
            }
            distance = (distance << m_numDistanceLowDirectBits) + lowDistBits;
            uint32_t lengthSymbol = m_lengthDecoder.DecodeSymbol(&m_inBitStream);
            if (lengthSymbol >= kLengthTableSize)
            {
                return RC_S_FALSE;
            }
            uint32_t length = lengthSymbol + m_minMatchLength;
            if (lengthSymbol == kLengthTableSize - 1) // special symbol  = 63
            {
                length += m_inBitStream.ReadBits(kNumAdditionalLengthBits);
            }
            while(distance >= pos && length > 0)
            {
                m_outWindowStream.PutByte(0);
                pos++;
                length--;
            }
            if (length > 0)
            {
                m_outWindowStream.CopyBlock(distance, length);
            }
            pos += length;
        }
        else
        {
            byte_t b;
            if (m_literalsOn)
            {
                uint32_t temp = m_literalDecoder.DecodeSymbol(&m_inBitStream);
                if (temp >= kLiteralTableSize)
                {
                    return RC_S_FALSE;
                }
                b = (byte_t)temp;
            }
            else
            {
                b = (byte_t)m_inBitStream.ReadBits(kNumBitsInByte);
            }
            m_outWindowStream.PutByte(b);
            pos++;
        }
    }
    if (pos > unPackSize)
    {
        return RC_S_FALSE;
    }
    return m_outWindowStream.Flush();
}

HResult RCImplodeDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 1)
    {
        return RC_E_INVALIDARG;
    }
    byte_t flag = data[0];
    m_bigDictionaryOn = ((flag & 2) != 0);
    m_numDistanceLowDirectBits = m_bigDictionaryOn ?
                                                    kNumDistanceLowDirectBitsForBigDict:
                                                    kNumDistanceLowDirectBitsForSmallDict ;
    m_literalsOn = ((flag & 4) != 0);
    m_minMatchLength = m_literalsOn ?
                                    kMatchMinLenWhenLiteralsOn :
                                    kMatchMinLenWhenLiteralsOff;
    return RC_S_OK ;
}

HResult RCImplodeDecoder::Flush()
{
    return m_outWindowStream.Flush() ;
}

void RCImplodeDecoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
}

END_NAMESPACE_RCZIP
