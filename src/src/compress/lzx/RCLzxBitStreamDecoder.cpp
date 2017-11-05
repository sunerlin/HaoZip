/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/lzx/RCLzxBitStreamDecoder.h"

/////////////////////////////////////////////////////////////////
//RCLzxBitStreamDecoder class implementation

BEGIN_NAMESPACE_RCZIP

class RCLzxBitStreamDefs
{
public:
    static const int32_t  s_kNumBigValueBits = 8 * 4;
    static const int32_t  s_kNumValueBits = 17;
    static const uint32_t s_kBitDecoderValueMask = (1 << s_kNumValueBits) - 1;
};

RCLzxBitStreamDecoder::RCLzxBitStreamDecoder():
    m_value(0),
    m_bitPos(0)
{
}

RCLzxBitStreamDecoder::~RCLzxBitStreamDecoder()
{
}

bool RCLzxBitStreamDecoder::Create(uint32_t bufferSize)
{
    return m_stream.Create(bufferSize);
}

void RCLzxBitStreamDecoder::SetStream(ISequentialInStream *s)
{
    m_stream.SetStream(s);
}

void RCLzxBitStreamDecoder::ReleaseStream()
{
    m_stream.ReleaseStream();
}

void RCLzxBitStreamDecoder::Init()
{
    m_stream.Init();
    m_bitPos = RCLzxBitStreamDefs::s_kNumBigValueBits;
}

uint64_t RCLzxBitStreamDecoder::GetProcessedSize() const
{
    return m_stream.GetProcessedSize() - (RCLzxBitStreamDefs::s_kNumBigValueBits - m_bitPos) / 8;
}

int32_t RCLzxBitStreamDecoder::GetBitPosition() const
{
    return m_bitPos & 0xF;
}

void RCLzxBitStreamDecoder::Normalize()
{
    for (;m_bitPos >= 16; m_bitPos -= 16)
    {
        byte_t b0 = m_stream.ReadByte();
        byte_t b1 = m_stream.ReadByte();
        m_value = (m_value << 8) | b1;
        m_value = (m_value << 8) | b0;
    }
}

uint32_t RCLzxBitStreamDecoder::GetValue(int32_t numBits) const
{
    return ((m_value >>  ((32 - RCLzxBitStreamDefs::s_kNumValueBits) - m_bitPos)) & 
           RCLzxBitStreamDefs::s_kBitDecoderValueMask) >> (RCLzxBitStreamDefs::s_kNumValueBits - numBits) ;
}

void RCLzxBitStreamDecoder::MovePos(uint32_t numBits)
{
    m_bitPos += (int32_t)numBits;
    Normalize();
}

uint32_t RCLzxBitStreamDecoder::ReadBits(int32_t numBits)
{
    uint32_t res = GetValue(numBits);
    MovePos(numBits);
    return res;
}

uint32_t RCLzxBitStreamDecoder::ReadBitsBig(int32_t numBits)
{
    int32_t numBits0 = numBits / 2;
    int32_t numBits1 = numBits - numBits0;
    uint32_t res = ReadBits(numBits0) << numBits1;
    return res + ReadBits(numBits1);
}

bool RCLzxBitStreamDecoder::ReadUInt32(uint32_t &v)
{
    if (m_bitPos != 0)
    {
        return false;
    }
    v = ((m_value >> 16) & 0xFFFF) | ((m_value << 16) & 0xFFFF0000);
    m_bitPos = RCLzxBitStreamDefs::s_kNumBigValueBits;
    return true;
}

byte_t RCLzxBitStreamDecoder::DirectReadByte()
{
    return m_stream.ReadByte();
}

END_NAMESPACE_RCZIP
