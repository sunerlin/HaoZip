/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCZipStrongDecoder_h_
#define __RCZipStrongDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Zip Strong Key 结构
*/
struct RCZipStrongKeyInfo
{

    /** 主Key
    */
    byte_t m_masterKey[32] ;
    
    /** Key 大小
    */
    uint32_t m_keySize ;
    
    /** 设置密码
    @param [in] data 数据
    @param [in] size 大小
    */
    void SetPassword(const byte_t* data, uint32_t size) ;
};

/** Zip Strong 解码器基类
*/
class RCZipStrongBaseDecoder:
    public IUnknownImpl2<ICompressFilter,
                         ICryptoSetPassword>
{
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
    RCZipStrongKeyInfo m_key ;
    
    /** aes 过滤器
    */
    ICompressFilterPtr m_aesFilter ;
    
    /** 缓存
    */
    RCByteBuffer m_buf ;
};

/** Zip Strong 解码器
*/
class RCZipStrongDecoder:
    public RCZipStrongBaseDecoder
{
public:

    /** 过滤数据
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 实际处理数据的长度
    */
    uint32_t Filter(byte_t* data, uint32_t size) ;

    /** 读取头标记
    @param [in] inStream 输入流
    @param [in] crc crc校验
    @param [in] unpackSize 解压后大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadHeader(ISequentialInStream *inStream, uint32_t crc, uint64_t unpackSize) ;
    
    /** 检查密码
    @param [out] isPasswordOK 密码是否正确
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CheckPassword(bool &isPasswordOK) ;
    
private:
    
    /** iv大小
    */
    uint32_t m_ivSize ;
    
    /** iv
    */
    byte_t m_iv[16] ;
    
    /** rem大小
    */
    uint32_t m_remSize ;
};

END_NAMESPACE_RCZIP

#endif //__RCZipStrongDecoder_h_
