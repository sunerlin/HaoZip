/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1Context.h"

/////////////////////////////////////////////////////////////////
//RCSha1Context class implementation

BEGIN_NAMESPACE_RCZIP

RCSha1Context::RCSha1Context()
{
}

RCSha1Context::~RCSha1Context()
{
}

void RCSha1Context::Update(byte_t *data, size_t size, bool rar350Mode)
{
    bool returnRes = false;
    uint32_t curBufferPos = m_count2;
    while (size-- > 0)
    {
        uint32_t pos = (curBufferPos & 3) ;
        if (pos == 0)
        {
            m_buffer[curBufferPos >> 2] = 0 ;
        }
        m_buffer[curBufferPos >> 2] |= ((uint32_t)*data++) << (8 * (3 - pos));
        if (++curBufferPos == RCSha1Defs::kBlockSize)
        {
            curBufferPos = 0;
            RCSha1ContextBase::UpdateBlock(m_buffer, returnRes);
            if (returnRes)
            {
                for (int i = 0; i < RCSha1Defs::kBlockSizeInWords; i++)
                {
                    uint32_t d = m_buffer[i];
                    data[i * 4 + 0 - RCSha1Defs::kBlockSize] = (byte_t)(d);
                    data[i * 4 + 1 - RCSha1Defs::kBlockSize] = (byte_t)(d >>  8);
                    data[i * 4 + 2 - RCSha1Defs::kBlockSize] = (byte_t)(d >> 16);
                    data[i * 4 + 3 - RCSha1Defs::kBlockSize] = (byte_t)(d >> 24);
                }
            }
            returnRes = rar350Mode;
        }
    }
    m_count2 = curBufferPos ;
}

void RCSha1Context::Update(const byte_t *data, size_t size)
{
    Update((byte_t *)data, size, false);
}

void RCSha1Context::Final(byte_t *digest)
{
    const uint64_t lenInBits = (m_count << 9) + ((uint64_t)m_count2 << 3);
    uint32_t curBufferPos = m_count2;
    uint32_t pos = (curBufferPos & 3);
    curBufferPos >>= 2;
    if (pos == 0)
    {
        m_buffer[curBufferPos] = 0;
    }
    m_buffer[curBufferPos++] |= ((uint32_t)0x80) << (8 * (3 - pos));

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
    UpdateBlock();

    uint32_t i = 0 ;
    for (i = 0; i < RCSha1Defs::kDigestSizeInWords; ++i)
    {
        uint32_t state = m_state[i] & 0xFFFFFFFF;
        *digest++ = (byte_t)(state >> 24);
        *digest++ = (byte_t)(state >> 16);
        *digest++ = (byte_t)(state >> 8);
        *digest++ = (byte_t)(state);
    }
    Init() ;
}

END_NAMESPACE_RCZIP
