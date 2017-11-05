/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/range/RCRangeDecoder.h"
#include "compress/range/RCRangeCoderDefs.h"

/////////////////////////////////////////////////////////////////
//RCRangeDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRangeDecoder::RCRangeDecoder():
    m_range(0),
    m_code(0)
{
}

RCRangeDecoder::~RCRangeDecoder()
{
}

RCInBuffer& RCRangeDecoder::GetStream()
{
    return m_stream ;
}

uint32_t RCRangeDecoder::GetRange(void) const
{
    return m_range ;
}

uint32_t RCRangeDecoder::GetCode(void) const
{
    return m_code ;
}

void RCRangeDecoder::SetRange(uint32_t range)
{
    m_range = range ; 
}

void RCRangeDecoder::SetCode(uint32_t code)
{
    m_code = code ;
}

bool RCRangeDecoder::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

void RCRangeDecoder::Normalize()
{
    while (m_range < RCRangeCoderDefs::s_kTopValue)
    {
        m_code = (m_code << 8) | m_stream.ReadByte();
        m_range <<= 8;
    }
}

void RCRangeDecoder::SetStream(ISequentialInStream *stream)
{
    m_stream.SetStream(stream);
}

void RCRangeDecoder::Init()
{
    m_stream.Init();
    m_code = 0;
    m_range = 0xFFFFFFFF;
    for(int32_t i = 0; i < 5; i++)
    {
        m_code = (m_code << 8) | m_stream.ReadByte();
    }
}

void RCRangeDecoder::ReleaseStream()
{
    m_stream.ReleaseStream();
}

uint32_t RCRangeDecoder::GetThreshold(uint32_t total)
{
    return (m_code) / ( m_range /= total);
}

void RCRangeDecoder::Decode(uint32_t start, uint32_t size)
{
    m_code -= start * m_range;
    m_range *= size;
    Normalize();
}

uint32_t RCRangeDecoder::DecodeDirectBits(int numTotalBits)
{
    uint32_t range = m_range ;
    uint32_t code = m_code ;
    uint32_t result = 0 ;
    for (int32_t i = numTotalBits; i != 0; i--)
    {
        range >>= 1;
        uint32_t t = (code - range) >> 31;
        code -= range & (t - 1);
        result = (result << 1) | (1 - t);

        if (range < RCRangeCoderDefs::s_kTopValue)
        {
            code = (code << 8) | m_stream.ReadByte();
            range <<= 8;
        }
    }
    m_range = range;
    m_code = code;
    return result;
}

uint32_t RCRangeDecoder::DecodeBit(uint32_t size0, uint32_t numTotalBits)
{
    uint32_t newBound = (m_range >> numTotalBits) * size0;
    uint32_t symbol = 0 ;
    if (m_code < newBound)
    {
        symbol = 0;
        m_range = newBound;
    }
    else
    {
        symbol = 1;
        m_code -= newBound;
        m_range -= newBound;
    }
    Normalize();
    return symbol;
}

uint64_t RCRangeDecoder::GetProcessedSize()
{
    return m_stream.GetProcessedSize();
}

END_NAMESPACE_RCZIP
