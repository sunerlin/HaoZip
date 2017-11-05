/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCBitlBaseDecoder class implementation

BEGIN_NAMESPACE_RCZIP

template<class TInByte>
RCBitlBaseDecoder<TInByte>::RCBitlBaseDecoder():
    m_bitPos(0),
    m_value(0),
    m_numExtraBytes(0)
{
}

template<class TInByte>
RCBitlBaseDecoder<TInByte>::~RCBitlBaseDecoder()
{
}

template<class TInByte>
bool RCBitlBaseDecoder<TInByte>::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize) ;
}

template<class TInByte>
void RCBitlBaseDecoder<TInByte>::SetStream(ISequentialInStream *inStream)
{
    m_stream.SetStream(inStream);
}

template<class TInByte>
void RCBitlBaseDecoder<TInByte>::ReleaseStream()
{
    m_stream.ReleaseStream();
}

template<class TInByte>
void RCBitlBaseDecoder<TInByte>::Init()
{
    m_stream.Init();
    m_bitPos = RCBitlDecoderData::s_kNumBigValueBits;
    m_value = 0;
    m_numExtraBytes = 0;
}

template<class TInByte>
uint64_t RCBitlBaseDecoder<TInByte>::GetProcessedSize() const
{
    return m_stream.GetProcessedSize() + m_numExtraBytes - (RCBitlDecoderData::s_kNumBigValueBits - m_bitPos) / 8; 
}

template<class TInByte>
void RCBitlBaseDecoder<TInByte>::Normalize()
{
    for (;m_bitPos >= 8; m_bitPos -= 8)
    {
        byte_t b = 0;
        if (!m_stream.ReadByte(b))
        {
            b = 0xFF; // check it
            m_numExtraBytes++;
        }
        m_value = (b << (RCBitlDecoderData::s_kNumBigValueBits - m_bitPos)) | m_value;
    }
}

template<class TInByte>
uint32_t RCBitlBaseDecoder<TInByte>::ReadBits(int32_t numBits)
{
    Normalize();
    uint32_t res = m_value & ((1 << numBits) - 1);
    m_bitPos += numBits;
    m_value >>= numBits;
    return res;
}

template<class TInByte>
bool RCBitlBaseDecoder<TInByte>::ExtraBitsWereRead() const
{
    if (m_numExtraBytes == 0)
    {
        return false ;
    }
    return ((uint32_t)(RCBitlDecoderData::s_kNumBigValueBits - m_bitPos) < (m_numExtraBytes << 3));
}

template<class TInByte>
uint32_t RCBitlBaseDecoder<TInByte>::GetNumExtraBytes() const
{
    return m_numExtraBytes ;
}

END_NAMESPACE_RCZIP
