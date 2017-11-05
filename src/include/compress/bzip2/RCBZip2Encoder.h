/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2Encoder_h_
#define __RCBZip2Encoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"
#include "compress/bzip2/RCBZip2ThreadInfo.h"
#include "common/RCInBuffer.h"
#include "common/RCOutBuffer.h"
#include "compress/bzip2/RCBZip2CombinedCRC.h"
#include "common/RCBitmEncoder.h"
#include "thread/RCMutex.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 编码器
*/
class RCBZip2Encoder:
#ifdef COMPRESS_BZIP2_MT
    public IUnknownImpl3<ICompressCoder,
                         ICompressSetCoderProperties,
                         ICompressSetCoderMt
                        >
#else
    public IUnknownImpl2<ICompressCoder,
                         ICompressSetCoderProperties
                        >
#endif
{
public:

    /** 默认构造函数
    */
    RCBZip2Encoder() ;
    
    /** 默认析构函数
    */
    ~RCBZip2Encoder() ;
    
public:
    
    /** 写数据
    @param [in] data 数据
    @param [in] sizeInBits 位数
    @param [in] lastByte 最后字节
    */
    void WriteBytes(const byte_t* data, uint32_t sizeInBits, byte_t lastByte);
    
    /** 读取Rle块
    @param [in] buffer 数据缓冲
    @return 返回读取字节数
    */
    uint32_t ReadRleBlock(byte_t* buffer) ;
    
    /** 写位数据
    @param [in] value 数值
    @param [in] numBits 位数
    */
    void WriteBits(uint32_t value, uint32_t numBits) ;
    
    /** 写入字节
    @param [in] b 写入的字节
    */
    void WriteByte(byte_t b) ;
    
    /** 写位
    @param [in] v
    */
    void WriteBit(bool v) ;
    
    /** 写CRC校验值
    @param [in] v CRC校验值
    */
    void WriteCRC(uint32_t v) ;
    
    /** Flush数据
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Flush() ;
    
    /** 释放流
    */
    void ReleaseStreams() ;
    
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
        
public:
    
    /** 输入流缓存
    */
    RCInBuffer m_inStream;
    
    /** mt Pad
    */
    byte_t m_mtPad[1 << 8]; // It's pad for Multi-Threading. Must be >= Cache_Line_Size.
    
    /** 输出流缓存
    */
    RCBitmEncoder<RCOutBuffer> m_outStream;
    
    /** num Passes 值
    */
    uint32_t m_numPasses;
    
    /** CRC校验
    */
    RCBZip2CombinedCRC m_combinedCRC;

#ifdef COMPRESS_BZIP2_MT

    /** 线程信息
    */
    RCBZip2ThreadInfo* m_threadsInfo;
    
    /** 同步事件
    */
    RCManualResetEvent m_canProcessEvent;
    
    /** 同步临界区
    */
    RCMutex m_critSection;
    
    /** 线程数
    */
    uint32_t m_numThreads;
    
    /** 多线程模式
    */
    bool m_mtMode;
    
    /** 下一块编号
    */
    uint32_t m_nextBlockIndex;
    
    /** 关闭线程
    */
    bool m_closeThreads;
    
    /** 流是否完成
    */
    bool m_streamWasFinished;
    
    /** 同步事件
    */
    RCManualResetEvent m_canStartWaitingEvent;
    
    /** 操作结果
    */
    HResult m_result;
    
    /** 压缩进度
    */
    ICompressProgressInfo* m_progress;
    
    /** 创建
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Create();
    
    /** 释放
    */
    void Free();
    
    /** 设置线程数
    @param [in] numThreads 线程数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult SetNumberOfThreads(uint32_t numThreads) ;
    
#else

    /** 线程信息
    */
    RCBZip2ThreadInfo m_threadsInfo ;
    
#endif

private:

    /** 编码实现
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] inSize 输入大小
    @param [in] outSize 输出大小
    @param [in] progress 进度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult CodeReal(ISequentialInStream* inStream,
                     ISequentialOutStream* outStream, 
                     const uint64_t* inSize, 
                     const uint64_t* outSize,
                     ICompressProgressInfo* progress) ;
    
private:
    
    /** 块大小
    */
    uint32_t m_blockSizeMult;
    
    /** 优化表数量
    */
    bool m_optimizeNumTables;
    
    /** Passes数
    */
    uint32_t m_numPassesPrev;
    
    /** 线程数
    */
    uint32_t m_numThreadsPrev;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2Encoder_h_
