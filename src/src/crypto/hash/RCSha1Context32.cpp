/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1Context32.h"

/////////////////////////////////////////////////////////////////
//RCSha1Context32 class implementation

BEGIN_NAMESPACE_RCZIP

RCSha1Context32::RCSha1Context32()
{
}

RCSha1Context32::~RCSha1Context32()
{
}

void RCSha1Context32::Update(const uint32_t *data, size_t size)
{
    while (size-- > 0)
    {
        m_buffer[m_count2++] = *data++;
        if (m_count2 == RCSha1Defs::kBlockSizeInWords)
        {
            m_count2 = 0;
            UpdateBlock();
        }
    }
}

void RCSha1Context32::Final(uint32_t *digest)
{
    const uint64_t lenInBits = (m_count << 9) + ((uint64_t)m_count2 << 5);
    uint32_t curBufferPos = m_count2;
    m_buffer[curBufferPos++] = 0x80000000;
    while (curBufferPos != (16 - 2))
    {
        curBufferPos &= 0xF;
        if (curBufferPos == 0)
        {
            UpdateBlock();
        }
        m_buffer[curBufferPos++] = 0;
    }
    m_buffer[curBufferPos++] = (uint32_t)(lenInBits >> 32);
    m_buffer[curBufferPos++] = (uint32_t)(lenInBits);
    GetBlockDigest(m_buffer, digest);
    Init();
}

END_NAMESPACE_RCZIP
