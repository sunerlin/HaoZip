/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/range/RCRangeEncoder.h"
#include "compress/range/RCRangeCoderDefs.h"

/////////////////////////////////////////////////////////////////
//RCRangeEncoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRangeEncoder::RCRangeEncoder():
    m_cacheSize(0),
    m_cache(0),
    m_low(0),
    m_range(0)
{
}

RCRangeEncoder::~RCRangeEncoder()
{
}

uint64_t RCRangeEncoder::GetLow(void) const
{
    return m_low ;
}

uint32_t RCRangeEncoder::GetRange(void) const
{
    return m_range ;
}

RCOutBuffer& RCRangeEncoder::GetStream(void)
{
    return m_stream ;
}

void RCRangeEncoder::SetLow(uint64_t low)
{
    m_low = low ;
}

void RCRangeEncoder::SetRange(uint32_t range) 
{
    m_range = range ;
}

bool RCRangeEncoder::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize) ;
}

void RCRangeEncoder::SetStream(ISequentialOutStream *stream)
{
    m_stream.SetStream(stream) ;
}

void RCRangeEncoder::Init()
{
    m_stream.Init();
    m_low = 0;
    m_range = 0xFFFFFFFF;
    m_cacheSize = 1;
    m_cache = 0;
}

void RCRangeEncoder::FlushData()
{
    for(int32_t i = 0; i < 5; i++)
    {
        ShiftLow();
    }
}

HResult RCRangeEncoder::FlushStream()
{
    return m_stream.Flush(); 
}

void RCRangeEncoder::ReleaseStream()
{
    m_stream.ReleaseStream();
}

void RCRangeEncoder::Encode(uint32_t start, uint32_t size, uint32_t total)
{
    m_low += start * (m_range /= total);
    m_range *= size;
    while (m_range < RCRangeCoderDefs::s_kTopValue)
    {
        m_range <<= 8;
        ShiftLow();
    }
}

void RCRangeEncoder::ShiftLow()
{
    if ((uint32_t)m_low < (uint32_t)0xFF000000 || (int32_t)(m_low >> 32) != 0)
    {
        byte_t temp = m_cache;
        do
        {
            m_stream.WriteByte((byte_t)(temp + (byte_t)(m_low >> 32)));
            temp = 0xFF;
        }
        while(--m_cacheSize != 0);
        m_cache = (byte_t)((uint32_t)m_low >> 24);
    }
    m_cacheSize++;
    m_low = (uint32_t)m_low << 8;
}

void RCRangeEncoder::EncodeDirectBits(uint32_t value, int32_t numBits)
{
    for (numBits--; numBits >= 0; numBits--)
    {
        m_range >>= 1;
        m_low += m_range & (0 - ((value >> numBits) & 1));
        if (m_range < RCRangeCoderDefs::s_kTopValue)
        {
            m_range <<= 8;
            ShiftLow();
        }
    }
}

void RCRangeEncoder::EncodeBit(uint32_t size0, uint32_t numTotalBits, uint32_t symbol)
{
    uint32_t newBound = (m_range >> numTotalBits) * size0;
    if (symbol == 0)
    {
        m_range = newBound;
    }
    else
    {
        m_low += newBound;
        m_range -= newBound;
    }
    while (m_range < RCRangeCoderDefs::s_kTopValue)
    {
        m_range <<= 8;
        ShiftLow();
    }
}

uint64_t RCRangeEncoder::GetProcessedSize()
{ 
    return m_stream.GetProcessedSize() + m_cacheSize + 4 ;
}

END_NAMESPACE_RCZIP
