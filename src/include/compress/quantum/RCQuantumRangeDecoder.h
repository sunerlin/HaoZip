/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCQuantumRangeDecoder_h_
#define __RCQuantumRangeDecoder_h_ 1

#include "compress/quantum/RCQuantumStreamBitDecoder.h"

BEGIN_NAMESPACE_RCZIP

/** Quantum Range 解码器
*/
class RCQuantumRangeDecoder
{
public:

    /** 默认构造函数
    */
    RCQuantumRangeDecoder() ;
    
    /** 默认析构函数
    */
    ~RCQuantumRangeDecoder() ;

public:

    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;

    /** 设置输入流
    @param [in] stream 输入流
    */
    void SetStream(ISequentialInStream* stream) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 初始化
    */
    void Init() ;

    /** 结束
    */
    void Finish() ;

    /** 返回已经处理大小
    @return 返回已处理大小
    */
    uint64_t GetProcessedSize() const ;

    /** 返回阀值
    @param [in] total 总数
    @return 返回阀值
    */
    uint32_t GetThreshold(uint32_t total) const ;

    /** 解码
    @param [in] start 开始
    @param [in] end 结束
    @param [in] total 总数
    */
    void Decode(uint32_t start, uint32_t end, uint32_t total) ;
      
public:

    /** 解码器
    */
    RCQuantumStreamBitDecoder m_stream ;
    
private:

    /** 低位
    */
    uint32_t m_low ;

    /** 序列
    */
    uint32_t m_range ;

    /** 编号
    */
    uint32_t m_code ;
};

END_NAMESPACE_RCZIP

#endif //__RCQuantumRangeDecoder_h_
