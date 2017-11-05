/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1Hmac32.h"
#include "crypto/hash/RCSha1Context.h"

/////////////////////////////////////////////////////////////////
//RCSha1Hmac32 class implementation

BEGIN_NAMESPACE_RCZIP

RCSha1Hmac32::RCSha1Hmac32()
{
}

RCSha1Hmac32::~RCSha1Hmac32()
{
}

void RCSha1Hmac32::SetKey(const byte_t *key, size_t keySize)
{
    uint32_t keyTemp[RCSha1Defs::kBlockSizeInWords];
    size_t i;
    for (i = 0; i < RCSha1Defs::kBlockSizeInWords; ++i)
    {
        keyTemp[i] = 0;
    }
    if(keySize > RCSha1Defs::kBlockSize)
    {
        RCSha1Context sha;
        sha.Init();
        sha.Update(key, keySize);
        byte_t digest[RCSha1Defs::kDigestSize];
        sha.Final(digest);
    
        for (int i = 0 ; i < RCSha1Defs::kDigestSizeInWords; ++i)
        {
            keyTemp[i] =  ((uint32_t)(digest[i * 4 + 0]) << 24) |
                          ((uint32_t)(digest[i * 4 + 1]) << 16) |
                          ((uint32_t)(digest[i * 4 + 2]) <<  8) |
                          ((uint32_t)(digest[i * 4 + 3]));
            keySize = RCSha1Defs::kDigestSizeInWords;
        }
    }
    else
    {
        for (size_t i = 0; i < keySize; i++)
        {
            keyTemp[i / 4] |= (key[i] << (24 - 8 * (i & 3)));
        }
    }
    for (i = 0; i < RCSha1Defs::kBlockSizeInWords; ++i)
    {
        keyTemp[i] ^= 0x36363636;
    }
    m_sha.Init();
    m_sha.Update(keyTemp, RCSha1Defs::kBlockSizeInWords);
    for (i = 0; i < RCSha1Defs::kBlockSizeInWords; ++i)
    {
        keyTemp[i] ^= 0x36363636 ^ 0x5C5C5C5C;
    }
    m_sha2.Init();
    m_sha2.Update(keyTemp, RCSha1Defs::kBlockSizeInWords);
}

void RCSha1Hmac32::Update(const uint32_t *data, size_t dataSize)
{
    m_sha.Update(data, dataSize);
}

void RCSha1Hmac32::Final(uint32_t *mac, size_t macSize) 
{
    uint32_t digest[RCSha1Defs::kDigestSizeInWords];
    m_sha.Final(digest);
    m_sha2.Update(digest, RCSha1Defs::kDigestSizeInWords);
    m_sha2.Final(digest);
    for(size_t i = 0; i < macSize; ++i)
    {
        mac[i] = digest[i];
    }
}

void RCSha1Hmac32::GetLoopXorDigest(uint32_t *mac, uint32_t numIteration)
{
    uint32_t block[RCSha1Defs::kBlockSizeInWords];
    uint32_t block2[RCSha1Defs::kBlockSizeInWords];
    m_sha.PrepareBlock(block, RCSha1Defs::kDigestSizeInWords);
    m_sha2.PrepareBlock(block2, RCSha1Defs::kDigestSizeInWords);
    for(uint32_t s = 0; s < RCSha1Defs::kDigestSizeInWords; ++s)
    {
        block[s] = mac[s];
    }
    for(uint32_t i = 0; i < numIteration; ++i)
    {
        m_sha.GetBlockDigest(block, block2);
        m_sha2.GetBlockDigest(block2, block);
        for (uint32_t s = 0; s < RCSha1Defs::kDigestSizeInWords; ++s)
        {
            mac[s] ^= block[s];
        }
    }
}

END_NAMESPACE_RCZIP
