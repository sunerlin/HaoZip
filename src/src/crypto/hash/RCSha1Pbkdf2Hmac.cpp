/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/hash/RCSha1Pbkdf2Hmac.h"
#include "crypto/hash/RCSha1Hmac.h"
#include "crypto/hash/RCSha1Hmac32.h"

/////////////////////////////////////////////////////////////////
//RCSha1Pbkdf2Hmac class implementation

BEGIN_NAMESPACE_RCZIP

void RCSha1Pbkdf2Hmac::Pbkdf2Hmac( const byte_t* pwd, 
                                   size_t pwdSize, 
                                   const byte_t* salt, 
                                   size_t saltSize,
                                   uint32_t numIterations, 
                                   byte_t* key, 
                                   size_t keySize
                                   )
{
    RCSha1Hmac baseCtx;
    baseCtx.SetKey(pwd, pwdSize);
    for (uint32_t i = 1; keySize > 0; ++i)
    {
        RCSha1Hmac ctx = baseCtx;
        ctx.Update(salt, saltSize);
        byte_t u[RCSha1Defs::kDigestSize] = { (byte_t)(i >> 24), (byte_t)(i >> 16), (byte_t)(i >> 8), (byte_t)(i) };
        const uint32_t curSize = (keySize < RCSha1Defs::kDigestSize) ? (uint32_t)keySize : RCSha1Defs::kDigestSize;
        ctx.Update(u, 4);
        ctx.Final(u, RCSha1Defs::kDigestSize);

        uint32_t s = 0 ;
        for (s = 0; s < curSize; ++s)
        {
            key[s] = u[s];
        }
    
        for (uint32_t j = numIterations; j > 1; --j)
        {
            ctx = baseCtx;
            ctx.Update(u, RCSha1Defs::kDigestSize);
            ctx.Final(u, RCSha1Defs::kDigestSize);
            for (s = 0; s < curSize; ++s)
            {
                key[s] ^= u[s];
            }
        }
        key += curSize;
        keySize -= curSize;
    }
}

void RCSha1Pbkdf2Hmac::Pbkdf2Hmac32( const byte_t* pwd, 
                                     size_t pwdSize, 
                                     const uint32_t* salt,
                                     size_t saltSize,
                                     uint32_t numIterations, 
                                     uint32_t* key,
                                     size_t keySize
                                    )
{
    RCSha1Hmac32 baseCtx;
    baseCtx.SetKey(pwd, pwdSize);
    for (uint32_t i = 1; keySize > 0; ++i)
    {
        RCSha1Hmac32 ctx = baseCtx;
        ctx.Update(salt, saltSize);
        uint32_t u[RCSha1Defs::kDigestSizeInWords] = { i };
        const uint32_t curSize = (keySize < RCSha1Defs::kDigestSizeInWords) ? (uint32_t)keySize : RCSha1Defs::kDigestSizeInWords;
        ctx.Update(u, 1);
        ctx.Final(u, RCSha1Defs::kDigestSizeInWords);

        // Speed-optimized code start
        ctx = baseCtx;
        ctx.GetLoopXorDigest(u, numIterations - 1);
        // Speed-optimized code end
    
        uint32_t s = 0 ;
        for (s = 0; s < curSize; s++)
        {
            key[s] = u[s];
        }
        key += curSize;
        keySize -= curSize;
    }
}

END_NAMESPACE_RCZIP
