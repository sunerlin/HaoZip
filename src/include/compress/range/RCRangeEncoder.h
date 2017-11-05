/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRangeEncoder_h_
#define __RCRangeEncoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCOutBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Range 编码
*/
class RCRangeEncoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRangeEncoder() ;
    
    /** 默认析构函数
    */
    ~RCRangeEncoder() ;

public:
    /** Get Low
    */
    uint64_t GetLow(void) const ;
    
    /** Get Range
    */
    uint32_t GetRange(void) const ;
    
    /** Get Out Stream
    */
    RCOutBuffer& GetStream(void) ;
    
    /** Set Low
    @param [in] low 值
    */
    void SetLow(uint64_t low) ;
    
    /** Set Range
    @param [in] range 值
    */
    void SetRange(uint32_t range) ;

    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;

    /** 设置输出流
    @param [in] stream 输出流
    */
    void SetStream(ISequentialOutStream *stream) ;

    /** 初始化
    */
    void Init() ;
 
    /** Flush数据
    */
    void FlushData() ;

    /** Flush流
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult FlushStream() ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 编码
    @param [in] start 开始
    @param [in] size 大小
    @param [in] total 总数
    */
    void Encode(uint32_t start, uint32_t size, uint32_t total) ;

    /** ShiftLow
    */
    void ShiftLow() ;

    /** 位编码
    @param [in] value 数值
    @param [in] numBits 位数
    */
    void EncodeDirectBits(uint32_t value, int32_t numBits) ;

    /** 位编码
    @param [in] size0 大小
    @param [in] numTotalBits 位数
    @param [in] symbol 符号
    */
    void EncodeBit(uint32_t size0, uint32_t numTotalBits, uint32_t symbol) ;

    /** 取得已处理大小
    @return 返回已处理大小
    */
    uint64_t GetProcessedSize() ;
    
private:

    /** cache 大小
    */
    uint32_t m_cacheSize ;

    /** cache
    */
    byte_t m_cache ;

    /** low
    */
    uint64_t m_low ;

    /** range
    */
    uint32_t m_range ;

    /** 输出流缓存
    */
    RCOutBuffer m_stream ;
};

END_NAMESPACE_RCZIP

#endif //__RCRangeEncoder_h_
