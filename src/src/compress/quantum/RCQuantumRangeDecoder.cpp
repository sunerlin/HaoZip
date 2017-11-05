/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/quantum/RCQuantumRangeDecoder.h"

/////////////////////////////////////////////////////////////////
//RCQuantumRangeDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCQuantumRangeDecoder::RCQuantumRangeDecoder():
    m_low(0),
    m_range(0),
    m_code(0)
{
}

RCQuantumRangeDecoder::~RCQuantumRangeDecoder()
{
}

bool RCQuantumRangeDecoder::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize) ;
}

void RCQuantumRangeDecoder::SetStream(ISequentialInStream *stream)
{
    m_stream.SetStream(stream);
}

void RCQuantumRangeDecoder::ReleaseStream()
{
    m_stream.ReleaseStream();
}

void RCQuantumRangeDecoder::Init()
{
    m_stream.Init();
    m_low = 0;
    m_range = 0x10000;
    m_code = m_stream.ReadBits(16);
}

void RCQuantumRangeDecoder::Finish()
{
    // we need these extra two Bit_reads
    m_stream.ReadBit();
    m_stream.ReadBit();
    m_stream.Finish();
}

uint64_t RCQuantumRangeDecoder::GetProcessedSize() const
{
    return m_stream.GetProcessedSize();
}

uint32_t RCQuantumRangeDecoder::GetThreshold(uint32_t total) const
{
    return ((m_code + 1) * total - 1) / m_range; // & 0xFFFF is not required;
}

void RCQuantumRangeDecoder::Decode(uint32_t start, uint32_t end, uint32_t total)
{
    uint32_t high = m_low + end * m_range / total - 1;
    uint32_t offset = start * m_range / total;
    m_code -= offset;
    m_low += offset;
    for (;;)
    {
        if ((m_low & 0x8000) != (high & 0x8000))
        {
            if ((m_low & 0x4000) == 0 || (high & 0x4000) != 0)
            {
                break;
            }
            m_low &= 0x3FFF;
            high |= 0x4000;
        }
        m_low = (m_low << 1) & 0xFFFF;
        high = ((high << 1) | 1) & 0xFFFF;
        m_code = ((m_code << 1) | m_stream.ReadBit());
    }
    m_range = high - m_low + 1;
}

END_NAMESPACE_RCZIP
