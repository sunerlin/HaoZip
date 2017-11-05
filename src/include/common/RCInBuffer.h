/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCInBuffer_h_
#define __RCInBuffer_h_ 1

#include "interface/IStream.h"
#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

/** 输入缓冲区
*/
class RCInBuffer:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCInBuffer() ;
    
    /** 默认析构函数
    */
    ~RCInBuffer() ;
    
public:

    /** 创建缓冲区
    @param [in] bufferSize 缓冲区大小
    @return 成功返回true,失败返回false
    */
    bool Create(uint32_t bufferSize) ;
    
    /** 初始化
    */
    void Init();
    
    /** 释放缓冲区
    */
    void Free() ;
  
    /** 设置输入流
    @param [in] stream 输入流的指针
    */
    void SetStream(ISequentialInStream* stream) ;

    /** 释放输入流
    */
    void ReleaseStream() ;

    /** 读取字节
    @param [out] value 读取的字节值
    @return 成功返回true, 失败则返回false
    @throws 如果读取数据失败抛异常
    */
    bool ReadByte(byte_t& value) ;
    
    /** 读取字节
    @return 返回读取的字节，如果失败返回0xFF
    @throws 如果读取数据失败抛异常
    */
    byte_t ReadByte() ;
    
    /** 读取多字节
    @param [out] buf 存放读取结果的缓冲区地址
    @param [in]  size 缓冲区的长度
    @return 返回实际读取的长度
    @throws 如果读取数据失败抛异常
    */
    uint32_t ReadBytes(byte_t* buf, uint32_t size) ;
    
    /** 获取当前读取的字节数
    @return 返回已经处理的大小
    */
    uint64_t GetProcessedSize() const ;
    
    /** 是否结束
    @return 如果处理结束，返回true，否则返回false
    */
    bool WasFinished() const ;
  
private:
    
    /** 读取数据块
    @return 成功返回true, 失败返回false
    */
    bool ReadBlock() ;
    
    /** 读取数据块
    @return 成功返回读到的数据，失败则返回0xFF
    */
    byte_t ReadBlock2() ;
       
private:
    
    /** 数据缓冲区
    */
    byte_t* m_buffer ;
    
    /** 有效数据缓冲区
    */
    byte_t* m_bufferLimit ;
    
    /** 缓冲区基地址
    */
    byte_t* m_bufferBase ;
    
    /** 输入流
    */
    ISequentialInStreamPtr m_spStream ;
    
    /** 已处理大小
    */
    uint64_t m_processedSize ;
    
    /** 缓冲区大小
    */
    uint32_t m_bufferSize ;
    
    /** 是否已完成
    */
    bool m_wasFinished ;
};

END_NAMESPACE_RCZIP

#endif //__RCInBuffer_h_
