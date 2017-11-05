/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWavPackDecoder_h
#define __RCWavPackDecoder_h 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** WavPack 解码器
*/
class RCWavPackDecoder :
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetDecoderProperties2>
{
public:

    /** 默认构造函数
    */
    RCWavPackDecoder();

    /** 默认析构函数
    */
    ~RCWavPackDecoder();

public:

    /** 解码
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @param [in] progress 解压回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ; 

    /** 设置解码器属性
    @param [in] data 数据
    @param [in] size 数据大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;   
};

END_NAMESPACE_RCZIP

#endif //__RCWavPackDecoder_h
