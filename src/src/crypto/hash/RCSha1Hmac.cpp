/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1Hmac.h"

/////////////////////////////////////////////////////////////////
//RCSha1Hmac class implementation

BEGIN_NAMESPACE_RCZIP

RCSha1Hmac::RCSha1Hmac()
{
}

RCSha1Hmac::~RCSha1Hmac()
{
}

void RCSha1Hmac::SetKey(const byte_t *key, size_t keySize)
{
    byte_t keyTemp[RCSha1Defs::kBlockSize];
     size_t i;
    for (i = 0; i < RCSha1Defs::kBlockSize; ++i)
    {
        keyTemp[i] = 0;
    }
    if(keySize > RCSha1Defs::kBlockSize)
    {
        m_sha.Init();
        m_sha.Update(key, keySize);
        m_sha.Final(keyTemp);
        keySize = RCSha1Defs::kDigestSize;
    }
    else
    {
        for (i = 0; i < keySize; ++i)
        {
            keyTemp[i] = key[i];
        }
    }
    for (i = 0; i < RCSha1Defs::kBlockSize; ++i)
    {
        keyTemp[i] ^= 0x36;
    }
    m_sha.Init();
    m_sha.Update(keyTemp, RCSha1Defs::kBlockSize);
    for (i = 0; i < RCSha1Defs::kBlockSize; ++i)
    {
        keyTemp[i] ^= 0x36 ^ 0x5C;
    }
    m_sha2.Init();
    m_sha2.Update(keyTemp, RCSha1Defs::kBlockSize);
}

void RCSha1Hmac::Update(const byte_t *data, size_t dataSize)
{
    m_sha.Update(data, dataSize);
}

void RCSha1Hmac::Final(byte_t *mac, size_t macSize)
{
    byte_t digest[RCSha1Defs::kDigestSize];
    m_sha.Final(digest);
    m_sha2.Update(digest, RCSha1Defs::kDigestSize);
    m_sha2.Final(digest);
    for(size_t i = 0; i < macSize; ++i)
    {
        mac[i] = digest[i];
    }
}

END_NAMESPACE_RCZIP
