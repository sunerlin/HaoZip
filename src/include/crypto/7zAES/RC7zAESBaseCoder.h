/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zAESBaseCoder_h_
#define __RC7zAESBaseCoder_h_ 1


#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "crypto/7zAES/RC7zAESBase.h"

BEGIN_NAMESPACE_RCZIP

/** 7z AES Base 解码器
*/
class RC7zAESBaseCoder:
    public IUnknownImpl2<ICompressFilter,
                         ICryptoSetPassword
                        >,
    public RC7zAESBase
{
public:

    /** 默认构造函数
    */
    RC7zAESBaseCoder() ;
    
    /** 默认析构函数
    */
    ~RC7zAESBaseCoder() ;

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
    
protected:
    
    /** 创建过滤器
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CreateFilter() = 0 ;
    
protected:

    /** AES Filter
    */
    ICompressFilterPtr m_aesFilter ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zAESBaseCoder_h_
