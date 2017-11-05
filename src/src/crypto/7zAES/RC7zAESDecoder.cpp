/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/7zAES/RC7zAESDecoder.h"
#include "crypto/AES/RCAesCbcDecoder.h"

/////////////////////////////////////////////////////////////////
//RC7zAESDecoder class implementation

BEGIN_NAMESPACE_RCZIP

RC7zAESDecoder::RC7zAESDecoder()
{
}

RC7zAESDecoder::~RC7zAESDecoder()
{
}

HResult RC7zAESDecoder::QueryInterface(RC_IID iid, void** outObject) 
{
    if(outObject == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    if(RC7zAESBaseCoder::QueryInterface(iid,outObject) == RC_S_OK)
    {
        return RC_S_OK ;
    }        
    else if(iid == ICompressSetDecoderProperties2::IID)
    {
        ICompressSetDecoderProperties2* ptr = (ICompressSetDecoderProperties2*)this ;        
        ptr->AddRef() ;
        *outObject = ptr ;
        return RC_S_OK ;
    }
    else
    {
        return RC_E_NOINTERFACE ;
    }
}

void RC7zAESDecoder::AddRef(void)
{
    RCRefCounted::Increase() ;
}
    
void RC7zAESDecoder::Release(void)
{
    RCRefCounted::Decrease() ;
}

HResult RC7zAESDecoder::CreateFilter()
{
    m_aesFilter = new RCAesCbcDecoder;
    return RC_S_OK ;
}

HResult RC7zAESDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    m_key.Init();
    uint32_t i = 0 ;
    for (i = 0; i < sizeof(m_iv); i++)
    {
        m_iv[i] = 0;
    }
    if (size == 0)
    {
        return RC_S_OK ;
    }
    uint32_t pos = 0;
    byte_t firstByte = data[pos++];

    m_key.m_numCyclesPower = firstByte & 0x3F;
    if ((firstByte & 0xC0) == 0)
    {
        return RC_S_OK;
    }
    m_key.m_saltSize = (firstByte >> 7) & 1;
    uint32_t ivSize = (firstByte >> 6) & 1;

    if (pos >= size)
    {
        return RC_E_INVALIDARG;
    }
    byte_t secondByte = data[pos++];
  
    m_key.m_saltSize += (secondByte >> 4);
    ivSize += (secondByte & 0x0F);
  
    if (pos + m_key.m_saltSize + ivSize > size)
    {
        return RC_E_INVALIDARG;
    }
    for (i = 0; i < m_key.m_saltSize; i++)
    {
        m_key.m_salt[i] = data[pos++];
    }
    for (i = 0; i < ivSize; i++)
    {
        m_iv[i] = data[pos++];
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
