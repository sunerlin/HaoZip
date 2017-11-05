/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStreamBinder_h_
#define __RCStreamBinder_h_ 1

#include "base/RCNonCopyable.h"
#include "thread/RCSynchronization.h"

BEGIN_NAMESPACE_RCZIP

class ISequentialInStream ;
class ISequentialOutStream ;

class RCStreamBinder:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCStreamBinder() ;
    
    /** 默认析构函数
    */
    ~RCStreamBinder() ;

public:
    
    /** 创建同步的Event
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult CreateEvents() ;
    
    /** 创建流接口
    @param [out] inStream 输入流接口
    @param [out] outStream 输出流接口
    */
    void CreateStreams(ISequentialInStream** inStream,
                       ISequentialOutStream** outStream) ;


    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 关闭数据读
    */
    void CloseRead() ;
    
    /** 写数据  
    @param [in] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK, 否则返回错误码
    */
    HResult Write(const void* data, uint32_t size, uint32_t* processedSize);
    
    /** 关闭数据写
    */
    void CloseWrite() ;
    
    /** 重新初始化
    */
    void ReInit() ;
    
    /** 获取已处理大小
    @return 返回已处理数据大小
    */
    uint64_t GetProcessedSize(void) const ;
    
private:
    
    /** 所有数据都写入同步事件
    */
    RCManualResetEvent m_allBytesAreWritenEvent;
    
    /** 仍有数据可读取同步事件
    */
    RCManualResetEvent m_thereAreBytesToReadEvent;
    
    /** 数据输入流已关闭同步事件
    */
    RCManualResetEvent m_readStreamIsClosedEvent;

    /** 缓冲区大小
    */
    uint32_t m_bufferSize ;
    
    /** 数据缓冲区
    */
    const void* m_buffer ;
    
    /** 已处理数据大小
    */
    uint64_t m_processedSize ;
};

END_NAMESPACE_RCZIP

#endif //__RCStreamBinder_h_
