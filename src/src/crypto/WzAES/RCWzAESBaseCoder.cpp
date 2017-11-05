/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/WzAES/RCWzAESBaseCoder.h"
#include "crypto/hash/RCSha1Pbkdf2Hmac.h"

/////////////////////////////////////////////////////////////////
//RCWzAESBaseCoder class implementation

BEGIN_NAMESPACE_RCZIP

#ifndef _NO_WZAES_OPTIMIZATIONS

static void BytesToBeUInt32s(const byte_t *src, uint32_t *dest, int destSize)
{
  for (int i = 0 ; i < destSize; i++)
      dest[i] =
          ((uint32_t)(src[i * 4 + 0]) << 24) |
          ((uint32_t)(src[i * 4 + 1]) << 16) |
          ((uint32_t)(src[i * 4 + 2]) <<  8) |
          ((uint32_t)(src[i * 4 + 3]));
}

#endif

#define SetUi32(p, d) { uint32_t x = (d); (p)[0] = (byte_t)x; (p)[1] = (byte_t)(x >> 8); \
                        (p)[2] = (byte_t)(x >> 16); (p)[3] = (byte_t)(x >> 24); }


RCWzAESBaseCoder::RCWzAESBaseCoder()
{
}

RCWzAESBaseCoder::~RCWzAESBaseCoder()
{
}

void RCWzAESBaseCoder::EncryptData(byte_t *data, uint32_t size)
{
    uint32_t pos = m_blockPos ;
    for (; size > 0; --size)
    {
        if (pos == AES_BLOCK_SIZE)
        {
            if (++m_counter[0] == 0)
            m_counter[1]++;
            uint32_t temp[4];
            Aes_Encode32(&m_aes, temp, m_counter);
            SetUi32(m_buffer,      temp[0]);
            SetUi32(m_buffer + 4,  temp[1]);
            SetUi32(m_buffer + 8,  temp[2]);
            SetUi32(m_buffer + 12, temp[3]);
            pos = 0;
        }
        *data++ ^= m_buffer[pos++];
    }
    m_blockPos = pos ;
}

uint32_t RCWzAESBaseCoder::GetHeaderSize() const
{
    return m_key.GetSaltSize() + RCWzAESDefs::s_kPwdVerifCodeSize;
}

HResult RCWzAESBaseCoder::Init()
{
    uint32_t keySize = m_key.GetKeySize();
    uint32_t keysTotalSize = 2 * keySize + RCWzAESDefs::s_kPwdVerifCodeSize;
    byte_t buf[2 * RCWzAESDefs::s_kAesKeySizeMax + RCWzAESDefs::s_kPwdVerifCodeSize];
  
    {
#ifdef _NO_WZAES_OPTIMIZATIONS
        RCSha1Pbkdf2Hmac::Pbkdf2Hmac( m_key.Password.data(), 
                                      m_key.Password.GetCapacity(),
                                      m_key.m_salt, 
                                      m_key.GetSaltSize(),
                                      RCWzAESDefs::s_kNumKeyGenIterations,
                                      buf,
                                      keysTotalSize) ;
#else
        uint32_t buf32[(2 * RCWzAESDefs::s_kAesKeySizeMax + RCWzAESDefs::s_kPwdVerifCodeSize + 3) / 4];
        uint32_t key32SizeTotal = (keysTotalSize + 3) / 4;
        uint32_t salt[RCWzAESDefs::s_kSaltSizeMax * 4];
        uint32_t saltSizeInWords = m_key.GetSaltSize() / 4;
        BytesToBeUInt32s(m_key.m_salt, salt, saltSizeInWords);
        RCSha1Pbkdf2Hmac::Pbkdf2Hmac32( m_key.m_password.data(), 
                                        m_key.m_password.GetCapacity(),
                                        salt, 
                                        saltSizeInWords,
                                        RCWzAESDefs::s_kNumKeyGenIterations,
                                        buf32, 
                                        key32SizeTotal);
        for (uint32_t j = 0; j < keysTotalSize; ++j)
        {
            buf[j] = (byte_t)(buf32[j / 4] >> (24 - 8 * (j & 3)));
        }    
#endif
    }
    m_hmac.SetKey(buf + keySize, keySize);
    memcpy(m_key.m_pwdVerifComputed, buf + 2 * keySize, RCWzAESDefs::s_kPwdVerifCodeSize);
  
    m_blockPos = AES_BLOCK_SIZE;
    for (int i = 0; i < 4; ++i)
    {
        m_counter[i] = 0;
    }
    Aes_SetKeyEncode(&m_aes, buf, keySize);
    return RC_S_OK ;
}

HResult RCWzAESBaseCoder::CryptoSetPassword(const byte_t* data, uint32_t size)
{
    if(data == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    if(size > RCWzAESDefs::s_kPasswordSizeMax)
    {
        return RC_E_Password_Too_Long;
    }
    m_key.m_password.SetCapacity(size);
    if(size > 0)
    {
        memcpy(m_key.m_password.data(), data, size);
    }
    return RC_S_OK ;
}
END_NAMESPACE_RCZIP
