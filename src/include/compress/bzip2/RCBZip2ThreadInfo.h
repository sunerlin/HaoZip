/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBZip2ThreadInfo_h_
#define __RCBZip2ThreadInfo_h_ 1

#include "base/RCNonCopyable.h"
#include "compress/bzip2/RCBZip2Defs.h"
#include "thread/RCSynchronization.h"
#include "thread/RCThread.h"

BEGIN_NAMESPACE_RCZIP

/** BZip2 Msbf 编码
*/
class RCBZip2MsbfEncoderTemp ;

/** BZip2 编码器
*/
class RCBZip2Encoder;

/** BZip2线程信息
*/
class RCBZip2ThreadInfo:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCBZip2ThreadInfo() ;
    
    /** 默认析构函数
    */
    ~RCBZip2ThreadInfo() ;
    
public:
    
    /** 申请内存
    @return 成功返回true,否则返回false
    */
    bool Alloc() ;
    
    /** 释放内存
    */
    void Free() ;
    
    /** 块编码3
    @param [in] blockSize 块大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult EncodeBlock3(uint32_t blockSize) ;
    
public:
    
    /** 块数据指针
    */
    byte_t* m_block ;
    
    /** 优化表个数
    */
    bool m_optimizeNumTables ;
    
    /** 编码器指针
    */
    RCBZip2Encoder* m_encoder ;
    
#ifdef COMPRESS_BZIP2_MT

    /** 线程指针
    */
    RCThreadPtr m_spThread ;
    
    /** 同步事件
    */
    RCAutoResetEvent m_streamWasFinishedEvent ;
    
    /** 同步事件
    */
    RCAutoResetEvent m_waitingWasStartedEvent ;

    /** 同步事件
    */
    RCAutoResetEvent m_canWriteEvent ;
    
    /** 压缩后大小
    */
    uint64_t m_packSize ;
    
    /** mt Pad
    */
    byte_t m_mtPad[1 << 8] ; // It's pad for Multi-Threading. Must be >= Cache_Line_Size.
    
    /** 创建
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Create() ;
    
    /** 释放流
    @param [in] needLeave 是否离开
    */
    void FinishStream(bool needLeave) ;
    
    /** 线程函数
    */
    void ThreadFunc() ;
    
#endif

private:
    
    /** 写数据位
    @param [in] value 数值
    @param [in] numBits 位数
    */
    void WriteBits2(uint32_t value, uint32_t numBits) ;
    
    /** 写字节2
    @param [in] b 写入字节
    */
    void WriteByte2(byte_t b);
    
    /** 写位函数2
    @param [in] v
    */
    void WriteBit2(bool v) ;
    
    /** 写CRC校验
    @param [in] v 校验值
    */
    void WriteCRC2(uint32_t v) ;
    
    /** 块编码
    @param [in] block 块数据
    @param [in] blockSize 块大小
    */
    void EncodeBlock(const byte_t* block, uint32_t blockSize) ;
    
    /** 带头块编码
    @param [in] block 块数据
    @param [in] blockSize 块大小
    @return 返回编码后字节数
    */
    uint32_t EncodeBlockWithHeaders(const byte_t* block, uint32_t blockSize) ;
    
    /** 块编码2
    @param [in] block 块数据
    @param [in] blockSize 块大小
    @param [in] numPasses 忽略大小
    */
    void EncodeBlock2(const byte_t* block, uint32_t blockSize, uint32_t numPasses) ;
    
private:
    
    /** mt 队列
    */
    byte_t* m_mtfArray;
    
    /** 临时队列
    */
    byte_t* m_tempArray;
    
    /** 块排序后编号
    */
    uint32_t* m_blockSorterIndex;
    
    /** 当前输出流
    */
    RCBZip2MsbfEncoderTemp* m_outStreamCurrent;
    
    /** 长度
    */
    byte_t m_lens[RCBZip2Defs::s_kNumTablesMax][RCBZip2Defs::s_kMaxAlphaSize];
        
    /** 频度
    */
    uint32_t m_freqs[RCBZip2Defs::s_kNumTablesMax][RCBZip2Defs::s_kMaxAlphaSize];
        
    /** 编码
    */
    uint32_t m_codes[RCBZip2Defs::s_kNumTablesMax][RCBZip2Defs::s_kMaxAlphaSize];
        
    /** 选择子
    */
    byte_t m_selectors[RCBZip2Defs::s_kNumSelectorsMax];
        
    /** CRC校验
    */
    uint32_t m_crcs[1 << RCBZip2Defs::s_kNumPassesMax];
    
    /** CRC数量
    */
    uint32_t m_numCrcs;
    
    /** 块编号
    */
    uint32_t m_blockIndex;
};

END_NAMESPACE_RCZIP

#endif //__RCBZip2ThreadInfo_h_
