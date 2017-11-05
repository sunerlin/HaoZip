/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBaseEvent_h_
#define __RCBaseEvent_h_ 1

#include "thread/RCHandle.h"
#include "algorithm/Threads.h"

BEGIN_NAMESPACE_RCZIP

class RCBaseEvent
{
public:

    /** 默认构造函数
    */
    RCBaseEvent() ;

    /** 默认析构函数
    */
    ~RCBaseEvent() ;

public:

    /** Event是否被创建
    @return 如果被创建返回true, 否则返回false
    */
    bool IsCreated() ;

    /** 返回句柄值
    */
    operator HANDLE() const ;

    /** 关闭Event
    @return 成功返回0, 否则返回错误号
    */
    WRes Close() ;

#ifdef RCZIP_OS_WIN

    /** 创建Event
    @param [in] manualReset 是否设置manualReset属性
    @param [in] initiallyOwn 是否初始化为拥有者
    @param [in] name Event名称
    @param [in] securityAttributes 安全属性
    @return 成功返回0, 否则返回错误号
    */
    WRes Create(bool manualReset,
                bool initiallyOwn,
                LPCTSTR name = NULL,
                LPSECURITY_ATTRIBUTES securityAttributes = NULL) ;

    /** Open Event
    @param [in] desiredAccess Desired Access 属性
    @param [in] inheritHandle 是否继承句柄
    @param [in] name Event名称
    @return 成功返回0, 否则返回错误号
    */
    WRes Open(DWORD desiredAccess, bool inheritHandle, LPCTSTR name) ;
    
#endif

    /** Set Event
    @return 成功返回0, 否则返回错误号
    */
    WRes Set() ;

    /** Set Event
    @return 成功返回0, 否则返回错误号
    */
    WRes Reset() ;
    
    /** Lock Event
    @return 成功返回0, 否则返回错误号
    */
    WRes Lock() ;
    
protected:

    /** Event对象值
    */
    ::CEvent m_object ;
};

END_NAMESPACE_RCZIP

#endif //__RCBaseEvent_h_
