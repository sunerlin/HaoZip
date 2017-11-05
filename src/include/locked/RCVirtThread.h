/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCVirtThread_h_
#define __RCVirtThread_h_ 1

#include "base/RCNonCopyable.h"

BEGIN_NAMESPACE_RCZIP

class RCThread ;

class RCVirtThread:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCVirtThread() ;
    
    /** 默认析构函数
    */
    virtual ~RCVirtThread() ;
    
public:
    
    /** 启动线程
    @return 成功返回true，失败返回false
    */
    bool Start(void);
    
    /** 等待线程结束
    */
    void WaitFinish(void) ;
    
    /** 在线程中执行函数，该函数结束，则线程退出
    */
    virtual void Execute(void) = 0 ;
    
private:
    
    /** 线程实现
    */
    RCThread* m_thread ;
};

END_NAMESPACE_RCZIP

#endif //__RCVirtThread_h_
