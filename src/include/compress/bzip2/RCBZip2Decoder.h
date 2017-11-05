/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2Decoder_h_
#define __RCBZip2Decoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "common/RCInBuffer.h"
#include "common/RCOutBuffer.h"
#include "common/RCBitmDecoder.h"
#include "compress/huffman/RCHuffmanDecoder.h"
#include "compress/bzip2/RCBZip2Defs.h"
#include "compress/bzip2/RCBZip2CombinedCRC.h"
#include "compress/bzip2/RCBZip2DecoderState.h"
#include "thread/RCMutex.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 解码器
*/
class RCBZip2Decoder:
#ifdef COMPRESS_BZIP2_MT
    public IUnknownImpl5<ICompressCoder,
                         ICompressGetInStreamProcessedSize,
                         ICompressSetCoderMt,
                         IBZip2ResumeDecoder,
                         ICompressSetCoderProperties
                        >
#else
    public IUnknownImpl4<ICompressCoder,
                         ICompressGetInStreamProcessedSize,
                         IBZip2ResumeDecoder,
                         ICompressSetCoderProperties
                        >
#endif
{
public:
    
    typedef RCHuffmanDecoder<RCBZip2Defs::s_kMaxHuffmanLen, RCBZip2Defs::s_kMaxAlphaSize> RCBZip2HuffmanDecoder ;
        
public:

    /** 默认构造函数
    */
    RCBZip2Decoder() ;
    
    /** 默认析构函数
    */
    ~RCBZip2Decoder() ;
    
public:
    
    /** 设置进度
    @param [in] packSize 数据大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult SetRatioProgress(uint64_t packSize);
    
    /** 读取签名
    @param [out] wasFinished 是否完成
    @param [out] crc CRC校验值
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult ReadSignatures(bool& wasFinished, uint32_t& crc) ;
    
    /** Flush数据
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
    /** 释放文件流
    @param [in] releaseInStream 是否释放输入流
    */
    void ReleaseStreams(bool releaseInStream) ;

public:
    
    /** 设置输入流
    @param [in] inStream 数据输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetInStream(ISequentialInStream* inStream) ;
    
    /** 释放输入流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ReleaseInStream() ;
    
    /** 编码
    @param [in] outStream 数据输出流
    @param [out] isBZ 是否为BZip2编码
    @param [in]  progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CodeResume(ISequentialOutStream* outStream, bool& isBZ, ICompressProgressInfo* progress) ;
    
    /** 获取输入流数据已处理长度
    @return 返回输入流已处理字节数
    */
    virtual uint64_t GetInputProcessedSize() ;

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

    /** 获取输入流读取的数据长度
    @param [out] size 返回数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetInStreamProcessedSize(uint64_t& size) ;
        
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ; 

public:
    
    /** 输出缓冲
    */
    RCOutBuffer m_outStream;
    
    /** mt Pad
    */
    byte_t m_mtPad[1 << 8] ; // It's pad for Multi-Threading. Must be >= Cache_Line_Size.
    
    /** 输入缓冲
    */
    RCBitmDecoder<RCInBuffer> m_inStream;
    
    /** 选择
    */
    byte_t m_selectors[RCBZip2Defs::s_kNumSelectorsMax];
        
    /** Huffman编码
    */
    RCBZip2HuffmanDecoder m_huffmanDecoders[RCBZip2Defs::s_kNumTablesMax];
        
    /** inStart
    */
    uint64_t m_inStart ;
    
    /** CRC校验
    */
    RCBZip2CombinedCRC m_combinedCRC ;
    
#ifdef COMPRESS_BZIP2_MT
    
    /** 进度接口指针
    */
    ICompressProgressInfo* m_progress ;
    
    /** 状态
    */
    RCBZip2DecoderState* m_states ;
    
    /** 线程数
    */
    uint32_t m_numThreadsPrev ;
    
    /** 同步事件
    */
    RCManualResetEvent m_canProcessEvent;
    
    /** 同步临界区
    */
    RCMutex m_critSection;
    
    /** 线程数
    */
    uint32_t m_numThreads;
    
    /** 是否多线程模式
    */
    bool m_isMtMode ;
    
    /** 下一块编号
    */
    uint32_t m_nextBlockIndex ;
    
    /** 是否关闭线程
    */
    bool m_isCloseThreads;
    
    /** 流是否结束1
    */
    bool m_streamWasFinished1;
    
    /** 流是否结束2
    */
    bool m_streamWasFinished2;
    
    /** 同步事件
    */
    RCManualResetEvent m_canStartWaitingEvent;
    
    /** 处理结果1
    */
    HResult m_result1;
    
    /** 处理结果2
    */
    HResult m_result2;
    
    /** 最大块大小
    */
    uint32_t m_blockSizeMax;

    /** 创建解码器
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Create();
    
    /** 是否
    */
    void Free();

    /** 设置线程数
    @param [in] numThreads 线程数量
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult SetNumberOfThreads(uint32_t numThreads) ;
    
#else

    /** 状态
    */
    RCBZip2DecoderState m_states[1];
    
#endif

private:
    
    /** 是否输入流初始化
    */
    bool m_needInStreamInit ;

    /** 是否IszBzip2算法
    */
    bool m_isIszBZip2 ;

private:
    
    /** 读取数据位
    @param [in] numBits 数据位数
    @return 返回读取的数据位数
    */
    uint32_t ReadBits(int32_t numBits) ;
    
    /** 读取字节
    @return 返回读取的字节
    */
    byte_t ReadByte() ;
    
    /** 读取位
    @return 成功返回true,否则返回false
    */
    bool ReadBit() ;
    
    /** 取得CRC校验
    @return 返回CRC校验
    */
    uint32_t ReadCRC() ;
    
    /** 文件解码
    @param [out] isBZ 是否是BZ
    @param [in] progress 进度接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult DecodeFile(bool& isBZ, ICompressProgressInfo* progress) ;
    
    /** 解码操作
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [out] isBZ 是否BZip2
    @param [in] progress 解码进度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeReal(ISequentialInStream* inStream,
                     ISequentialOutStream* outStream, 
                     bool& isBZ,
                     ICompressProgressInfo* progress) ;

public:
    
    /** 读取数据位
    @param [in] inStream 输入数据流
    @param [in] num 数据位数
    @return 返回读取位数
    */
    static uint32_t ReadBits(RCBitmDecoder<RCInBuffer>* inStream, int32_t num) ;
    
    /** 读取位
    @param [in] inStream 输入数据流
    @return 返回读取位数
    */
    static uint32_t ReadBit(RCBitmDecoder<RCInBuffer>* inStream) ;
    
    /** 读取数据块
    @param [in] inStream 输入流
    @param [out] charCounters 字符数
    @param [out] blockSizeMax 块最大字节数
    @param [out] selectors
    @param [out] huffmanDecoders
    @param [out] blockSizeRes
    @param [out] origPtrRes
    @param [out] randRes
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult ReadBlock(RCBitmDecoder<RCInBuffer>* inStream,
                             uint32_t* charCounters, 
                             uint32_t blockSizeMax, 
                             byte_t* selectors, 
                             RCBZip2HuffmanDecoder* huffmanDecoders,
                             uint32_t* blockSizeRes, 
                             uint32_t* origPtrRes, 
                             bool* randRes) ;
                             
    /** 块解码1
    @param [in] charCounters 字节数
    @param [in] blockSize 块大小
    */
    static void DecodeBlock1(uint32_t* charCounters, uint32_t blockSize) ;
    
    /** 块解码2
    @param [in] tt 输入数据
    @param [in] blockSize 块大小
    @param [in] origPtr 
    @param [in] outStream 输出流
    @return 返回解码输出字节数
    */
    static uint32_t DecodeBlock2(const uint32_t* tt, 
                                 uint32_t blockSize, 
                                 uint32_t origPtr, 
                                 RCOutBuffer& outStream) ;
    
    /** 块解码2
    @param [in] tt  输入数据
    @param [in] blockSize 块大小
    @param [in] origPtr 
    @param [in] outStream 输出数据流
    @return 返回解码输出字节数
    */
    static uint32_t DecodeBlock2Rand(const uint32_t* tt, 
                                     uint32_t blockSize, 
                                     uint32_t origPtr, 
                                     RCOutBuffer& outStream) ;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2Decoder_h_
