/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCAdcDecoderInfo_h_
#define __RCAdcDecoderInfo_h_ 1

#include "common/RCCodecInfoBase.h"

BEGIN_NAMESPACE_RCZIP

/** Adc 解码器信息
*/
class RCAdcDecoderInfo:
    public RCCodecInfoBase
{
public:

    /** 默认构造函数
    */
    RCAdcDecoderInfo() ;
    
    /** 默认析构函数
    */
    ~RCAdcDecoderInfo() ;
    
public:
    
    /** 获取创建解码器的函数
    @return 返回解码器创建函数
    */
    virtual PFNCreateCodec GetCreateDecoderFunc(void) const ;
    
    /** 获取解码器的接口ID, 为IID_ICompressCoder or IID_ICompressCoder2 or IID_ICompressFilter
    @return 返回解码器ID
    */
    virtual RC_IID GetDecoderIID(void) const ;
    
    /** 获取创建编码器的函数
    @return 返回编码器创建函数
    */
    virtual PFNCreateCodec GetCreateEncoderFunc(void) const ;
    
    /** 获取编码器的接口ID
    @return 返回编码器ID
    */
    virtual RC_IID GetEncoderIID(void) const ;
    
    /** 获取编码ID
    @return 返回编码ID
    */
    virtual RCMethodID GetMethodID(void) const ;
    
    /** 获取编码名称
    @return 返回编码名称
    */
    virtual RCString GetMethodName(void) const ;
    
    /** 获取InStream的个数
    @return 返回输入流个数
    */
    virtual uint32_t GetNumInStreams(void) const ;
    
    /** 获取输出流的个数
    @return 返回输出流的个数
    */
    virtual uint32_t GetNumOutStreams(void) const ;
    
    /** 是否定义编码器
    @return 如果定义编码器返回true，否则返回false
    */
    virtual bool IsEncoderAssigned(void) const ;
    
    /** 是否定义解码器
    @return 如果定义解码器返回true，否则返回false
    */
    virtual bool IsDecoderAssigned(void) const ;
    
    /** 是否为简单编码器
    @return 如果只含有一个输入流和一个输出流返回true,否则返回false
    */
    virtual bool IsSimpleCodec(void) const ;
    
    /** 获取Filter属性
    @return 如果是过滤编码器返回true,否则返回false
    */
    virtual bool IsFilter(void) const ;
};

END_NAMESPACE_RCZIP

#endif //__RCAdcDecoderInfo_h_
