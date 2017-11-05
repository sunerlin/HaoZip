/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCOutMemStream_h_
#define __RCOutMemStream_h_ 1

#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "locked/RCMemLockBlocks.h"
#include "thread/RCSynchronization.h"

BEGIN_NAMESPACE_RCZIP

class RCMemBlockManagerMt ;

class RCOutMemStream:
    public IUnknownImpl<IOutStream>
{
public:

    /** 构造函数
    */
    explicit RCOutMemStream(RCMemBlockManagerMt* memManager) ;
    
    /** 默认析构函数
    */
    ~RCOutMemStream() ;
    
public:
    
    /** 初始化
    */
    void Init() ;
    
    /** 释放资源
    */
    void Free() ;
    
    /** 创建同步的Event
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult CreateEvents() ;
    
    /** 设置输出流接口
    @param [in] outStream 输出流接口指针
    */
    void SetOutStream(IOutStream* outStream) ;
    
    /** 设置输出流接口
    @param [in] outStream 输出流接口指针
    */
    void SetSeqOutStream(ISequentialOutStream* outStream) ;
    
    /** 释放输出流
    */
    void ReleaseOutStream() ;
    
    /** 数据写入输出流
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult WriteToRealStream() ;
    
    /** 数据输入内存块管理器
    @param [out] blocks 内存块管理器
    */
    void DetachData(RCMemLockBlocks& blocks) ;
    
    /** 设置为真实数据流输出模式
    */
    void SetRealStreamMode() ;
    
    /** 停止写入
    @param [in] res 操作结果
    @return 成功返回true, 否则返回false
    */
    void StopWriting(HResult res) ;
    
    /** 是否解锁事件已发出
    @return 如果已发出返回true, 否则返回false
    */
    bool WasUnlockEventSent() const ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Write(const void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;
    
    /** 调整大小
    @param [in] newSize 新的大小 
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    virtual HResult SetSize(uint64_t newSize) ;    
  
private:
    
    /** 获取当前位置
    @return 返回当前位置
    */
    uint64_t GetPos() const ;
    
private:
    
    /** 多线程内存块管理器
    */
    RCMemBlockManagerMt* m_memManager ;
    
    /** 当前内存块索引号
    */
    size_t m_curBlockIndex ;
    
    /** 当前内存块位置
    */
    size_t m_curBlockPos ;
    
    /** 真实数据流输出模式
    */
    bool m_realStreamMode ;
    
    /** 解锁事件发送标记
    */    
    bool m_unlockEventWasSent;
    
    /** 停止写入操作的结果
    */
    HResult m_stopWriteResult ;
    
    /** 内存块管理器
    */
    RCMemLockBlocks m_blocks ;
    
    /** 顺序输出流
    */
    ISequentialOutStreamPtr m_spOutSeqStream ;
    
    /** 输出流
    */
    IOutStreamPtr m_spOutStream ;
    
    /** 停止写入同步事件
    */
    RCAutoResetEvent m_stopWritingEvent;
    
    /** 写入真实流同步事件
    */
    RCAutoResetEvent m_writeToRealStreamEvent;
};

END_NAMESPACE_RCZIP

#endif //__RCOutMemStream_h_
