/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBitlEncoder_h_
#define __RCBitlEncoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCOutBuffer.h"

BEGIN_NAMESPACE_RCZIP

class ISequentialOutStream ;

class RCBitlEncoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCBitlEncoder() ;
    
    /** 默认析构函数
    */
    ~RCBitlEncoder() ;
    
public:
    
    /** 创建缓冲区
    @param [in] bufferSize 缓冲区大小
    @return 创建成功返回true, 失败则返回false
    */
    bool Create(uint32_t bufferSize) ;
    
    /** 设置输出流接口
    @param [in] outStream 输出流接口指针
    */
    void SetStream(ISequentialOutStream* outStream) ;
    
    /** 释放输出流
    */
    void ReleaseStream() ;
    
    /** 初始化
    */
    void Init() ;
    
    /** 回写数据
    @return 如果成功返回RC_S_OK, 否则返回错误码
    */
    HResult Flush() ;
    
    /** 回写字节
    */
    void FlushByte() ;
    
    /** 写入位数
    @param [in] value 写入的数值
    @param [in] numBits 写入的位数
    */
    void WriteBits(uint32_t value, int32_t numBits) ;
    
    /** 获取当前位的位置
    @return 当前位的位置
    */
    uint32_t GetBitPosition() const ;
    
    /** 获取已处理大小
    @return 返回已处理大小
    */
    uint64_t GetProcessedSize() const ;
    
    /** 写入字节
    @param [in] b 字节值
    */
    void WriteByte(byte_t b) ;
    
private:
    
    /** 输出流接口
    */
    RCOutBuffer m_stream ;
    
    /** 位偏移位置
    */
    int32_t m_bitPos ;
    
    /** 当前字节值
    */
    byte_t m_curByte ;
};

END_NAMESPACE_RCZIP

#endif //__RCBitlEncoder_h_
