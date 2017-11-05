/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCThread_h_
#define __RCThread_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCSmartPtr.h"

BEGIN_NAMESPACE_RCZIP

/** 线程函数定义
@param [in] pArg 自定义线程参数指针
*/
typedef void (*PFNThreadProc)(void* pArg) ;

/** 线程管理类
*/
class RCThread:
    private RCNonCopyable
{
public:
    
    /** 线程ID类型
    */
    typedef unsigned long thread_id ;
   
public:
    
    /** 构造函数
    @param [in] pfnThreadProc 线程函数
    @param [in] pArg 线程函数参数
    */
    RCThread(PFNThreadProc pfnThreadProc, void* pArg = NULL);
 
    /** 析构函数
    */
    ~RCThread() ; 
 
public:
    
    /** 启动线程
    @return 成功返回true,否则返回false
    */
    bool Start() ;
 
    /** 等待线程结束
    */
    void Join() ;
 
    /** 等待线程的时间
    @param [in] u32MilliSeconds 等待的时间
    */
    void TimeJoin(uint32_t u32MilliSeconds);
 
    /** 获得该线程的ID
    @return 返回线程ID
    */
    thread_id GetThreadID() ;
    
    /** 线程是否正在运行
    @return 如果线程正在运行返回true, 否则返回false
    */
    bool IsRunning(void) const ;

public:
    
    /** 退出线程
    @param [in] u32ExitStatus 线程的退出状态
    */
    static void Exit(uint32_t u32ExitStatus) ;
 
    /** Sleep 函数
     @param [in] u32MilliSeconds Sleep的时间
    */
    static void Sleep(uint32_t u32MilliSeconds) ;
    
    /** 让出CPU给其他线程
    */
    static void yield() ;
    
    /** 获得当前线程的ID
    @return 返回当前线程ID
    */
    static thread_id GetCurrentThreadID() ;

private:
    
    /** 实现数据
    */
    struct TImpl ;
    TImpl* m_impl ;
};

/** 线程智能指针定义
*/
typedef RCSharedPtr<RCThread> RCThreadPtr ;

END_NAMESPACE_RCZIP

#endif //__RCThread_h_