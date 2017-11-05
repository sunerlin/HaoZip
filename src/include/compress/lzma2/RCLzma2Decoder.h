/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzma2Decoder_h_
#define __RCLzma2Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "algorithm/Lzma2Dec.h"

BEGIN_NAMESPACE_RCZIP

/** Lzma2 解码器
*/
class RCLzma2Decoder:
    public IUnknownImpl6<ICompressCoder,
                         ICompressSetDecoderProperties2,
                         ICompressGetInStreamProcessedSize,
                         ICompressSetInStream,
                         ICompressSetOutStreamSize,
                         ISequentialInStream>
{
public:
    
    /** 默认构造函数
    */
    RCLzma2Decoder();

    /** 默认析构函数
    */
    virtual ~RCLzma2Decoder();

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

    /** 设置解压属性
    @param [in] data 属性数据
    @param [in] size 数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetDecoderProperties2(const byte_t* data, uint32_t size) ;

    /** 获取输入流读取的数据长度
    @param [out] size 返回数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetInStreamProcessedSize(uint64_t& size) ;

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
    
    /** 申请内存
    @param [in] p 指针
    @param [in] size 大小
    @return 返回申请内存指针
    */
    static void* SzAlloc(void* p, size_t size) ;
    
    /** 释放内存
    @param [in] p 指针
    @param [in] address 内存地址
    */
    static void SzFree(void* p, void* address) ;
    
    /** Res 转 HResult
    @param [in] res Res值
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult SResToHRESULT(result_t res) ;

private:
    
    /** 内存申请
    */
    static ISzAlloc s_alloc ;

private:
    
    /** 输入流
    */
    ISequentialInStreamPtr m_inStream;
    
    /** 输入缓存
    */
    byte_t* m_inBuf;
    
    /** 输入位置
    */
    uint32_t m_inPos;
    
    /** 输入大小
    */
    uint32_t m_inSize;
    
    /** 状态
    */
    CLzma2Dec m_state;
    
    /** 是否定义输出大小
    */
    bool m_outSizeDefined;
    
    /** 输出大小
    */
    uint64_t m_outSize;
    
    /** 已处理输入大小
    */
    uint64_t m_inSizeProcessed;
    
    /** 已处理输出大小
    */
    uint64_t m_outSizeProcessed;

};

END_NAMESPACE_RCZIP

#endif //__RCLzma2Decoder_h_
