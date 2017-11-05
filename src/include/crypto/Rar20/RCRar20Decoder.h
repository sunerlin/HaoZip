/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar20Decoder_h_
#define __RCRar20Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "crypto/Rar20/RCRar20Crypto.h"

BEGIN_NAMESPACE_RCZIP

/** Rar20 解码器
*/
class RCRar20Decoder:
    public IUnknownImpl2<ICompressFilter,
                         ICryptoSetPassword
                        >
{
public:

    /** 默认构造函数
    */
    RCRar20Decoder() ;
    
    /** 默认析构函数
    */
    ~RCRar20Decoder() ;
    
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
    
private:

    /** 解/编码器
    */
    RCRar20Crypto m_coder ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar20Decoder_h_
