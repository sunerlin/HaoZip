/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "RCBitmEncoder.h"

/////////////////////////////////////////////////////////////////
//RCBitmEncoder class implementation

BEGIN_NAMESPACE_RCZIP

template<class TOutByte>
RCBitmEncoder<TOutByte>::RCBitmEncoder():
    m_bitPos(0),
    m_curByte(0)
{
}

template<class TOutByte>
RCBitmEncoder<TOutByte>::~RCBitmEncoder()
{
}

template<class TOutByte>
bool RCBitmEncoder<TOutByte>::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

template<class TOutByte>
void RCBitmEncoder<TOutByte>::SetStream(ISequentialOutStream *outStream)
{
    m_stream.SetStream(outStream);
}

template<class TOutByte>
void RCBitmEncoder<TOutByte>::ReleaseStream()
{
    m_stream.ReleaseStream();
}

template<class TOutByte>
void RCBitmEncoder<TOutByte>::Init()
{
    m_stream.Init();
    m_bitPos = 8;
    m_curByte = 0;
}

template<class TOutByte>
HResult RCBitmEncoder<TOutByte>::Flush()
{
    if(m_bitPos < 8)
    {
        WriteBits(0, m_bitPos);
    }
    return m_stream.Flush() ;
}

template<class TOutByte>
void RCBitmEncoder<TOutByte>::WriteBits(uint32_t value, int32_t numBits)
{
    while(numBits > 0)
    {
        if (numBits < m_bitPos)
        {
            m_curByte |= ((byte_t)value << (m_bitPos -= numBits));
            return;
        }
        numBits -= m_bitPos;
        uint32_t newBits = (value >> numBits);
        value -= (newBits << numBits);
        m_stream.WriteByte((byte_t)(m_curByte | newBits));
        m_bitPos = 8;
        m_curByte = 0;
    }
}

template<class TOutByte>
uint64_t RCBitmEncoder<TOutByte>::GetProcessedSize() const
{
    return m_stream.GetProcessedSize() + (8 - m_bitPos + 7) / 8;
}

END_NAMESPACE_RCZIP
