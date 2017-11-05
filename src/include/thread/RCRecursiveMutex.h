/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRecursiveMutex_h_
#define __RCRecursiveMutex_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

class RCRecursiveMutex:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRecursiveMutex() ;
    
    /** 默认析构函数
    */
    ~RCRecursiveMutex() ;
    
public:

    /** 获取锁，如果取不到锁则一直等待
    */    
    void Lock() ;
        
    /** 尝试获取锁
    @return 如果取到锁，返回true,如果取不到锁则返回false
    */
    bool TryLock() ;
        
    /** 释放锁
    */
    void UnLock() ;

private:
    
    /** 实现数据
    */
    struct TImpl ;
    TImpl* m_impl ;
};

/** 自动加锁，解锁
*/
class RCRecursiveMutexLock:
    private RCNonCopyable
{
public:
    
    /** 默认构造函数，获取锁
    *@param [in] mutex 需要获取的锁
    */
    explicit RCRecursiveMutexLock(RCRecursiveMutex* mutex):
        m_mutex(mutex)
    {
        if(m_mutex)
        {
            m_mutex->Lock() ;
        }
    }
    
    /** 默认析构函数，释放锁
    */
    ~RCRecursiveMutexLock()
    {
        if(m_mutex)
        {
            m_mutex->UnLock() ;
        }
    }
    
private:
    /** 需要保护的锁
    */
    RCRecursiveMutex* m_mutex ;
};

END_NAMESPACE_RCZIP

#endif //__RCRecursiveMutex_h_
