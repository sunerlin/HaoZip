/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/deflate/RCDeflateDecoder.h"
#include "compress/deflate/RCDeflateLevels.h"
#include "interface/RCMethodDefs.h"


/////////////////////////////////////////////////////////////////
//RCDeflateDecoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCDeflateDecoderFlusher
{
public:
    RCDeflateDecoderFlusher(RCDeflateDecoder *coder):
        _coder(coder), 
        m_needFlush(true)
    {
    }
    ~RCDeflateDecoderFlusher()
    {
        if (m_needFlush)
        {
            _coder->Flush();
        }
        _coder->ReleaseOutStream();
    }
public:
    bool m_needFlush ;
    
private:
    RCDeflateDecoder* _coder;
};

RCDeflateDecoder::RCDeflateDecoder(bool deflate64Mode, bool deflateNSIS):
    m_zlibMode(0),
    m_storedBlockSize(0),
    m_finalBlock(false),
    m_storedMode(false),
    m_numDistLevels(0),
    m_deflateNSIS(deflateNSIS),    
    m_deflate64Mode(deflate64Mode),
    m_keepHistory(false),
    m_needInitInStream(true),
    m_remainLen(0),
    m_rep0(0),
    m_needReadTable(false)
{
}

RCDeflateDecoder::~RCDeflateDecoder()
{
}

uint32_t RCDeflateDecoder::ReadBits(int32_t numBits)
{
    return m_inBitStream.ReadBits(numBits);
}

bool RCDeflateDecoder::DeCodeLevelTable(byte_t* values, int32_t numSymbols)
{
    int32_t i = 0 ;
    do
    {
        uint32_t number = m_levelDecoder.DecodeSymbol(&m_inBitStream);
        if (number < RCDeflateDefs::s_kTableDirectLevels)
        {
            values[i++] = (byte_t)number;
        }
        else if (number < RCDeflateDefs::s_kLevelTableSize)
        {
            if (number == RCDeflateDefs::s_kTableLevelRepNumber)
            {
                if (i == 0)
                {
                    return false;
                }
                int32_t num = ReadBits(2) + 3;
                for (; num > 0 && i < numSymbols; num--, i++)
                {
                    values[i] = values[i - 1];
                }
            }
            else
            {
                int32_t num = 0 ;
                if (number == RCDeflateDefs::s_kTableLevel0Number)
                {
                    num = ReadBits(3) + 3;
                }
                else
                {
                    num = ReadBits(7) + 11;
                }
                for (;num > 0 && i < numSymbols; num--)
                {
                    values[i++] = 0;
                }
            }
        }
        else
        {
            return false;
        }
    } while(i < numSymbols);
    return true;
}

bool RCDeflateDecoder::ReadTables(void)
{
    m_finalBlock = (ReadBits(RCDeflateDefs::s_kFinalBlockFieldSize) == RCDeflateDefs::s_kFinalBlock);
    uint32_t blockType = ReadBits(RCDeflateDefs::s_kBlockTypeFieldSize);
    if (blockType > RCDeflateDefs::s_kDynamicHuffman)
    {
        return false;
    }

    if (blockType == RCDeflateDefs::s_kStored)
    {
        m_storedMode = true;
        m_inBitStream.AlignToByte() ;
        m_storedBlockSize = ReadBits(RCDeflateDefs::s_kStoredBlockLengthFieldSize);
        if (m_deflateNSIS)
        {
            return true;
        }
        return (m_storedBlockSize == (uint16_t)~ReadBits(RCDeflateDefs::s_kStoredBlockLengthFieldSize));
    }

    m_storedMode = false;

    RCDeflateLevels levels;
    if (blockType == RCDeflateDefs::s_kFixedHuffman)
    {
        levels.SetFixedLevels();
        m_numDistLevels = m_deflate64Mode ? RCDeflateDefs::s_kDistTableSize64 : RCDeflateDefs::s_kDistTableSize32;
    }
    else
    {
        int32_t numLitLenLevels = ReadBits(RCDeflateDefs::s_kNumLenCodesFieldSize) + RCDeflateDefs::s_kNumLitLenCodesMin;
        m_numDistLevels = ReadBits(RCDeflateDefs::s_kNumDistCodesFieldSize) + RCDeflateDefs::s_kNumDistCodesMin;
        int32_t numLevelCodes = ReadBits(RCDeflateDefs::s_kNumLevelCodesFieldSize) + RCDeflateDefs::s_kNumLevelCodesMin;

        if (!m_deflate64Mode)
        {
            if (m_numDistLevels > RCDeflateDefs::s_kDistTableSize32)
            {
                return false;
            }
        }

        byte_t levelLevels[RCDeflateDefs::s_kLevelTableSize];
        for (int32_t i = 0; i < RCDeflateDefs::s_kLevelTableSize; i++)
        {
            int32_t position = s_kDeflateCodeLengthAlphabetOrder[i];
            if(i < numLevelCodes)
            {
                levelLevels[position] = (byte_t)ReadBits(RCDeflateDefs::s_kLevelFieldSize);
            }
            else
            {
                levelLevels[position] = 0;
            }
        }

        if(!m_levelDecoder.SetCodeLengths(levelLevels))
        {
            return false ;
        }

        byte_t tmpLevels[RCDeflateDefs::s_kFixedMainTableSize + RCDeflateDefs::s_kFixedDistTableSize];
        if (!DeCodeLevelTable(tmpLevels, numLitLenLevels + m_numDistLevels))
        {
            return false;
        }

        levels.SubClear();
        memcpy(levels.m_litLenLevels, tmpLevels, numLitLenLevels);
        memcpy(levels.m_distLevels, tmpLevels + numLitLenLevels, m_numDistLevels);
    }
    if(!m_mainDecoder.SetCodeLengths(levels.m_litLenLevels))
    {
        return false ;
    }
    return m_distDecoder.SetCodeLengths(levels.m_distLevels);
}

void RCDeflateDecoder::ReleaseOutStream()
{
    m_outWindowStream.ReleaseStream();
}

HResult RCDeflateDecoder::Flush()
{
    return m_outWindowStream.Flush();
}

HResult RCDeflateDecoder::CodeSpec(uint32_t curSize)
{
    if (m_remainLen == RCDeflateDefs::s_kLenIdFinished)
    {
        return RC_S_OK;
    }
    if (m_remainLen == RCDeflateDefs::s_kLenIdNeedInit)
    {
        if (!m_keepHistory)
        {
            if (!m_outWindowStream.Create(m_deflate64Mode ? RCDeflateDefs::s_kHistorySize64: RCDeflateDefs::s_kHistorySize32))
            {
                return RC_E_OUTOFMEMORY;
            }
        }
        HResult hr = InitInStream(m_needInitInStream) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        m_outWindowStream.Init(m_keepHistory);

        m_finalBlock = false;
        m_remainLen = 0;
        m_needReadTable = true;
    }

    if (curSize == 0)
    {
        return RC_S_OK;
    }

    while(m_remainLen > 0 && curSize > 0)
    {
        m_remainLen--;
        byte_t b = m_outWindowStream.GetByte(m_rep0);
        m_outWindowStream.PutByte(b);
        curSize--;
    }

    while(curSize > 0)
    {
        if (m_needReadTable)
        {
            if (m_finalBlock)
            {
                m_remainLen = RCDeflateDefs::s_kLenIdFinished;
                break;
            }
            if (!ReadTables())
            {
                return RC_S_FALSE;
            }
            m_needReadTable = false;
        }

        if(m_storedMode)
        {
            for (; m_storedBlockSize > 0 && curSize > 0; m_storedBlockSize--, curSize--)
            {
                m_outWindowStream.PutByte(m_inBitStream.ReadByte());
            }
            m_needReadTable = (m_storedBlockSize == 0);
            continue;
        }
        while(curSize > 0)
        {
            if (m_inBitStream.GetNumExtraBytes() > 4)
            {
                return RC_S_FALSE;
            }

            uint32_t number = m_mainDecoder.DecodeSymbol(&m_inBitStream);
            if (number < 0x100)
            {
                m_outWindowStream.PutByte((byte_t)number);
                curSize--;
                continue;
            }
            else if (number == RCDeflateDefs::s_kSymbolEndOfBlock)
            {
                m_needReadTable = true;
                break;
            }
            else if (number < RCDeflateDefs::s_kMainTableSize)
            {
                number -= RCDeflateDefs::s_kSymbolMatch;
                uint32_t len;
                {
                    int32_t numBits = 0 ;
                    if (m_deflate64Mode)
                    {
                        len = s_kDeflateLenStart64[number];
                        numBits = s_kDeflateLenDirectBits64[number];
                    }
                    else
                    {
                        len = s_kDeflateLenStart32[number];
                        numBits = s_kDeflateLenDirectBits32[number];
                    }
                    len += RCDeflateDefs::s_kMatchMinLen + m_inBitStream.ReadBits(numBits);
                }
                uint32_t locLen = len;
                if (locLen > curSize)
                {
                    locLen = (uint32_t)curSize;
                }
                number = m_distDecoder.DecodeSymbol(&m_inBitStream);
                if (number >= m_numDistLevels)
                {
                    return RC_S_FALSE;
                }
                uint32_t distance = s_kDeflateDistStart[number] + m_inBitStream.ReadBits(s_kDeflateDistDirectBits[number]);
                if (!m_outWindowStream.CopyBlock(distance, locLen))
                {
                    return RC_S_FALSE;
                }
                curSize -= locLen;
                len -= locLen;
                if (len != 0)
                {
                    m_remainLen = (int32_t)len;
                    m_rep0 = distance;
                    break;
                }
            }
            else
            {
                return RC_S_FALSE;
            }
        }
    }
    return RC_S_OK;
}

void RCDeflateDecoder::SetKeepHistory(bool keepHistory)
{
    m_keepHistory = keepHistory;
}

HResult RCDeflateDecoder::Code(ISequentialInStream* inStream,
                               ISequentialOutStream* outStream, 
                               const uint64_t* /*inSize*/, 
                               const uint64_t* outSize,
                               ICompressProgressInfo* progress)
{
    SetInStream(inStream);
    SetOutStreamSize(outSize);
    HResult res = CodeReal(outStream, outSize, progress) ;
    ReleaseInStream() ;
    return res ;
}

HResult RCDeflateDecoder::CodeReal(ISequentialOutStream* outStream, 
                                   const uint64_t* outSize,
                                   ICompressProgressInfo* progress)
{
    try
    {
        return CodeRealImpl(outStream, outSize, progress) ;
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

HResult RCDeflateDecoder::CodeRealImpl(ISequentialOutStream* outStream, 
                                       const uint64_t* outSize,
                                       ICompressProgressInfo* progress) 
{
    m_outWindowStream.SetStream(outStream);
    RCDeflateDecoderFlusher flusher(this);

    const uint64_t inStart = m_needInitInStream ? 0 : m_inBitStream.GetProcessedSize() ;
    const uint64_t start = m_outWindowStream.GetProcessedSize() ;
    for (;;)
    {
        uint32_t curSize = 1 << 18;
        if (outSize != 0)
        {
            const uint64_t rem = *outSize - (m_outWindowStream.GetProcessedSize() - start);
            if (curSize > rem)
            {
                curSize = (uint32_t)rem;
            }
        }
        if (curSize == 0)
        {
            break;
        }
        HResult hr = CodeSpec(curSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (m_remainLen == RCDeflateDefs::s_kLenIdFinished)
        {
            break;
        }
        if (progress != NULL)
        {
            const uint64_t inSize = m_inBitStream.GetProcessedSize() - inStart;
            const uint64_t nowPos64 = m_outWindowStream.GetProcessedSize() - start;
            hr = progress->SetRatioInfo(inSize, nowPos64) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    if (m_remainLen == RCDeflateDefs::s_kLenIdFinished && m_zlibMode)
    {
        m_inBitStream.AlignToByte();
        for (int i = 0; i < 4; i++)
        {
            m_zlibFooter[i] = m_inBitStream.ReadByte() ;
        }
    }
    flusher.m_needFlush = false ;
    HResult res = Flush() ;
    if (res == RC_S_OK && InputEofError())
    {
        return RC_S_FALSE ;
    }
    return res ;
}

HResult RCDeflateDecoder::GetInStreamProcessedSize(uint64_t& size)
{
    size = m_inBitStream.GetProcessedSize();
    return RC_S_OK ;
}

HResult RCDeflateDecoder::SetInStream(ISequentialInStream* inStream)
{
    m_inBitStream.SetStream(inStream);
    return RC_S_OK ;
}

HResult RCDeflateDecoder::ReleaseInStream()
{
    m_inBitStream.ReleaseStream();
    return RC_S_OK ;
}

HResult RCDeflateDecoder::SetOutStreamSize(const uint64_t* outSize)
{
    m_remainLen = RCDeflateDefs::s_kLenIdNeedInit;
    m_needInitInStream = true ;
    m_outWindowStream.Init(m_keepHistory);
    return RC_S_OK ;
}

HResult RCDeflateDecoder::Read(void* data, uint32_t size, uint32_t* processedSize)
{
    try
    {
        if (processedSize)
        {
            *processedSize = 0;
        }
        const uint64_t startPos = m_outWindowStream.GetProcessedSize();
        m_outWindowStream.SetMemStream((byte_t *)data);
        HResult hr = CodeSpec(size) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (processedSize)
        {
            *processedSize = (uint32_t)(m_outWindowStream.GetProcessedSize() - startPos);
        }
        return Flush();
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

HResult RCDeflateDecoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
{
    RCPropertyIDPairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyIDPair& pair = *pos ;
        if(pair.first == RCMethodProperty::ID_SET_KEEP_HISTORY)
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
        else if(pair.first == RCMethodProperty::ID_SET_NSIS_DEFLATE)
        {
            if(IsBooleanType(pair.second))
            {
                m_deflateNSIS = GetBooleanValue(pair.second);
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

HResult RCDeflateDecoder::CodeResume(ISequentialOutStream* outStream, const uint64_t* outSize, ICompressProgressInfo* progress)
{
    m_remainLen = RCDeflateDefs::s_kLenIdNeedInit ;
    m_outWindowStream.Init(m_keepHistory);
    return CodeReal(outStream, outSize, progress) ;
}

HResult RCDeflateDecoder::InitInStream(bool needInit)
{
    if (!m_inBitStream.Create(1 << 17))
    {
        return RC_E_OUTOFMEMORY ;
    }
    if (needInit)
    {
        m_inBitStream.Init() ;
        m_needInitInStream = false ;
    }
    return RC_S_OK;
}

void RCDeflateDecoder::AlignToByte()
{
    m_inBitStream.AlignToByte();
}

byte_t RCDeflateDecoder::ReadByte()
{
    return (byte_t)m_inBitStream.ReadBits(8);
}

bool RCDeflateDecoder::InputEofError() const
{
    return m_inBitStream.ExtraBitsWereRead();
}

uint64_t RCDeflateDecoder::GetInputProcessedSize() const
{
    return m_inBitStream.GetProcessedSize();
}

END_NAMESPACE_RCZIP
