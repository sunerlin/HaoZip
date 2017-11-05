/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzmaFiltersDecoder_h_
#define __RCLzmaFiltersDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "format/lzma/RCLzmaHeader.h"

BEGIN_NAMESPACE_RCZIP

class RCLzmaFiltersDecoder
{
public:
    
    /** 创建编码解码器
    @param [in] compressCodecsInfo 编码解码器指针
    @param [in] filteredMode 是否是过滤模式
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Create(ICompressCodecsInfo* compressCodecsInfo, 
                   bool filteredMode , 
                   ISequentialInStream* inStream) ;
    

    /** 解码
    @param [in] header 头部数据
    @param [in] outStream 输出流
    @param [in] progress 压缩进度信息指针接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Code(const RCLzmaHeader& header,
                 ISequentialOutStream* outStream, 
                 ICompressProgressInfo* progress) ;

    
    /** 获取输入流处理的数据大小
    @return 返回输入流处理的数据大小
    */
    uint64_t GetInputProcessedSize() const ;
    
    /** 释放输入流
    */
    void ReleaseInStream() ;
    
    /** 读取数据
    @param [in] data 存放读取结果的缓冲区地址
    @param [in] size 缓冲区的大小
    @param [in] processedSize 返回实际读到的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadInput(byte_t* data, uint32_t size, uint32_t* processedSize) ;
    
private:

    /** lzma解码器
    */
    ILzmaResumeDecoderPtr m_lzmaDecoderSpec ;

    /** lzma解码器
    */
    ICompressCoderPtr m_lzmaDecoder ;

    /** 输出流
    */
    ISequentialOutStreamPtr m_bcjStream ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzmaFiltersDecoder_h_
