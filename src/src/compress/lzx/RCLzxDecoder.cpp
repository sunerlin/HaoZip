/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzx/RCLzxDecoder.h"
#include "interface/RCMethodDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCLzxDecoderFlusher
{
private:
    RCLzxDecoder* m_Decoder;
public:
    bool NeedFlush ;
    RCLzxDecoderFlusher(RCLzxDecoder *decoder): m_Decoder(decoder), NeedFlush(true) {}
    ~RCLzxDecoderFlusher()
    {
        if (NeedFlush)
        {
            m_Decoder->Flush();
        }
        m_Decoder->ReleaseStreams();
    }
};

#define LEN_ID_NEED_INIT (-2) 

/////////////////////////////////////////////////////////////////
//RCLzxDecoder class implementation

RCLzxDecoder::RCLzxDecoder(bool wimMode):
    m_numPosLenSlots(0),
    m_isUncompressedBlock(false),
    m_alignIsUsed(false),
    m_x86ConvertOutStreamSpec(NULL),
    m_unCompressedBlockSize(0),
    m_keepHistory(false),
    m_remainLen(0),
    m_skipByte(false),
    m_wimMode(wimMode)
{
    m_x86ConvertOutStreamSpec = new RCLzxX86ConvertOutStream ;
    m_x86ConvertOutStream = m_x86ConvertOutStreamSpec ;
}

RCLzxDecoder::~RCLzxDecoder()
{
}

uint32_t RCLzxDecoder::ReadBits(int32_t numBits)
{
    return m_inBitStream.ReadBits(numBits);
}

bool RCLzxDecoder::ReadTable(byte_t* lastLevels, byte_t* newLevels, uint32_t numSymbols)
{
    byte_t levelLevels[RCLzxDefs::s_kLevelTableSize];
    uint32_t i = 0 ;
    for (i = 0; i < RCLzxDefs::s_kLevelTableSize; i++)
    {
        levelLevels[i] = (byte_t)ReadBits(RCLzxDefs::s_kNumBitsForPreTreeLevel);
    }
    if(!m_levelDecoder.SetCodeLengths(levelLevels))
    {
        return false ;
    }
    int32_t num = 0;
    byte_t symbol = 0;
    for (i = 0; i < numSymbols;)
    {
        if (num != 0)
        {
            lastLevels[i] = newLevels[i] = symbol;
            i++;
            num--;
            continue;
        }
        uint32_t number = m_levelDecoder.DecodeSymbol(&m_inBitStream);
        if (number == RCLzxDefs::s_kLevelSymbolZeros)
        {
            num = RCLzxDefs::s_kLevelSymbolZerosStartValue + (int32_t)ReadBits(RCLzxDefs::s_kLevelSymbolZerosNumBits);
            symbol = 0;
        }
        else if (number == RCLzxDefs::s_kLevelSymbolZerosBig)
        {
            num = RCLzxDefs::s_kLevelSymbolZerosBigStartValue + (int32_t)ReadBits(RCLzxDefs::s_kLevelSymbolZerosBigNumBits);
            symbol = 0;
        }
        else if (number == RCLzxDefs::s_kLevelSymbolSame || number <= RCLzxDefs::s_kNumHuffmanBits)
        {
            if (number <= RCLzxDefs::s_kNumHuffmanBits)
            {
                num = 1;
            }
            else
            {
                num = RCLzxDefs::s_kLevelSymbolSameStartValue + (int32_t)ReadBits(RCLzxDefs::s_kLevelSymbolSameNumBits);
                number = m_levelDecoder.DecodeSymbol(&m_inBitStream);
                if (number > RCLzxDefs::s_kNumHuffmanBits)
                {
                    return false;
                }
            }
            symbol = byte_t((17 + lastLevels[i] - number) % (RCLzxDefs::s_kNumHuffmanBits + 1));
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool RCLzxDecoder::ReadTables(void)
{
    byte_t newLevels[RCLzxDefs::s_kMaxTableSize];
    {
        if (m_skipByte)
        {
            m_inBitStream.DirectReadByte();
        }
        m_inBitStream.Normalize();

        int32_t blockType = (int32_t)ReadBits(RCLzxDefs::s_kNumBlockTypeBits);
        if (blockType > RCLzxDefs::s_kBlockTypeUncompressed)
        {
            return false;
        }
        if (m_wimMode)
        {
            if (ReadBits(1) == 1)
            {
                m_unCompressedBlockSize = (1 << 15);
            }
            else
            {
                m_unCompressedBlockSize = ReadBits(16);
            }
        }
        else
        {
            m_unCompressedBlockSize = m_inBitStream.ReadBitsBig(RCLzxDefs::s_kUncompressedBlockSizeNumBits);
        }
        m_isUncompressedBlock = (blockType == RCLzxDefs::s_kBlockTypeUncompressed);
        m_skipByte = (m_isUncompressedBlock && ((m_unCompressedBlockSize & 1) != 0));

        if (m_isUncompressedBlock)
        {
            ReadBits(16 - m_inBitStream.GetBitPosition());
            if (!m_inBitStream.ReadUInt32(m_repDistances[0]))
            {
                return false;
            }
            m_repDistances[0]--;
            for (int32_t i = 1; i < RCLzxDefs::s_kNumRepDistances; i++)
            {
                uint32_t rep = 0;
                for (int32_t j = 0; j < 4; j++)
                {
                    rep |= (uint32_t)m_inBitStream.DirectReadByte() << (8 * j);
                }
                m_repDistances[i] = rep - 1;
            }
            return true;
        }
        m_alignIsUsed = (blockType == RCLzxDefs::s_kBlockTypeAligned);
        if (m_alignIsUsed)
        {
            for(int32_t i = 0; i < RCLzxDefs::s_kAlignTableSize; i++)
            {
                newLevels[i] = (byte_t)ReadBits(RCLzxDefs::s_kNumBitsForAlignLevel);
            }
            if(!m_alignDecoder.SetCodeLengths(newLevels))
            {
                return false ;
            }
        }
    }

    if(!ReadTable(m_lastMainLevels, newLevels, 256))
    {
        return false ;
    }
    if(!ReadTable(m_lastMainLevels + 256, newLevels + 256, m_numPosLenSlots))
    {
        return false ;
    }
    for (uint32_t i = 256 + m_numPosLenSlots; i < RCLzxDefs::s_kMainTableSize; i++)
    {
        newLevels[i] = 0;
    }
    if(!m_mainDecoder.SetCodeLengths(newLevels))
    {
        return false ;
    }

    if(!ReadTable(m_lastLenLevels, newLevels, RCLzxDefs::s_kNumLenSymbols))
    {
        return false ;
    }
    return m_lenDecoder.SetCodeLengths(newLevels);
}

void RCLzxDecoder::ClearPrevLevels()
{
    int32_t i = 0 ;
    for (i = 0; i < RCLzxDefs::s_kMainTableSize; i++)
    {
        m_lastMainLevels[i] = 0;
    }
    for (i = 0; i < RCLzxDefs::s_kNumLenSymbols; i++)
    {
        m_lastLenLevels[i] = 0;
    }
}

HResult RCLzxDecoder::CodeSpec(uint32_t curSize)
{
    if (m_remainLen == LEN_ID_NEED_INIT)
    {
        m_remainLen = 0;
        m_inBitStream.Init();
        if (!m_keepHistory || !m_isUncompressedBlock)
        {
            m_inBitStream.Normalize();
        }
        if (!m_keepHistory)
        {
            m_skipByte = false;
            m_unCompressedBlockSize = 0;
            ClearPrevLevels();
            uint32_t i86TranslationSize = 12000000;
            bool translationMode = true;
            if (!m_wimMode)
            {
                translationMode = (ReadBits(1) != 0);
                if (translationMode)
                {
                    i86TranslationSize = ReadBits(16) << 16;
                    i86TranslationSize |= ReadBits(16);
                }
            }
            m_x86ConvertOutStreamSpec->Init(translationMode, i86TranslationSize);

            for(int i = 0 ; i < RCLzxDefs::s_kNumRepDistances; i++)
            {
                 m_repDistances[i] = 0;
            }
        }
    }

    while(m_remainLen > 0 && curSize > 0)
    {
        m_outWindowStream.PutByte(m_outWindowStream.GetByte(m_repDistances[0]));
        m_remainLen--;
        curSize--;
    }

    while(curSize > 0)
    {
        if (m_unCompressedBlockSize == 0)
        {
            if (!ReadTables())
            {
                return RC_S_FALSE ;
            }
        }
        uint32_t next = (int32_t)std::min(m_unCompressedBlockSize, curSize);
        curSize -= next;
        m_unCompressedBlockSize -= next;
        if (m_isUncompressedBlock)
        {
            while(next > 0)
            {
                m_outWindowStream.PutByte(m_inBitStream.DirectReadByte());
                next--;
            }
        }
        else while(next > 0)
        {
            uint32_t number = m_mainDecoder.DecodeSymbol(&m_inBitStream);
            if (number < 256)
            {
                m_outWindowStream.PutByte((byte_t)number);
                next--;
            }
            else
            {
                uint32_t posLenSlot = number - 256;
                if (posLenSlot >= m_numPosLenSlots)
                {
                    return RC_S_FALSE;
                }
                uint32_t posSlot = posLenSlot / RCLzxDefs::s_kNumLenSlots;
                uint32_t lenSlot = posLenSlot % RCLzxDefs::s_kNumLenSlots;
                uint32_t len = RCLzxDefs::s_kMatchMinLen + lenSlot;
                if (lenSlot == RCLzxDefs::s_kNumLenSlots - 1)
                {
                    uint32_t lenTemp = m_lenDecoder.DecodeSymbol(&m_inBitStream);
                    if (lenTemp >= RCLzxDefs::s_kNumLenSymbols)
                    {
                        return RC_S_FALSE;
                    }
                    len += lenTemp;
                }

                if (posSlot < RCLzxDefs::s_kNumRepDistances)
                {
                    uint32_t distance = m_repDistances[posSlot];
                    m_repDistances[posSlot] = m_repDistances[0];
                    m_repDistances[0] = distance;
                }
                else
                {
                    uint32_t distance = 0 ;
                    int32_t numDirectBits = 0 ;
                    if (posSlot < RCLzxDefs::s_kNumPowerPosSlots)
                    {
                        numDirectBits = (int32_t)(posSlot >> 1) - 1;
                        distance = ((2 | (posSlot & 1)) << numDirectBits);
                    }
                    else
                    {
                        numDirectBits = RCLzxDefs::s_kNumLinearPosSlotBits;
                        distance = ((posSlot - 0x22) << RCLzxDefs::s_kNumLinearPosSlotBits);
                    }

                    if (m_alignIsUsed && numDirectBits >= RCLzxDefs::s_kNumAlignBits)
                    {
                        distance += (m_inBitStream.ReadBits(numDirectBits - RCLzxDefs::s_kNumAlignBits) << RCLzxDefs::s_kNumAlignBits);
                        uint32_t alignTemp = m_alignDecoder.DecodeSymbol(&m_inBitStream);
                        if (alignTemp >= RCLzxDefs::s_kAlignTableSize)
                        {
                            return RC_S_FALSE;
                        }
                        distance += alignTemp;
                    }
                    else
                    {
                        distance += m_inBitStream.ReadBits(numDirectBits);
                    }
                    m_repDistances[2] = m_repDistances[1];
                    m_repDistances[1] = m_repDistances[0];
                    m_repDistances[0] = distance - RCLzxDefs::s_kNumRepDistances;
                }

                uint32_t locLen = len;
                if (locLen > next)
                {
                    locLen = next;
                }

                if (!m_outWindowStream.CopyBlock(m_repDistances[0], locLen))
                {
                    return RC_S_FALSE;
                }

                len -= locLen;
                next -= locLen;
                if (len != 0)
                {
                    m_remainLen = (int32_t)len;
                    return RC_S_OK;
                }
            }
        }
    }
    return RC_S_OK;
}

void RCLzxDecoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    m_inBitStream.ReleaseStream();
    if(m_x86ConvertOutStreamSpec)
    {
        m_x86ConvertOutStreamSpec->ReleaseStream();
    }
}

HResult RCLzxDecoder::SetParams(int32_t numDictBits)
{
    if (numDictBits < RCLzxDefs::s_kNumDictionaryBitsMin || 
        numDictBits > RCLzxDefs::s_kNumDictionaryBitsMax)
    {
        return RC_E_INVALIDARG;
    }
    uint32_t numPosSlots = 0 ;
    if (numDictBits < 20)
    {
        numPosSlots = 30 + (numDictBits - 15) * 2;
    }
    else if (numDictBits == 20)
    {
        numPosSlots = 42;
    }
    else
    {
        numPosSlots = 50;
    }
    m_numPosLenSlots = numPosSlots * RCLzxDefs::s_kNumLenSlots;
    if (!m_outWindowStream.Create(RCLzxDefs::s_kDictionarySizeMax))
    {
        return RC_E_OUTOFMEMORY;
    }
    if (!m_inBitStream.Create(1 << 16))
    {
        return RC_E_OUTOFMEMORY;
    }
    return RC_S_OK;
}

void RCLzxDecoder::SetKeepHistory(bool keepHistory)
{
    m_keepHistory = keepHistory;
}

HResult RCLzxDecoder::Code(ISequentialInStream* inStream,
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

HResult RCLzxDecoder::CodeReal( ISequentialInStream* inStream,
                                ISequentialOutStream* outStream, 
                                const uint64_t* inSize, 
                                const uint64_t* outSize,
                                ICompressProgressInfo* progress) 
{
    if (outSize == NULL)
    {
        return RC_E_INVALIDARG;
    }
    uint64_t size = *outSize;

    HResult hr = SetInStream(inStream) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    m_x86ConvertOutStreamSpec->SetStream(outStream);
    m_outWindowStream.SetStream(m_x86ConvertOutStream.Get());
    hr = SetOutStreamSize(outSize) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    
    RCLzxDecoderFlusher flusher(this);

    const uint64_t start = m_outWindowStream.GetProcessedSize();
    for (;;)
    {
        uint32_t curSize = 1 << 18;
        uint64_t rem = size - (m_outWindowStream.GetProcessedSize() - start);
        if (curSize > rem)
        {
            curSize = (uint32_t)rem;
        }
        if (curSize == 0)
        {
            break;
        }
        hr = CodeSpec(curSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (progress != NULL)
        {
            uint64_t inSize = m_inBitStream.GetProcessedSize();
            uint64_t nowPos64 = m_outWindowStream.GetProcessedSize() - start;
            hr = progress->SetRatioInfo(inSize, nowPos64) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    flusher.NeedFlush = false;
    return Flush() ;
}

HResult RCLzxDecoder::Flush()
{
    HResult hr = m_outWindowStream.Flush() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return m_x86ConvertOutStreamSpec->Flush();
}

HResult RCLzxDecoder::SetInStream(ISequentialInStream *inStream)
{
    m_inBitStream.SetStream(inStream);
    return RC_S_OK ;
}

HResult RCLzxDecoder::ReleaseInStream()
{
    m_inBitStream.ReleaseStream();
    return RC_S_OK ;
}

HResult RCLzxDecoder::SetOutStreamSize(const uint64_t *outSize)
{
    if (outSize == NULL)
    {
        return RC_E_FAIL;
    }
    m_remainLen = LEN_ID_NEED_INIT;
    m_outWindowStream.Init(m_keepHistory);
    return RC_S_OK ;
}

HResult RCLzxDecoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& pair = *pos ;
        if(pair.first == RCMethodProperty::ID_SET_PARAMS)
        {
            if(IsInteger64Type(pair.second))
            {
                int32_t numDictBits = static_cast<int32_t>(GetInteger64Value(pair.second)) ;
                HResult hr = SetParams(numDictBits) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
            }
            else
            {
                return RC_E_INVALIDARG ;
            }
        }
        else if(pair.first == RCMethodProperty::ID_SET_KEEP_HISTORY)
        {
            if(IsBooleanType(pair.second))
            {
                SetKeepHistory(GetBooleanValue(pair.second)) ;
            }
            else
            {
                return RC_E_INVALIDARG ;
            }
        }
        else if(pair.first == RCMethodProperty::ID_SET_LZX_WIM_MODE)
        {
            if(IsBooleanType(pair.second))
            {
                m_wimMode = GetBooleanValue(pair.second) ;
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
