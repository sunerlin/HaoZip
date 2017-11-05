/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLockedInStream_h_
#define __RCLockedInStream_h_ 1

#include "base/RCNonCopyable.h"
#include "interface/IStream.h"
#include "thread/RCMutex.h"

BEGIN_NAMESPACE_RCZIP

class RCLockedInStream:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCLockedInStream() ;
    
    /** 默认析构函数
    */
    ~RCLockedInStream() ;
    
public:
    
    /** 初始化
    @param [in] stream 输入流接口
    */
    void Init(IInStream* stream) ;
    
    /** 读取数据
    @param [in] startPos 起始位置
    @param [out] data 数据缓冲区
    @param [in] size 期望读取的数据长度
    @param [out] processedSize 返回实际读取的数据长度
    @return 成功返回RC_S_OK, 失败则返回错误码
    */
    HResult Read(uint64_t startPos, 
                 void* data, 
                 uint32_t size, 
                 uint32_t* processedSize);
    
private:
    
    /** 数据输入流接口
    */
    IInStreamPtr m_stream ;
    
    /** 多线程同步锁
    */
    RCMutex m_lock ;
};

END_NAMESPACE_RCZIP

#endif //__RCLockedInStream_h_
