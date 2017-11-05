/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCAutoResetEvent_h_
#define __RCAutoResetEvent_h_ 1

#include "thread/RCBaseEvent.h"

BEGIN_NAMESPACE_RCZIP

class RCAutoResetEvent: 
    public RCBaseEvent
{
public:

    /** 创建Event
    @return 成功返回0, 否则返回错误号
    */
    WRes Create() ;

    /** 如果没有创建自动创建，如果已经创建，返回成功标记
    @return 成功返回0, 否则返回错误号
    */
    WRes CreateIfNotCreated() ;
};

END_NAMESPACE_RCZIP

#endif //__RCAutoResetEvent_h_
