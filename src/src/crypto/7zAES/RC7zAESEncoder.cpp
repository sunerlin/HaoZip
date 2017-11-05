/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/7zAES/RC7zAESEncoder.h"
#include "crypto/AES/RCAesCbcEncoder.h"
#include "crypto/hash/RCRandomGenerator.h"
#include "interface/IStream.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RC7zAESEncoder class implementation

BEGIN_NAMESPACE_RCZIP

RC7zAESEncoder::RC7zAESEncoder()
{
}

RC7zAESEncoder::~RC7zAESEncoder()
{
}

HResult RC7zAESEncoder::QueryInterface(RC_IID iid, void** outObject) 
{
    if(outObject == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    if(RC7zAESBaseCoder::QueryInterface(iid,outObject) == RC_S_OK)
    {
        return RC_S_OK ;
    }        
    else if(iid == ICompressWriteCoderProperties::IID)
    {
        ICompressWriteCoderProperties* ptr = (ICompressWriteCoderProperties*)this ;        
        ptr->AddRef() ;
        *outObject = ptr ;
        return RC_S_OK ;
    }
    else if(iid == ICryptoResetInitVector::IID)
    {
        ICryptoResetInitVector* ptr = (ICryptoResetInitVector*)this ;
        ptr->AddRef() ;
        *outObject = ptr ;        
        return RC_S_OK ;
    }
    else
    {
        return RC_E_NOINTERFACE ;
    }
}

void RC7zAESEncoder::AddRef(void)
{
    RCRefCounted::Increase() ;
}
    
void RC7zAESEncoder::Release(void)
{
    RCRefCounted::Decrease() ;
}

HResult RC7zAESEncoder::CreateFilter() 
{
    m_aesFilter = new RCAesCbcEncoder ;
    return RC_S_OK ;
}

HResult RC7zAESEncoder::WriteCoderProperties(ISequentialOutStream* outStream)
{
    for (uint32_t i = m_ivSize; i < sizeof(m_iv); i++)
    {
        m_iv[i] = 0;
    }

    uint32_t ivSize = m_ivSize ;  
    m_key.m_numCyclesPower = 19 ;

    byte_t firstByte = (byte_t)(m_key.m_numCyclesPower |
                                (((m_key.m_saltSize == 0) ? 0 : 1) << 7) |
                                (((ivSize == 0) ? 0 : 1) << 6)) ;
    HResult hr = outStream->Write(&firstByte, 1, NULL) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    if (m_key.m_saltSize == 0 && ivSize == 0)
    {
        return RC_S_OK;
    }
    byte_t saltSizeSpec = (byte_t)((m_key.m_saltSize == 0) ? 0 : (m_key.m_saltSize - 1));
    byte_t ivSizeSpec = (byte_t)((ivSize == 0) ? 0 : (ivSize - 1));
    byte_t secondByte = (byte_t)(((saltSizeSpec) << 4) | ivSizeSpec);
    hr = outStream->Write(&secondByte, 1, NULL) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
     if (m_key.m_saltSize > 0)
    {
        hr = RCStreamUtils::WriteStream(outStream, m_key.m_salt, m_key.m_saltSize) ;
        if(hr != RC_S_OK)
        {
            return hr ;
        }
    }
    if (ivSize > 0)
    {
        hr = RCStreamUtils::WriteStream(outStream, m_iv, ivSize) ;
        if(hr != RC_S_OK)
        {
            return hr ;
        }
    }
    return RC_S_OK;
}

HResult RC7zAESEncoder::ResetInitVector()
{
    m_ivSize = 8;
    RCRandomGenerator::Instance().Generate(m_iv, (unsigned)m_ivSize);
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
