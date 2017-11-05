/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRangeDecoder_h_
#define __RCRangeDecoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Range 解码器
*/
class RCRangeDecoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRangeDecoder() ;
    
    /** 默认析构函数
    */
    ~RCRangeDecoder() ;

public:

    /** 返回输入流缓存
    @return 返回输入缓存
    */
    RCInBuffer& GetStream() ;

    /** 返回Range
    @return 返回Range
    */
    uint32_t GetRange(void) const ;

    /** 返回Code
    @return 返回Code
    */
    uint32_t GetCode(void) const ;

    /** 设置Range
    @param [in] range 值
    */
    void SetRange(uint32_t range) ;

    /** 设置code
    @param [in] code 值
    */
    void SetCode(uint32_t code) ;

    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;

    /** 正常化
    */
    void Normalize() ;

    /** 设置输入流
    @param [in] stream 输入流
    */
    void SetStream(ISequentialInStream *stream) ;

    /** 初始化
    */
    void Init() ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 返回阀值
    @param [in] total 总数
    @return 返回阀值
    */
    uint32_t GetThreshold(uint32_t total) ;

    /** 解码
    @param [in] start 开始
    @param [in] size 大小
    */
    void Decode(uint32_t start, uint32_t size) ;

    /** 直接位解码
    @param [in] numTotalBits 位数
    @return 返回解码结果
    */
    uint32_t DecodeDirectBits(int numTotalBits) ;

    /** 位解码
    @param [in] size0 大小
    @param [in] numTotalBits 位总数
    @return 返回解码结果
    */
    uint32_t DecodeBit(uint32_t size0, uint32_t numTotalBits) ;

    /** 返回已处理大小
    */
    uint64_t GetProcessedSize() ;
    
private:

    /** 输入缓存
    */
    RCInBuffer m_stream ;

    /** range
    */
    uint32_t m_range ;

    /** code
    */
    uint32_t m_code ;
};

END_NAMESPACE_RCZIP

#endif //__RCRangeDecoder_h_
