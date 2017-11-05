/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar3BitDecoder.h"

/////////////////////////////////////////////////////////////////
//RCRar3BitDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRar3BitDecoder::RCRar3BitDecoder():
    m_bitPos(0),
    m_value(0)
{
}

RCRar3BitDecoder::~RCRar3BitDecoder()
{
}

bool RCRar3BitDecoder::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

void RCRar3BitDecoder::SetStream(ISequentialInStream *inStream)
{
    m_stream.SetStream(inStream) ;
}

void RCRar3BitDecoder::ReleaseStream()
{
    m_stream.ReleaseStream();
}
void RCRar3BitDecoder::Init()
{
    m_stream.Init();
    m_bitPos = 0;
    m_value = 0;
}

uint64_t RCRar3BitDecoder::GetProcessedSize() const
{
    return m_stream.GetProcessedSize() - (m_bitPos) / 8;
}

uint32_t RCRar3BitDecoder::GetBitPosition() const
{
    return ((8 - m_bitPos) & 7);
}

uint32_t RCRar3BitDecoder::GetValue(uint32_t numBits)
{
    if (m_bitPos < numBits)
    {
        m_bitPos += 8;
        m_value = (m_value << 8) | m_stream.ReadByte();
        if (m_bitPos < numBits)
        {
            m_bitPos += 8;
            m_value = (m_value << 8) | m_stream.ReadByte();
        }
    }
    return m_value >> (m_bitPos - numBits);
}

void RCRar3BitDecoder::MovePos(uint32_t numBits)
{
    m_bitPos -= numBits;
    m_value = m_value & ((1 << m_bitPos) - 1);
}

uint32_t RCRar3BitDecoder::ReadBits(uint32_t numBits)
{
    uint32_t res = GetValue(numBits);
    MovePos(numBits);
    return res;
}

END_NAMESPACE_RCZIP
