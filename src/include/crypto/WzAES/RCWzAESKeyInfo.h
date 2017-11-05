/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWzAESKeyInfo_h_
#define __RCWzAESKeyInfo_h_ 1

#include "base/RCNonCopyable.h"
#include "crypto/WzAES/RCWzAESDefs.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** WzAES key信息
*/
class RCWzAESKeyInfo:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCWzAESKeyInfo() ;
    
    /** 默认析构函数
    */
    ~RCWzAESKeyInfo() ;
    
public:

    /** 初始化
    */
    void Init() ;

    /** 返回key大小
    @return 返回key大小
    */
    uint32_t GetKeySize() const ;

    /** 返回salt大小
    @return 返回salt大小
    */
    uint32_t GetSaltSize() const ;

public:

    /** kei 大小模式
    */
    byte_t m_keySizeMode; // 1 - 128-bit , 2 - 192-bit , 3 - 256-bit

    /** salt
    */
    byte_t m_salt[RCWzAESDefs::s_kSaltSizeMax];

    /** 密码校验
    */ 
    byte_t m_pwdVerifComputed[RCWzAESDefs::s_kPwdVerifCodeSize];
    
    /** 密码
    */
    RCByteBuffer m_password ;
};

END_NAMESPACE_RCZIP

#endif //__RCWzAESKeyInfo_h_
