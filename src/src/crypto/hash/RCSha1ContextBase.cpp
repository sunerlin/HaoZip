/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1ContextBase.h"
#include "algorithm/RotateDefs.h"
#include "crypto/hash/RCSha1Defs.h"

/////////////////////////////////////////////////////////////////
//RCSha1ContextBase class implementation

BEGIN_NAMESPACE_RCZIP

RCSha1ContextBase::RCSha1ContextBase():
    m_count(0)
{
}

RCSha1ContextBase::~RCSha1ContextBase()
{
}

void RCSha1ContextBase::UpdateBlock(uint32_t *data, bool returnRes)
{
    GetBlockDigest(data, m_state, returnRes);
    ++m_count ;
}

void RCSha1ContextBase::Init()
{
    m_state[0] = 0x67452301;
    m_state[1] = 0xEFCDAB89;
    m_state[2] = 0x98BADCFE;
    m_state[3] = 0x10325476;
    m_state[4] = 0xC3D2E1F0;
    m_count = 0 ;
}

void RCSha1ContextBase::GetBlockDigest(uint32_t *data, uint32_t *destDigest, bool returnRes)
{
    uint32_t W[s_kNumW] ;
    uint32_t a = m_state[0];
    uint32_t b = m_state[1];
    uint32_t c = m_state[2];
    uint32_t d = m_state[3];
    uint32_t e = m_state[4];
#ifdef _SHA1_UNROLL
    RX_5(R0, 0); RX_5(R0, 5); RX_5(R0, 10);
#else
    int32_t i = 0 ;
    for (i = 0; i < 15; i += 5) { RX_5(R0, i); }
#endif
    RX_1_4(R0, R1, 15);


#ifdef _SHA1_UNROLL
    RX_5(R2, 20); RX_5(R2, 25); RX_5(R2, 30); RX_5(R2, 35);
    RX_5(R3, 40); RX_5(R3, 45); RX_5(R3, 50); RX_5(R3, 55);
    RX_5(R4, 60); RX_5(R4, 65); RX_5(R4, 70); RX_5(R4, 75);
#else
    i = 20;
    for (; i < 40; i += 5) { RX_5(R2, i); }
    for (; i < 60; i += 5) { RX_5(R3, i); }
    for (; i < 80; i += 5) { RX_5(R4, i); }
#endif

    destDigest[0] = m_state[0] + a;
    destDigest[1] = m_state[1] + b;
    destDigest[2] = m_state[2] + c;
    destDigest[3] = m_state[3] + d;
    destDigest[4] = m_state[4] + e;

    if (returnRes)
    {
        for (int32_t i = 0 ; i < 16; ++i)
        {
            data[i] = W[s_kNumW - 16 + i];
        }
    }
}

void RCSha1ContextBase::PrepareBlock(uint32_t *block, uint32_t size) const
{
    uint32_t curBufferPos = size & 0xF ;
    block[curBufferPos++] = 0x80000000;
    while (curBufferPos != (16 - 2))
    {
        block[curBufferPos++] = 0;
    }
    const uint64_t lenInBits = (m_count << 9) + ((uint64_t)size << 5);
    block[curBufferPos++] = (uint32_t)(lenInBits >> 32);
    block[curBufferPos++] = (uint32_t)(lenInBits);
}

END_NAMESPACE_RCZIP
