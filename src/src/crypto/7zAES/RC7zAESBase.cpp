/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "crypto/7zAES/RC7zAESBase.h"

/////////////////////////////////////////////////////////////////
//RC7zAESBase class implementation

BEGIN_NAMESPACE_RCZIP

RC7zAESKeyInfoCache RC7zAESBase::s_globalKeyCache(32) ;
RCMutex RC7zAESBase::s_lock ;

RC7zAESBase::RC7zAESBase():
    m_cachedKeys(16),
    m_ivSize(0)
{
    for (size_t i = 0; i < sizeof(m_iv); ++i)
    {
        m_iv[i] = 0;
    }
}

void RC7zAESBase::CalculateDigest()
{
    RCMutexLock spLock(&s_lock);
    if (m_cachedKeys.Find(m_key))
    {
        s_globalKeyCache.Add(m_key);
    }
    else
    {
        if (!s_globalKeyCache.Find(m_key))
        {
            m_key.CalculateDigest();
            s_globalKeyCache.Add(m_key);
        }
        m_cachedKeys.Add(m_key);
    }
}

END_NAMESPACE_RCZIP
