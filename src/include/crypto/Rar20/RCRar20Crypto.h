/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar20Crypto_h_
#define __RCRar20Crypto_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** Rar20 加密
*/
class RCRar20Crypto:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRar20Crypto() ;
    
    /** 默认析构函数
    */
    ~RCRar20Crypto() ;
    
public:

    /** 块编码
    @param [in] buf 编码数据
    */
    void EncryptBlock(byte_t *buf) ;

    /** 块解码
    @param [in] buf 解码数据
    */
    void DecryptBlock(byte_t *buf) ;

    /** 设置密码
    @param [in] password 密码
    @param [in] passwordLength 长度
    */
    void SetPassword(const byte_t *password, uint32_t passwordLength);
    
private:

    /** SubstLong
    @param [in] t 长度
    */
    uint32_t SubstLong(uint32_t t) ;

    /** 更新key
    @param [in] data 数据
    */
    void UpdateKeys(const byte_t* data);

    /** 加密块
    @param [in] buf 数据缓存
    @param [in] encrypt 是否加密
    */
    void CryptBlock(byte_t* buf, bool encrypt);

    /** 交换
    @param [in] b1 字节1
    @param [in] b2 字节2
    */
    void Swap(byte_t *b1, byte_t *b2) ;

    /** 返回小头存储uint32
    @param [in] p 数据
    @return 返回uint32
    */
    uint32_t GetUInt32FromMemLE(const byte_t *p) ;

    /** uint32写入小头存储
    @param [in] v uint32值
    @param [in] p 数据缓存
    */
    void WriteUInt32ToMemLE(uint32_t v, byte_t *p) ;
        
private:

    /** 子表
    */
    byte_t m_substTable[256];

    /** keys
    */
    uint32_t m_keys[4];
};

END_NAMESPACE_RCZIP

#endif //__RCRar20Crypto_h_
