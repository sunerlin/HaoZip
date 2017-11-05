/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/quantum/RCQuantumStreamBitDecoder.h"

/////////////////////////////////////////////////////////////////
//RCQuantumStreamBitDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCQuantumStreamBitDecoder::RCQuantumStreamBitDecoder():
    m_value(0)
{
}

RCQuantumStreamBitDecoder::~RCQuantumStreamBitDecoder()
{
}

bool RCQuantumStreamBitDecoder::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

void RCQuantumStreamBitDecoder::SetStream(ISequentialInStream *inStream)
{
    m_stream.SetStream(inStream);
}

void RCQuantumStreamBitDecoder::ReleaseStream()
{
    m_stream.ReleaseStream();
}

void RCQuantumStreamBitDecoder::Finish()
{
    m_value = 0x10000;
}

void RCQuantumStreamBitDecoder::Init()
{
    m_stream.Init() ;
    m_value = 0x10000;
}

uint64_t RCQuantumStreamBitDecoder::GetProcessedSize() const
{
    return m_stream.GetProcessedSize();
}

bool RCQuantumStreamBitDecoder::WasFinished() const
{
    return m_stream.WasFinished();
}

uint32_t RCQuantumStreamBitDecoder::ReadBit()
{
    if (m_value >= 0x10000)
    {
        m_value = 0x100 | m_stream.ReadByte();
    }
    uint32_t res = (m_value >> 7) & 1 ;
    m_value <<= 1;
    return res ;
}

uint32_t RCQuantumStreamBitDecoder::ReadBits(int32_t numBits)
{
    uint32_t res = 0;
    do
    {
        res = (res << 1) | ReadBit();
    }
    while(--numBits != 0);
    return res;
}

END_NAMESPACE_RCZIP
