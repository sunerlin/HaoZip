/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

/////////////////////////////////////////////////////////////////
//RCBitlDecoder class implementation

BEGIN_NAMESPACE_RCZIP

template<class TInByte>
RCBitlDecoder<TInByte>::RCBitlDecoder():
    m_normalValue(0)
{
}

template<class TInByte>
RCBitlDecoder<TInByte>::~RCBitlDecoder()
{
}

template<class TInByte>
void RCBitlDecoder<TInByte>::Init()
{
    RCBitlBaseDecoder<TInByte>::Init();
    m_normalValue = 0 ;
}

template<class TInByte>
void RCBitlDecoder<TInByte>::Normalize()
{
    for (; this->m_bitPos >= 8; this->m_bitPos -= 8)
    {
        byte_t b = 0;
        if (!this->m_stream.ReadByte(b))
        {
            b = 0xFF; // check it
            this->m_numExtraBytes++;
        }
        m_normalValue = (b << (RCBitlDecoderData::s_kNumBigValueBits - this->m_bitPos)) | m_normalValue;
        this->m_value = (this->m_value << 8) | RCBitlDefs::Instance().s_kInvertTable[b];
    }
}

template<class TInByte>
uint32_t RCBitlDecoder<TInByte>::GetValue(int32_t numBits)
{
    Normalize();
    return ((this->m_value >> (8 - this->m_bitPos)) & RCBitlDecoderData::s_kMask) >> (RCBitlDecoderData::s_kNumValueBits - numBits);
}

template<class TInByte>
void RCBitlDecoder<TInByte>::MovePos(int32_t numBits)
{
    this->m_bitPos += numBits;
    m_normalValue >>= numBits;
}

template<class TInByte>
uint32_t RCBitlDecoder<TInByte>::ReadBits(int32_t numBits)
{
    Normalize();
    uint32_t res = m_normalValue & ( (1 << numBits) - 1);
    MovePos(numBits);
    return res;
}

template<class TInByte>
void RCBitlDecoder<TInByte>::AlignToByte()
{
    MovePos((32 - this->m_bitPos) & 7) ;
}

template<class TInByte>
byte_t RCBitlDecoder<TInByte>::ReadByte()
{
    if (this->m_bitPos == RCBitlDecoderData::s_kNumBigValueBits)
    {
        byte_t b = 0;
        if (!this->m_stream.ReadByte(b))
        {
            b = 0xFF;
            this->m_numExtraBytes++;
        }
        return b;
    }
    {
        byte_t b = (byte_t)(m_normalValue & 0xFF);
        MovePos(8);
        return b ;
    }
}

END_NAMESPACE_RCZIP
