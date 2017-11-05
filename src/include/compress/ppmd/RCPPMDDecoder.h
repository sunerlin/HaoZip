/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPPMDDecoder_h_
#define __RCPPMDDecoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCOutBuffer.h"
#include "compress/ppmd/RCPPMDRangeDecoder.h"
#include "compress/ppmd/RCPPMDDecodeInfo.h"

BEGIN_NAMESPACE_RCZIP

/** PPMD 解码器
*/
class RCPPMDDecoder:
    public IUnknownImpl5<ICompressCoder,
                         ICompressSetDecoderProperties2,
                         ICompressSetInStream,
                         ICompressSetOutStreamSize,
                         ISequentialInStream
                        >
{
public:

    /** 默认构造函数
    */
    RCPPMDDecoder() ;
    
    /** 默认析构函数
    */
    ~RCPPMDDecoder() ;
    
public:
    
    /** 释放流
    */
    void ReleaseStreams() ;
    
    /** Flush 数据
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
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
    
    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;
    
    /** 设置输入流
    @param [in] inStream 输入流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ReleaseInStream() ;
    
    /** 设置输出流大小
    @param [in] outSize 输出流大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOutStreamSize(const uint64_t* outSize) ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
private:
    
    /** 解码
    @param [in] size 大小
    @param [in] memStream 内存输入流
    @return 成功返回RC_S_OK,否则返回错误号 
    */
    HResult CodeSpec(uint32_t size, byte_t* memStream) ;
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeReal(ISequentialInStream* inStream,
                             ISequentialOutStream* outStream, 
                             const uint64_t* inSize, 
                             const uint64_t* outSize,
                             ICompressProgressInfo* progress) ; 
    
private:
    
    /** 解码器
    */
    RCPPMDRangeDecoder m_rangeDecoder ;
    
    /** 输出流
    */
    RCOutBuffer m_outStream ;
    
    /** 信息
    */
    RCPPMDDecodeInfo m_info ;
    
    /** 顺序
    */
    byte_t m_order ;
    
    /** 使用内存大小
    */
    uint32_t m_usedMemorySize ;
    
    /** 保留长度
    */
    int32_t m_remainLen ;
    
    /** 输出大小
    */
    uint64_t m_outSize ;
    
    /** 输出大小是否定义
    */
    bool m_outSizeDefined ;
    
    /** 已经处理大小
    */
    uint64_t m_processedSize ;
};

END_NAMESPACE_RCZIP

#endif //__RCPPMDDecoder_h_
