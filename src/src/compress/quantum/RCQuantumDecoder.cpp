/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/quantum/RCQuantumDecoder.h"
#include "interface/RCMethodDefs.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////
//RCQuantumDecoder class implementation

BEGIN_NAMESPACE_RCZIP

static const int32_t s_kQuantumLenIdNeedInit = -2 ;

class RCQuantumDecoderFlusher
{
public:
    RCQuantumDecoderFlusher(RCQuantumDecoder *decoder):
        m_decoder(decoder), 
        m_needFlush(true)
    {
    }
    ~RCQuantumDecoderFlusher()
    {
        if(m_needFlush)
        {
            m_decoder->Flush();
        }
        m_decoder->ReleaseStreams() ;
    }
public:
    bool m_needFlush ;
private:
    RCQuantumDecoder* m_decoder ;
};

RCQuantumDecoder::RCQuantumDecoder():
    m_outSize(0),
    m_remainLen(0),
    m_rep0(0),
    m_numDictBits(0),
    m_dictionarySize(0),
    m_keepHistory(false)
{
}

RCQuantumDecoder::~RCQuantumDecoder()
{
}

void RCQuantumDecoder::Init()
{
    m_selector.Init(RCQuantumDefs::s_kNumSelectors);
    for (uint32_t i = 0; i < RCQuantumDefs::s_kNumLitSelectors; i++)
    {
        m_literals[i].Init(RCQuantumDefs::s_kNumLitSymbols);
    }
    uint32_t numItems = m_numDictBits << 1;
    m_posSlot[0].Init((uint32_t)std::min(numItems, RCQuantumDefs::s_kNumLen3PosSymbolsMax));
    m_posSlot[1].Init((uint32_t)std::min(numItems, RCQuantumDefs::s_kNumLen4PosSymbolsMax));
    m_posSlot[2].Init((uint32_t)std::min(numItems, RCQuantumDefs::s_kNumLen5PosSymbolsMax));
    m_lenSlot.Init(RCQuantumDefs::s_kNumLenSymbols);
}

HResult RCQuantumDecoder::CodeSpec(uint32_t curSize)
{
    if (m_remainLen == s_kQuantumLenIdNeedInit)
    {
        if (!m_keepHistory)
        {
            if (!m_outWindowStream.Create(m_dictionarySize))
            {
                return RC_E_OUTOFMEMORY;
            }
            Init();
        }
        if (!m_rangeDecoder.Create(1 << 20))
        {
            return RC_E_OUTOFMEMORY;
        }
        m_rangeDecoder.Init();
        m_remainLen = 0;
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
        if (m_rangeDecoder.m_stream.WasFinished())
        {
            return RC_S_FALSE ;
        }

        uint32_t selector = m_selector.Decode(&m_rangeDecoder);
        if (selector < RCQuantumDefs::s_kNumLitSelectors)
        {
            byte_t b = (byte_t)((selector << (8 - RCQuantumDefs::s_kNumLitSelectorBits)) + m_literals[selector].Decode(&m_rangeDecoder));
            m_outWindowStream.PutByte(b);
            curSize--;
        }
        else
        {
            selector -= RCQuantumDefs::s_kNumLitSelectors;
            uint32_t len = selector + RCQuantumDefs::s_kMatchMinLen;
            if (selector == 2)
            {
                uint32_t lenSlot = m_lenSlot.Decode(&m_rangeDecoder);;
                if (lenSlot >= RCQuantumDefs::s_kNumSimpleLenSlots)
                {
                    lenSlot -= 2;
                    int32_t numDirectBits = (int32_t)(lenSlot >> 2);
                    len +=  ((4 | (lenSlot & 3)) << numDirectBits) - 2;
                    if (numDirectBits < 6)
                    {
                        len += m_rangeDecoder.m_stream.ReadBits(numDirectBits);
                    }
                }
                else
                {
                    len += lenSlot;
                }
            }
            uint32_t rep0 = m_posSlot[selector].Decode(&m_rangeDecoder);;
            if (rep0 >= RCQuantumDefs::s_kNumSimplePosSlots)
            {
                int32_t numDirectBits = (int32_t)((rep0 >> 1) - 1);
                rep0 = ((2 | (rep0 & 1)) << numDirectBits) + m_rangeDecoder.m_stream.ReadBits(numDirectBits);
            }
            uint32_t locLen = len;
            if (len > curSize)
            {
                locLen = (uint32_t)curSize;
            }
            if (!m_outWindowStream.CopyBlock(rep0, locLen))
            {
                return RC_S_FALSE;
            }
            curSize -= locLen;
            len -= locLen;
            if (len != 0)
            {
                m_remainLen = (int32_t)len;
                m_rep0 = rep0;
                break;
            }
        }
    }
    return m_rangeDecoder.m_stream.WasFinished() ? RC_S_FALSE : RC_S_OK;
}

void RCQuantumDecoder::ReleaseStreams()
{
    m_outWindowStream.ReleaseStream();
    ReleaseInStream();
}

HResult RCQuantumDecoder::Flush()
{
    return m_outWindowStream.Flush();
}

void RCQuantumDecoder::SetParams(int32_t numDictBits)
{
    m_numDictBits = numDictBits;
    m_dictionarySize = (uint32_t)1 << numDictBits;
}

void RCQuantumDecoder::SetKeepHistory(bool keepHistory)
{
    m_keepHistory = keepHistory;
}

HResult RCQuantumDecoder::Code( ISequentialInStream* inStream,
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

HResult RCQuantumDecoder::CodeReal( ISequentialInStream* inStream,
                                    ISequentialOutStream* outStream, 
                                    const uint64_t* inSize, 
                                    const uint64_t* outSize,
                                    ICompressProgressInfo* progress)
{
    if (outSize == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    uint64_t size = *outSize;

    SetInStream(inStream);
    m_outWindowStream.SetStream(outStream);
    SetOutStreamSize(outSize);
    RCQuantumDecoderFlusher flusher(this);

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
        HResult hr = CodeSpec(curSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (progress != NULL)
        {
            uint64_t inSize = m_rangeDecoder.GetProcessedSize();
            uint64_t nowPos64 = m_outWindowStream.GetProcessedSize() - start;
            hr = progress->SetRatioInfo(inSize, nowPos64) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
    }
    flusher.m_needFlush = false;
    return Flush();
}

HResult RCQuantumDecoder::SetInStream(ISequentialInStream* inStream)
{
    m_rangeDecoder.SetStream(inStream);
    return RC_S_OK ;
}

HResult RCQuantumDecoder::ReleaseInStream()
{
    m_rangeDecoder.ReleaseStream();
    return RC_S_OK ;
}

HResult RCQuantumDecoder::SetOutStreamSize(const uint64_t* outSize)
{
    if (outSize == NULL)
    {
        return RC_E_FAIL ;
    }
    m_remainLen = s_kQuantumLenIdNeedInit ;
    m_outWindowStream.Init(m_keepHistory) ;
    return RC_S_OK ;
}

HResult RCQuantumDecoder::SetCoderProperties(const RCPropertyIDPairArray& propertyArray)
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
                SetParams(numDictBits) ;
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
        else
        {
            return RC_E_INVALIDARG ;
        }
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
