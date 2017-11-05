/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/Zip/RCZipDecoder.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCZipDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RCZipDecoder::RCZipDecoder()
{
}

RCZipDecoder::~RCZipDecoder()
{
}

HResult RCZipDecoder::Init()
{
    return RC_S_OK ;
}

uint32_t RCZipDecoder::Filter(byte_t* data, uint32_t size)
{
    uint32_t i = 0 ;
    for (i = 0; i < size; ++i)
    {
        data[i] = m_cipher.DecryptByte(data[i]);
    }
    return i;
}

HResult RCZipDecoder::CryptoSetPassword(const byte_t* data, uint32_t size)
{
    m_cipher.SetPassword(data, size);
    return RC_S_OK ;
}

HResult RCZipDecoder::ReadHeader(ISequentialInStream *inStream)
{
    const int32_t kHeaderSize = RCZipCipher::s_headerSize ;
    byte_t header[kHeaderSize];
    HResult hr = RCStreamUtils::ReadStream_FAIL(inStream, header, kHeaderSize) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    m_cipher.DecryptHeader(header);
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
