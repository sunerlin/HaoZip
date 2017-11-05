/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCManualResetEvent_h_
#define __RCManualResetEvent_h_ 1

#include "thread/RCBaseEvent.h"

BEGIN_NAMESPACE_RCZIP

class RCManualResetEvent: 
    public RCBaseEvent
{
public:
    
    /** 创建Event
    @param [in] initiallyOwn 是否初始化为拥有者
    @return 成功返回0, 否则返回错误号
    */
    WRes Create(bool initiallyOwn = false) ;
    
    /** 如果没有创建自动创建，如果已经创建，返回成功标记
    @return 成功返回0, 否则返回错误号
    */
    WRes CreateIfNotCreated() ;

#ifdef RCZIP_OS_WIN

    /** 创建有名Event
    @param [in] initiallyOwn 是否初始化为拥有者
    @param [in] name 名称
    */
    WRes CreateWithName(bool initiallyOwn, LPCTSTR name) ;
    
#endif
};

END_NAMESPACE_RCZIP

#endif //__RCManualResetEvent_h_
