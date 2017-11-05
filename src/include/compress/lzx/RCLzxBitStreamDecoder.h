/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzxBitStreamDecoder_h_
#define __RCLzxBitStreamDecoder_h_ 1

#include "base/RCNonCopyable.h"
#include "common/RCInBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Lzx 解码器
*/
class RCLzxBitStreamDecoder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCLzxBitStreamDecoder() ;
    
    /** 默认析构函数
    */
    ~RCLzxBitStreamDecoder() ;
    
public:
    
    /** 创建
    @param [in] bufferSize 缓存大小
    @return 成功返回true,否则返回false
    */
    bool Create(uint32_t bufferSize) ;
    
    /** 设置输入流
    @param [in] s 输入流
    */
    void SetStream(ISequentialInStream *s) ;
    
    /** 释放流
    */
    void ReleaseStream() ;
    
    /** 初始化
    */
    void Init() ;
    
    /** 返回已处理大小
    @return 返回已经处理大小
    */
    uint64_t GetProcessedSize() const ;
    
    /** 返回Bit位置
    @return 返回Bit位置
    */
    int32_t GetBitPosition() const ;
    
    /** 规范化
    */
    void Normalize() ;
    
    /** 返回制定位数的数值
    @param [in] numBits 位数
    @return 返回数值
    */
    uint32_t GetValue(int32_t numBits) const ;
    
    /** 移动位置
    @param [in] numBits 移动位数
    */
    void MovePos(uint32_t numBits) ;
    
    /** 读取位
    @param [in] numBits 位数
    @return 返回读取字节数
    */
    uint32_t ReadBits(int32_t numBits) ;
    
    /** 读取位
    @param [in] numBits 位数
    @return 返回字节数
    */
    uint32_t ReadBitsBig(int32_t numBits) ;
    
    /** 读取无符号整数
    @param [out] v 返回值
    @return 成功返回true,否则返回false
    */
    bool ReadUInt32(uint32_t &v) ;
    
    /** 直接读取字节
    @return 返回读取的字节
    */
    byte_t DirectReadByte() ;
    
private:
    
    /** 输入缓存
    */
    RCInBuffer m_stream ;
    
    /** 数值
    */
    uint32_t m_value ;
    
    /** 位偏移量
    */
    int32_t m_bitPos ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzxBitStreamDecoder_h_
