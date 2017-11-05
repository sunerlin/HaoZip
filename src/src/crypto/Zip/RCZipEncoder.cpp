/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/Zip/RCZipEncoder.h"
#include "filesystem/RCStreamUtils.h"
#include "crypto/hash/RCRandomGenerator.h"

/////////////////////////////////////////////////////////////////
//RCZipEncoder class implementation

BEGIN_NAMESPACE_RCZIP

RCZipEncoder::RCZipEncoder():
    m_crc(0)
{
}

RCZipEncoder::~RCZipEncoder()
{
}

HResult RCZipEncoder::Init()
{
    return RC_S_OK ;
}

uint32_t RCZipEncoder::Filter(byte_t* data, uint32_t size)
{
    uint32_t i = 0 ;
    for (i = 0; i < size; ++i)
    {
        data[i] = m_cipher.EncryptByte(data[i]);
    }
    return i;
}

HResult RCZipEncoder::CryptoSetPassword(const byte_t* data, uint32_t size)
{
    m_cipher.SetPassword(data, size);
    return RC_S_OK ;
}

HResult RCZipEncoder::CryptoSetCRC(uint32_t crc)
{
    m_crc = crc;
    return RC_S_OK ;
}

HResult RCZipEncoder::WriteHeader(ISequentialOutStream *outStream)
{
    const int32_t kHeaderSize = RCZipCipher::s_headerSize ;
    byte_t header[kHeaderSize];
    RCRandomGenerator::Instance().Generate(header, kHeaderSize - 2);

    header[kHeaderSize - 1] = byte_t(m_crc >> 24);
    header[kHeaderSize - 2] = byte_t(m_crc >> 16);

    m_cipher.EncryptHeader(header);
    return RCStreamUtils::WriteStream(outStream, header, kHeaderSize);
}

END_NAMESPACE_RCZIP
