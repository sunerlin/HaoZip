/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar3RangeDecoder.h"
#include "compress/rar/RCRar3Defs.h"

/////////////////////////////////////////////////////////////////
//RCRar3RangeDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRar3RangeDecoder::RCRar3RangeDecoder():
    m_range(0),
    m_low(0),
    m_code(0)
{
}

RCRar3RangeDecoder::~RCRar3RangeDecoder()
{
}

void RCRar3RangeDecoder::Normalize()
{
    while( (m_low ^ (m_low + m_range)) < RCRar3Defs::s_kTopValue ||
            m_range < RCRar3Defs::s_kBot && ((m_range = (0 - m_low) & (RCRar3Defs::s_kBot - 1)), 1))
    {
        m_code = (m_code << 8) | m_stream.ReadByte();
        m_range <<= 8;
        m_low <<= 8;
    }
}

void RCRar3RangeDecoder::InitRangeCoder()
{
    m_code = 0;
    m_low = 0;
    m_range = 0xFFFFFFFF;
    for(int i = 0; i < 4; i++)
    {
        m_code = (m_code << 8) | ReadBits(8);
    }
}

uint32_t RCRar3RangeDecoder::GetThreshold(uint32_t total)
{
    return (m_code - m_low) / ( m_range /= total);
}

void RCRar3RangeDecoder::Decode(uint32_t start, uint32_t size)
{
    m_low += start * m_range;
    m_range *= size;
    Normalize();
}

uint32_t RCRar3RangeDecoder::DecodeBit(uint32_t size0, uint32_t numTotalBits)
{
    if (((m_code - m_low) / (m_range >>= numTotalBits)) < size0)
    {
        Decode(0, size0);
        return 0;
    }
    else
    {
        Decode(size0, (1 << numTotalBits) - size0);
        return 1;
    }
}

END_NAMESPACE_RCZIP
