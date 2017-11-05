/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCIntrusivePtr_h_
#define __RCIntrusivePtr_h_ 1

#include "base/RCRefCounted.h"
#include <boost/intrusive_ptr.hpp>

BEGIN_NAMESPACE_RCZIP

/** Intrusive智能指针宏定义
*/
    
#define RCIntrusivePtr  boost::intrusive_ptr

/** 对接口对象引用计数加1
@param [in,out] p 接口指针
*/
inline void intrusive_ptr_add_ref(RCRefCounted* p)
{
    p->Increase() ;
}

/** 对接口对象引用计数减1
@param [in,out] p 接口指针
*/
inline void intrusive_ptr_release(RCRefCounted* p)
{
    p->Decrease() ;
}

END_NAMESPACE_RCZIP

#endif //__RCIntrusivePtr_h_
