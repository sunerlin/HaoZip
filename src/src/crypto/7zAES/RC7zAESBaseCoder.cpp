/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/7zAES/RC7zAESBaseCoder.h"

/////////////////////////////////////////////////////////////////
//RC7zAESBaseCoder class implementation

BEGIN_NAMESPACE_RCZIP

RC7zAESBaseCoder::RC7zAESBaseCoder()
{
}

RC7zAESBaseCoder::~RC7zAESBaseCoder()
{
}

HResult RC7zAESBaseCoder::Init()
{
    CalculateDigest() ;
    if(m_aesFilter == NULL)
    {
        HResult hr = CreateFilter() ;
        if(hr != RC_S_OK)
        {
            return hr ;
        }
    }
    ICryptoPropertiesPtr cp ;
    HResult hr = m_aesFilter.QueryInterface(IID_ICryptoProperties, cp.GetAddress()) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    hr = cp->SetKey(m_key.m_key, sizeof(m_key.m_key)) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    hr = cp->SetInitVector(m_iv, sizeof(m_iv)) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    return RC_S_OK ;
}

uint32_t RC7zAESBaseCoder::Filter(byte_t* data, uint32_t size)
{
    return m_aesFilter->Filter(data, size) ;
}

HResult RC7zAESBaseCoder::CryptoSetPassword(const byte_t* data, uint32_t size)
{
    m_key.m_password.SetCapacity((size_t)size);
    if( (size > 0) && (data != NULL) )
    {
        memcpy(m_key.m_password.data(), data, (size_t)size);
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP
