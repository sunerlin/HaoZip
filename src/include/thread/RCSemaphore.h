/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSemaphore_h_
#define __RCSemaphore_h_ 1

#include "thread/RCHandle.h"
#include "algorithm/Threads.h"

BEGIN_NAMESPACE_RCZIP

class RCSemaphore
{
public:

    /** 默认构造函数
    */
    RCSemaphore() ;
   
    /** 默认析构函数
    */
    ~RCSemaphore() ;

public:

    /** 关闭Semaphore
    @return 成功返回0, 否则返回错误号
    */
    WRes Close() ;

    /** 返回句柄值
    */
    operator HANDLE() const ;

    /** 创建Semaphore
    @param [in] initiallyCount 初始引用计数
    @param [in] maxCount 最大引用计数值
    @return 成功返回0, 否则返回错误号
    */
    WRes Create(uint32_t initiallyCount, uint32_t maxCount) ;

    /** 释放Semaphore, 引用计数减1
    @return 成功返回0, 否则返回错误号
    */
    WRes Release() ;

    /** 释放Semaphore, 引用计数减 releaseCount
    @param [in] releaseCount 需要释放的引用计数值
    @return 成功返回0, 否则返回错误号
    */
    WRes Release(uint32_t releaseCount) ;
    
    /** 对Semaphore加锁
    @return 成功返回0, 否则返回错误号
    */
    WRes Lock() ;
    
private:

    /** Semaphore对象
    */
    ::CSemaphore m_object;
};

END_NAMESPACE_RCZIP

#endif //__RCSemaphore_h_
