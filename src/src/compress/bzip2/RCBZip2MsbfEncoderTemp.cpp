/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/bzip2/RCBZip2MsbfEncoderTemp.h"

/////////////////////////////////////////////////////////////////
//RCBZip2MsbfEncoderTemp class implementation

BEGIN_NAMESPACE_RCZIP

RCBZip2MsbfEncoderTemp::RCBZip2MsbfEncoderTemp():
    m_pos(0),
    m_bitPos(0),
    m_curByte(0),
    m_buffer(NULL)
{
}

RCBZip2MsbfEncoderTemp::~RCBZip2MsbfEncoderTemp()
{
}

void RCBZip2MsbfEncoderTemp::SetStream(byte_t* buffer)
{
    m_buffer = buffer;
}

byte_t* RCBZip2MsbfEncoderTemp::GetStream() const
{
    return m_buffer;
}

void RCBZip2MsbfEncoderTemp::Init()
{
    m_pos = 0;
    m_bitPos = 8;
    m_curByte = 0;
}

void RCBZip2MsbfEncoderTemp::Flush()
{
    if(m_bitPos < 8)
    {
        WriteBits(0, m_bitPos);
    }
}

void RCBZip2MsbfEncoderTemp::WriteBits(uint32_t value, int32_t numBits)
{
    while(numBits > 0)
    {
        int32_t numNewBits = MyMin(numBits, m_bitPos);
        numBits -= numNewBits;
        m_curByte <<= numNewBits;
        uint32_t newBits = value >> numBits;
        m_curByte |= byte_t(newBits);
        value -= (newBits << numBits);
        m_bitPos -= numNewBits;

        if (m_bitPos == 0)
        {
            m_buffer[m_pos++] = m_curByte;
            m_bitPos = 8;
        }
    }
}

uint32_t RCBZip2MsbfEncoderTemp::GetBytePos() const
{
    return m_pos ;
}

uint32_t RCBZip2MsbfEncoderTemp::GetPos() const
{
    return m_pos * 8 + (8 - m_bitPos);
}

byte_t RCBZip2MsbfEncoderTemp::GetCurByte() const
{
    return m_curByte;
}

void RCBZip2MsbfEncoderTemp::SetPos(uint32_t bitPos)
{
    m_pos = bitPos / 8;
    m_bitPos = 8 - ((int32_t)bitPos & 7);
}

void RCBZip2MsbfEncoderTemp::SetCurState(int32_t bitPos, byte_t curByte)
{
    m_bitPos = 8 - bitPos;
    m_curByte = curByte;
}

END_NAMESPACE_RCZIP
