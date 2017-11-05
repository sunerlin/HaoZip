/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/Zip/RCZipCipher.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCZipCipher class implementation

BEGIN_NAMESPACE_RCZIP

RCZipCipher::RCZipCipher()
{
}

RCZipCipher::~RCZipCipher()
{
}

void RCZipCipher::UpdateKeys(byte_t b)
{
    m_keys[0] = CRC_UPDATE_BYTE(m_keys[0], b);
    m_keys[1] += m_keys[0] & 0xff;
    m_keys[1] = m_keys[1] * 134775813L + 1;
    m_keys[2] = CRC_UPDATE_BYTE(m_keys[2], (byte_t)(m_keys[1] >> 24));
}

byte_t RCZipCipher::DecryptByteSpec()
{
    uint32_t temp = m_keys[2] | 2;
    return (byte_t)((temp * (temp ^ 1)) >> 8);
}

void RCZipCipher::SetPassword(const byte_t *password, uint32_t passwordLength)
{
    if(password == NULL)
    {
        return ;
    }
    m_keys[0] = 305419896L;
    m_keys[1] = 591751049L;
    m_keys[2] = 878082192L;
    for (uint32_t i = 0; i < passwordLength; i++)
    {
        UpdateKeys(password[i]);
    }
}

byte_t RCZipCipher::DecryptByte(byte_t encryptedByte)
{
    byte_t c = (byte_t)(encryptedByte ^ DecryptByteSpec());
    UpdateKeys(c);
    return c ;
}

byte_t RCZipCipher::EncryptByte(byte_t b)
{
    byte_t c = (byte_t)(b ^ DecryptByteSpec());
    UpdateKeys(b);
    return c;
}

void RCZipCipher::DecryptHeader(byte_t *buffer)
{
    for (int i = 0; i < 12; i++)
    {
        buffer[i] = DecryptByte(buffer[i]);
    }
}

void RCZipCipher::EncryptHeader(byte_t *buffer)
{
    for (int i = 0; i < 12; i++)
    {
        buffer[i] = EncryptByte(buffer[i]);
    }
}

END_NAMESPACE_RCZIP
