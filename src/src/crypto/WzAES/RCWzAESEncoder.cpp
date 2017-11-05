/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/WzAES/RCWzAESEncoder.h"
#include "crypto/hash/RCRandomGenerator.h"
#include "filesystem/RCStreamUtils.h"

/////////////////////////////////////////////////////////////////
//RCWzAESEncoder class implementation

BEGIN_NAMESPACE_RCZIP

RCWzAESEncoder::RCWzAESEncoder()
{
}

RCWzAESEncoder::~RCWzAESEncoder()
{
}

HResult RCWzAESEncoder::WriteHeader(ISequentialOutStream* outStream)
{
    uint32_t saltSize = m_key.GetSaltSize() ;
    RCRandomGenerator::Instance().Generate(m_key.m_salt, saltSize);
    Init();
    HResult hr = RCStreamUtils::WriteStream(outStream, m_key.m_salt, saltSize) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    return RCStreamUtils::WriteStream(outStream, m_key.m_pwdVerifComputed, RCWzAESDefs::s_kPwdVerifCodeSize);
}

HResult RCWzAESEncoder::WriteFooter(ISequentialOutStream* outStream)
{
    byte_t mac[RCWzAESDefs::s_kMacSize];
    m_hmac.Final(mac, RCWzAESDefs::s_kMacSize);
    return RCStreamUtils::WriteStream(outStream, mac, RCWzAESDefs::s_kMacSize);
}

bool RCWzAESEncoder::SetKeyMode(byte_t mode)
{
    if (mode < 1 || mode > 3)
    {
        return false;
    }
    m_key.m_keySizeMode = mode ;
    return true ;
}

uint32_t RCWzAESEncoder::Filter(byte_t* data, uint32_t size)
{
    EncryptData(data, size) ;
    m_hmac.Update(data, size) ;
    return size ;
}

END_NAMESPACE_RCZIP
