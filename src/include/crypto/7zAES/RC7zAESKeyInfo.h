/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zAESKeyInfo_h_
#define __RC7zAESKeyInfo_h_ 1

#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** 7z AES Key
*/
class RC7zAESKeyInfo
{
public:

    /** Key Size
    */
    static const int32_t s_kKeySize = 32 ;
    
public:
    /** 构造函数
    */
    RC7zAESKeyInfo() ;
    
    /** 初始化
    */
    void Init() ;
    
    /** 是否相等
    @param [in] a 比较值
    @return 相等返回true,否则返回false
    */
    bool IsEqualTo(const RC7zAESKeyInfo& a) const;

    /** 计算摘要
    */
    void CalculateDigest();
    
public:
    
    /** Cycles Power
    */
    int32_t m_numCyclesPower ;

    /** salt Size
    */
    uint32_t m_saltSize ;

    /** salt
    */
    byte_t m_salt[16] ;

    /** 密码
    */
    RCByteBuffer m_password ;

    /** key
    */
    byte_t m_key[s_kKeySize] ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zAESKeyInfo_h_
