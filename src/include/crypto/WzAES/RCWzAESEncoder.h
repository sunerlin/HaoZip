/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWzAESEncoder_h_
#define __RCWzAESEncoder_h_ 1

#include "crypto/WzAES/RCWzAESBaseCoder.h"

BEGIN_NAMESPACE_RCZIP

/** 序列化输入流
*/
class ISequentialOutStream ;

/** WzAES 编码器
*/
class RCWzAESEncoder:
    public RCWzAESBaseCoder
{
public:

    /** 默认构造函数
    */
    RCWzAESEncoder() ;
    
    /** 默认析构函数
    */
    ~RCWzAESEncoder() ;
    
public:

    /** 写头标记
    @param [in] outStream 输出流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteHeader(ISequentialOutStream* outStream) ;

    /** 写尾标记
    @param [in] outStream 输出流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult WriteFooter(ISequentialOutStream* outStream) ;

    /** 设置key模式
    @param [in] mode 模式
    */
    bool SetKeyMode(byte_t mode) ;
        
    /** 过滤数据
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 实际处理数据的长度
    */
    virtual uint32_t Filter(byte_t* data, uint32_t size) ;
};

END_NAMESPACE_RCZIP

#endif //__RCWzAESEncoder_h_
