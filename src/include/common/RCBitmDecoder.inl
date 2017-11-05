/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "RCBitmDecoder.h"

/////////////////////////////////////////////////////////////////
//RCBitmDecoder class implementation

BEGIN_NAMESPACE_RCZIP

template<class TInByte>
RCBitmDecoder<TInByte>::RCBitmDecoder():
    m_bitPos(0),
    m_value(0)
{
}

template<class TInByte>
RCBitmDecoder<TInByte>::~RCBitmDecoder()
{
}

template<class TInByte>
bool RCBitmDecoder<TInByte>::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

template<class TInByte>
void RCBitmDecoder<TInByte>::SetStream(ISequentialInStream *inStream)
{
    m_stream.SetStream(inStream);
}

template<class TInByte>
void RCBitmDecoder<TInByte>::ReleaseStream()
{
    m_stream.ReleaseStream();
}

template<class TInByte>
void RCBitmDecoder<TInByte>::Init()
{
    m_stream.Init();
    m_bitPos = s_kNumBigValueBits;
    Normalize();
}

template<class TInByte>
uint64_t RCBitmDecoder<TInByte>::GetProcessedSize() const
{
    return m_stream.GetProcessedSize() - (s_kNumBigValueBits - m_bitPos) / 8;
}

template<class TInByte>
void RCBitmDecoder<TInByte>::Normalize()
{
    for (;m_bitPos >= 8; m_bitPos -= 8)
    {
        m_value = (m_value << 8) | m_stream.ReadByte();
    }
}

template<class TInByte>
uint32_t RCBitmDecoder<TInByte>::GetValue(uint32_t numBits) const
{
    return ((m_value >> (8 - m_bitPos)) & s_kMask) >> (s_kNumValueBits - numBits);
}

template<class TInByte>
void RCBitmDecoder<TInByte>::MovePos(uint32_t numBits)
{
    m_bitPos += numBits;
    Normalize();
}

template<class TInByte>
uint32_t RCBitmDecoder<TInByte>::ReadBits(uint32_t numBits)
{
    uint32_t res = GetValue(numBits);
    MovePos(numBits);
    return res;
}

template<class TInByte>
void RCBitmDecoder<TInByte>::AlignToByte()
{
    MovePos((32 - m_bitPos) & 7) ;
}
  
END_NAMESPACE_RCZIP
