/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeltaDecoder_h_
#define __RCDeltaDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "RCDelta.h"

BEGIN_NAMESPACE_RCZIP

/** Delta 解码器
*/
class RCDeltaDecoder:
    public RCDelta,
    public IUnknownImpl2<ICompressFilter,
                         ICompressSetDecoderProperties2>
{
public:
    
    /** 初始化
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
};

END_NAMESPACE_RCZIP

#endif //__RCDeltaDecoder_h_
