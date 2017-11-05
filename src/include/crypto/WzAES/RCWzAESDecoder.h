/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWzAESDecoder_h_
#define __RCWzAESDecoder_h_ 1

#include "crypto/WzAES/RCWzAESBaseCoder.h"

BEGIN_NAMESPACE_RCZIP

/** WzAES 解码器
*/
class RCWzAESDecoder:
    public RCWzAESBaseCoder,
    public ICompressSetDecoderProperties2
{
public:

    /** 默认构造函数
    */
    RCWzAESDecoder() ;
    
    /** 默认析构函数
    */
    ~RCWzAESDecoder() ;
    
public:

    /** 读取头数据
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadHeader(ISequentialInStream* inStream);

    /** 检查密码
    @return 成功返回true,否则返回false
    */
    bool CheckPasswordVerifyCode();

    /** 检查mac
    @param [in] inStream 输入流
    @param [out] isOK 是否正确
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CheckMac(ISequentialInStream* inStream, bool& isOK);
        
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
};

END_NAMESPACE_RCZIP

#endif //__RCWzAESDecoder_h_
