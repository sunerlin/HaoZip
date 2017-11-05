/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zAESBase_h_
#define __RC7zAESBase_h_ 1

#include "base/RCNonCopyable.h"
#include "crypto/7zAES/RC7zAESKeyInfoCache.h"
#include "thread/RCMutex.h"

BEGIN_NAMESPACE_RCZIP

/** 7z AES 基类
*/
class RC7zAESBase:
    private RCNonCopyable
{
protected:

    /** 默认构造函数
    */
    RC7zAESBase() ;

    /** 计算摘要
    */
    void CalculateDigest();
     
protected:

    /** key
    */
    RC7zAESKeyInfo m_key ;

    /** iv
    */
    byte_t m_iv[16] ;

    /** iv Size
    */
    uint32_t m_ivSize ;
     
private:

    /** Cached Keys
    */
    RC7zAESKeyInfoCache m_cachedKeys ;
    
private:

    /** Global Key Cache
    */
    static RC7zAESKeyInfoCache s_globalKeyCache ;

    /** lock
    */
    static RCMutex s_lock ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zAESBase_h_
