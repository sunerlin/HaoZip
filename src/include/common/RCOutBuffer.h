/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCOutBuffer_h_
#define __RCOutBuffer_h_ 1

#include "interface/IStream.h"
#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

class ISequentialOutStream ;

/** 输出缓冲区
*/
class RCOutBuffer:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCOutBuffer() ;
    
    /** 默认析构函数
    */
    ~RCOutBuffer() ;

public:
    
    /** 创建缓冲区
    @param [in] bufferSize 缓冲区大小
    @return 成功返回true,失败返回false
    */
    bool Create(uint32_t bufferSize);
    
    /** 初始化
    */
    void Init();
    
    /** 释放缓冲区
    */
    void Free();

    /** 设置 Mem Stream
    @param [in] buffer 缓冲区地址
    */
    void SetMemStream(byte_t* buffer) ;
    
    /** 设置输出流
    @param [in] stream 输出流指针
    */
    void SetStream(ISequentialOutStream* stream) ;
  
    /** 缓冲区写入输出流
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult Flush() ;
    
    /** 缓冲区写入输出流，如果失败则抛出异常
    */
    void FlushWithCheck();
    
    /** 释放输出流
    */
    void ReleaseStream() ;

    /** 写入字节
    @param [in] value 需要写入的字节值
    @throws 如果写数据失败抛异常
    */
    void WriteByte(byte_t value) ;
    
    /** 写入多字节
    @param [in] data 字节起始地址
    @param [in] size 字节长度
    @throws 如果写数据失败抛异常
    */
    void WriteBytes(const void* data, size_t size) ;
    
    /** 获取已处理数据大小
    @return 返回已处理的长度
    */
    uint64_t GetProcessedSize() const;
    
protected:    
    
    /** 缓冲区写入输出流
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    HResult FlushPart() ;
    
protected:
    
    /** 数据缓冲区
    */
    byte_t* m_buffer ;
    
    /** 数据偏移
    */
    uint32_t m_pos ;
    
    /** 限制数据偏移
    */
    uint32_t m_limitPos ;
    
    /** 流的位置偏移
    */
    uint32_t m_streamPos ;
    
    /** 缓冲区大小
    */
    uint32_t m_bufferSize ;
    
    /** 输出流接口
    */
    ISequentialOutStreamPtr m_spStream ;
    
    /** 已处理大小
    */
    uint64_t m_processedSize ;
    
    /** 内存缓冲区地址
    */
    byte_t* m_buffer2 ;
    
    /** 是否已经达到缓冲区大小
    */
    bool m_overDict ;
};

END_NAMESPACE_RCZIP

#endif //__RCOutBuffer_h_
