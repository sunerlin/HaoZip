/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCQuantumStreamBitDecoder_h_
#define __RCQuantumStreamBitDecoder_h_ 1

#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Quantum Bit 解码器
*/
class RCQuantumStreamBitDecoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCQuantumStreamBitDecoder() ;
    
    /** 默认析构函数
    */
    ~RCQuantumStreamBitDecoder() ;
    
public:

    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;

    /** 设置输入流
    @param [in] inStream 输入流
    */
    void SetStream(ISequentialInStream *inStream) ;

    /** 释放流
    */
    void ReleaseStream() ;

    /** 解码结束
    */
    void Finish() ;

    /** 初始化
    */
    void Init() ;

    /** 取得已经处理大小
    @return 返回已处理大小
    */
    uint64_t GetProcessedSize() const ;

    /** 是否结束
    @return 结束返回true,否则返回false
    */
    bool WasFinished() const ;

    /** 读取位
    @return 返回读取结果
    */
    uint32_t ReadBit() ;

    /** 读取位
    @param [in] numBits 读取位数
    @return 返回读取结果
    */
    uint32_t ReadBits(int32_t numBits) ; // numBits > 0
    
private:

    /** 值
    */
    uint32_t m_value;

    /** 输入缓存
    */
    RCInBuffer m_stream;
};

END_NAMESPACE_RCZIP

#endif //__RCQuantumStreamBitDecoder_h_
