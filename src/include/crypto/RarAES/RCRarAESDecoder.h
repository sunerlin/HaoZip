/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRarAESDecoder_h_
#define __RCRarAESDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBuffer.h"
#include "algorithm/Aes.h"

BEGIN_NAMESPACE_RCZIP

/** Rar AES 解码器
*/
class RCRarAESDecoder:
    public IUnknownImpl4<ICompressFilter,
                         ICompressSetDecoderProperties2,
                         ICryptoSetPassword,
                         ICompressSetCoderProperties>
{
public:

    /** 默认构造函数
    */
    RCRarAESDecoder() ;
    
    /** 默认析构函数
    */
    ~RCRarAESDecoder() ;
    
public:

    /** 初始化
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Init() ;
    
    /** 过滤数据
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 实际处理数据的长度
    */
    virtual uint32_t Filter(byte_t* data, uint32_t size) ;
    
    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;
    
    /** 设置密码
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetPassword(const byte_t* data, uint32_t size) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
    
private:

    /** 计算
    */
    void Calculate() ;
    
private:

    /** Key Size
    */
    static const uint32_t s_kRarAesKeySize = 16 ;

    /** Max Password Length
    */
    static const uint32_t s_kMaxPasswordLength = 127 * 2;
    
private:

    /** salt
    */
    byte_t m_salt[8];

    /** 是否Salt
    */
    bool m_thereIsSalt;

    /** 缓存
    */
    RCByteBuffer m_buffer;

    /** aes Key
    */
    byte_t m_aesKey[s_kRarAesKeySize];

    /** aes 初始值
    */
    byte_t m_aesInit[AES_BLOCK_SIZE];

    /** 是否计算
    */
    bool m_needCalculate;

    /** aes
    */
    CAesCbc m_aes;

    /** 是否rar350模式
    */
    bool m_rar350Mode;
};

END_NAMESPACE_RCZIP

#endif //__RCRarAESDecoder_h_
