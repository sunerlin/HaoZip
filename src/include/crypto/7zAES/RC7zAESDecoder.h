/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zAESDecoder_h_
#define __RC7zAESDecoder_h_ 1

#include "crypto/7zAES/RC7zAESBaseCoder.h"

BEGIN_NAMESPACE_RCZIP

/** 7z AES 解码器
*/
class RC7zAESDecoder:
    public RC7zAESBaseCoder,
    public ICompressSetDecoderProperties2
{
public:

    /** 默认构造函数
    */
    RC7zAESDecoder() ;
    
    /** 默认析构函数
    */
    ~RC7zAESDecoder() ;
    
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject) ;
    
    /** 增加引用计数
    */
    virtual void AddRef(void) ;
    
    /** 减少引用计数
    */
    virtual void Release(void) ;
    
    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;

private:

    /** 创建过滤器
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CreateFilter() ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zAESDecoder_h_
