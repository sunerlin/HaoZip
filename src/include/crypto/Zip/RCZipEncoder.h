/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipEncoder_h_
#define __RCZipEncoder_h_ 1

#include "crypto/Zip/RCZipCipher.h"
#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** 序列输入流
*/
class ISequentialOutStream ;

/** Zip 编码器
*/
class RCZipEncoder:
    public IUnknownImpl3<ICompressFilter,
                         ICryptoSetPassword,
                         ICryptoSetCRC>
{
public:

    /** 默认构造函数
    */
    RCZipEncoder() ;
    
    /** 默认析构函数
    */
    ~RCZipEncoder() ;
    
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
    
    /** 设置密码
    @param [in] data 数据缓冲区
    @param [in] size 数据长度
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetPassword(const byte_t* data, uint32_t size) ;
    
    /** 设置CRC值
    @param [in] crc CRC数值
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    virtual HResult CryptoSetCRC(uint32_t crc) ;
    
    /** 写头标记
    @param [in] outStream 输出流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteHeader(ISequentialOutStream *outStream) ;
    
private:

    /** zip cipher 编码器
    */
    RCZipCipher m_cipher;

    /** crc 校验值
    */
    uint32_t m_crc;
};

END_NAMESPACE_RCZIP

#endif //__RCZipEncoder_h_
