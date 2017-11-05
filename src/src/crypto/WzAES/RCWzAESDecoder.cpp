/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/WzAES/RCWzAESDecoder.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCWzAESDecoder class implementation

BEGIN_NAMESPACE_RCZIP

static bool CompareArrays(const byte_t *p1, const byte_t *p2, uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        if (p1[i] != p2[i])
        {
            return false;
        }
    }
    return true ;
}

RCWzAESDecoder::RCWzAESDecoder()
{
}

RCWzAESDecoder::~RCWzAESDecoder()
{
}

HResult RCWzAESDecoder::ReadHeader(ISequentialInStream* inStream)
{
    uint32_t saltSize = m_key.GetSaltSize();
    uint32_t extraSize = saltSize + RCWzAESDefs::s_kPwdVerifCodeSize;
    byte_t temp[RCWzAESDefs::s_kSaltSizeMax + RCWzAESDefs::s_kPwdVerifCodeSize];
    HResult hr = RCStreamUtils::ReadStream_FAIL(inStream, temp, extraSize) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    uint32_t i;
    for (i = 0; i < saltSize; i++)
    {
        m_key.m_salt[i] = temp[i];
    }
    for (i = 0; i < RCWzAESDefs::s_kPwdVerifCodeSize; i++)
    {
        m_pwdVerifFromArchive[i] = temp[saltSize + i];
    }
    return RC_S_OK ;
}

bool RCWzAESDecoder::CheckPasswordVerifyCode()
{
    return CompareArrays(m_key.m_pwdVerifComputed, m_pwdVerifFromArchive, RCWzAESDefs::s_kPwdVerifCodeSize);
}

HResult RCWzAESDecoder::CheckMac(ISequentialInStream* inStream, bool& isOK)
{
    isOK = false;
    byte_t mac1[RCWzAESDefs::s_kMacSize];
    HResult hr = RCStreamUtils::ReadStream_FAIL(inStream, mac1, RCWzAESDefs::s_kMacSize) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    byte_t mac2[RCWzAESDefs::s_kMacSize];
    m_hmac.Final(mac2, RCWzAESDefs::s_kMacSize);
    isOK = CompareArrays(mac1, mac2, RCWzAESDefs::s_kMacSize);
    return RC_S_OK ;
}

HResult RCWzAESDecoder::QueryInterface(RC_IID iid, void** outObject) 
{
    if(outObject == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    if(RCWzAESBaseCoder::QueryInterface(iid,outObject) == RC_S_OK)
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

void RCWzAESDecoder::AddRef(void)
{
    RCRefCounted::Increase() ;
}
    
void RCWzAESDecoder::Release(void)
{
    RCRefCounted::Decrease() ;
}

uint32_t RCWzAESDecoder::Filter(byte_t* data, uint32_t size) 
{
    m_hmac.Update(data, size) ;
    EncryptData(data, size) ;
    return size ;
}

HResult RCWzAESDecoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size != 1)
    {
        return RC_E_INVALIDARG;
    }
    m_key.Init();
    byte_t keySizeMode = data[0];
    if (keySizeMode < 1 || keySizeMode > 3)
    {
        return RC_E_INVALIDARG;
    }
    m_key.m_keySizeMode = keySizeMode;
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
