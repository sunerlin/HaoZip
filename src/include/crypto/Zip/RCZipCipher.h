/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipCipher_h_
#define __RCZipCipher_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** Zip Cipher
*/
class RCZipCipher:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCZipCipher() ;
    
    /** 默认析构函数
    */
    ~RCZipCipher() ;
    
public:

    /** 设置密码
    @param [in] password 密码
    @param [in] passwordLength 密码长度
    */
    void SetPassword(const byte_t *password, uint32_t passwordLength);

    /** 数据解码
    @param [in] encryptedByte 加密数据
    @return 解码后数据
    */
    byte_t DecryptByte(byte_t encryptedByte);

    /** 数据编码
    @param [in] b 编码前数据
    @return 编码后数据
    */
    byte_t EncryptByte(byte_t b);

    /** 解码头标记
    @param [in] buffer 数据缓存
    */
    void DecryptHeader(byte_t *buffer);

    /** 编码头标记
    @param [in] buffer 数据缓存
    */
    void EncryptHeader(byte_t *buffer);
    
private:

    /** 更新key 
    @param [in] b 数据
    */
    void UpdateKeys(byte_t b) ;

    /** 解码字节
    @return 返回解码字节
    */
    byte_t DecryptByteSpec() ;
        
public:
    
    /** 头标记大小
    */
    static const int32_t s_headerSize = 12 ;
        
private:

    /** keys
    */
    uint32_t m_keys[3] ;    
};

END_NAMESPACE_RCZIP

#endif //__RCZipCipher_h_
