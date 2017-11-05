/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimBitStream.h"

/////////////////////////////////////////////////////////////////
//RCWimBitStream class implementation

BEGIN_NAMESPACE_RCZIP

RCWimBitStream::RCWimBitStream():
    m_value(0),
    m_bitPos(0)
{
}

RCWimBitStream::~RCWimBitStream()
{
}

bool RCWimBitStream::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

void RCWimBitStream::SetStream(ISequentialInStream *s)
{
    m_stream.SetStream(s);
}

void RCWimBitStream::ReleaseStream()
{
    m_stream.ReleaseStream();
}

void RCWimBitStream::Init()
{
    m_stream.Init();
    m_bitPos = 0;
}

byte_t RCWimBitStream::DirectReadByte()
{
    return m_stream.ReadByte() ;
}

void RCWimBitStream::Normalize()
{
    if (m_bitPos < 16)
    {
        byte_t b0 = m_stream.ReadByte();
        byte_t b1 = m_stream.ReadByte();
        m_value = (m_value << 8) | b1;
        m_value = (m_value << 8) | b0;
        m_bitPos += 16;
    }
}

uint32_t RCWimBitStream::GetValue(uint32_t numBits)
{
    Normalize();
    return (m_value >> (m_bitPos - numBits)) & ((1 << numBits) - 1);
}

void RCWimBitStream::MovePos(uint32_t numBits)
{
    m_bitPos -= numBits;
}

uint32_t RCWimBitStream::ReadBits(uint32_t numBits)
{
    uint32_t res = GetValue(numBits);
    m_bitPos -= numBits;
    return res;
}

END_NAMESPACE_RCZIP
