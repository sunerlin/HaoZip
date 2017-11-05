/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/Rar20/RCRar20Decoder.h"

/////////////////////////////////////////////////////////////////
//RCRar20Decoder class implementation

BEGIN_NAMESPACE_RCZIP

RCRar20Decoder::RCRar20Decoder()
{
}

RCRar20Decoder::~RCRar20Decoder()
{
}

HResult RCRar20Decoder::Init() 
{
    return RC_S_OK ;
}

uint32_t RCRar20Decoder::Filter(byte_t* data, uint32_t size)
{
    const uint16_t kBlockSize = 16;
    if (size > 0 && size < kBlockSize)
    {
        return kBlockSize;
    }
    uint32_t i = 0 ;
    for (i = 0; i + kBlockSize <= size; i += kBlockSize)
    {
        m_coder.DecryptBlock(data + i);
     }
    return i ;
}

HResult RCRar20Decoder::CryptoSetPassword(const byte_t* data, uint32_t size)
{
    m_coder.SetPassword(data, size);
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
