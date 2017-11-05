/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDeflateCOMEncoder64_h_
#define __RCDeflateCOMEncoder64_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/deflate/RCDeflateEncoder.h"

BEGIN_NAMESPACE_RCZIP

/** Deflate 64编码器
*/
class RCDeflateCOMEncoder64:
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetCoderProperties
                        >,
    public RCDeflateEncoder
{
public:

    /** 默认构造函数
    */
    RCDeflateCOMEncoder64() ;
    
    /** 默认析构函数
    */
    virtual ~RCDeflateCOMEncoder64() ;
    
public:
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
};

END_NAMESPACE_RCZIP

#endif //__RCDeflateCOMEncoder64_h_
