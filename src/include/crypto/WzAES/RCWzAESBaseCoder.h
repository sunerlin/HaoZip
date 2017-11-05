/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWzAESBaseCoder_h_
#define __RCWzAESBaseCoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "crypto/WzAES/RCWzAESKeyInfo.h"
#include "crypto/hash/RCSha1Hmac.h"
#include "algorithm/Aes.h"

BEGIN_NAMESPACE_RCZIP

/** WzAES 编码器
*/
class RCWzAESBaseCoder:
    public IUnknownImpl2<ICompressFilter,
                         ICryptoSetPassword
                        >
{
public:

    /** 默认构造函数
    */
    RCWzAESBaseCoder() ;
    
    /** 默认析构函数
    */
    ~RCWzAESBaseCoder() ;
    
public:

    /** 返回头大小
    @return 返回头大小
    */
    uint32_t GetHeaderSize() const ;
        
protected:

    /** 数据编码
    @param [in] data 数据缓存
    @param [in] size 数据大小
    */
    void EncryptData(byte_t *data, uint32_t size);
        
    /** 初始化
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Init() ;
    
    /** 过滤数据
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 实际处理数据的长度
    */
    virtual uint32_t Filter(byte_t* data, uint32_t size) = 0 ;
    
    /** 设置密码
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetPassword(const byte_t* data, uint32_t size) ;
    
protected:

    /** key
    */
    RCWzAESKeyInfo m_key ;

    /** 计数
    */
    uint32_t m_counter[AES_BLOCK_SIZE / 4];

    /** 缓存
    */
    byte_t m_buffer[AES_BLOCK_SIZE];

    /** hmac
    */
    RCSha1Hmac m_hmac;

    /** 块位置
    */
    uint32_t m_blockPos;

    /** 密码校验
    */
    byte_t m_pwdVerifFromArchive[RCWzAESDefs::s_kPwdVerifCodeSize];

    /** Aes
    */
    CAes m_aes ;
};

END_NAMESPACE_RCZIP

#endif //__RCWzAESBaseCoder_h_
