/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/AES/RCAesCbcDecoder.h"
#include "crypto/AES/RCAesTableInit.h"
#include "algorithm/Aes.h"

/////////////////////////////////////////////////////////////////
//RCAesCbcDecoder class implementation

BEGIN_NAMESPACE_RCZIP

struct RCAesCbcDecoder::TAesCbcImpl
{
    CAesCbc m_aes ;
};

RCAesCbcDecoder::RCAesCbcDecoder()
{
    RCAesTableInitImpl& init = RCAesTableInit::Instance() ;
    (void)init ;
    m_impl = new TAesCbcImpl ;
}

RCAesCbcDecoder::~RCAesCbcDecoder()
{
    if(m_impl)
    {
        delete m_impl ;
        m_impl = NULL ;
    }
}

HResult RCAesCbcDecoder::Init()
{
    return RC_S_OK ;
}

uint32_t RCAesCbcDecoder::Filter(byte_t* data, uint32_t size)
{
    return (uint32_t)AesCbc_Decode(&(m_impl->m_aes), data, size);
}

HResult RCAesCbcDecoder::SetKey(const byte_t* data, uint32_t size)
{
    if ((size & 0x7) != 0 || size < 16 || size > 32)
    {
        return RC_E_INVALIDARG;
    }
    Aes_SetKeyDecode(&(m_impl->m_aes.aes), data, size);
    return RC_S_OK;
}

HResult RCAesCbcDecoder::SetInitVector(const byte_t *data, uint32_t size)
{
    if (size != AES_BLOCK_SIZE)
    {
        return RC_E_INVALIDARG;
    }
    AesCbc_Init(&(m_impl->m_aes), data);
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
