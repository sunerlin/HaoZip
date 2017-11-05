/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSmartPtr_h_
#define __RCSmartPtr_h_ 1

#include <boost/smart_ptr.hpp>

/** 共享智能指针宏定义，boost实现转接
*/
#define RCSharedPtr          boost::shared_ptr
    
/** 共享智能指针数组宏定义，boost实现转接
*/
#define RCSharedArrayPtr     boost::shared_array

/** Scoped智能指针宏定义，boost实现转接
*/
#define RCScopedPtr          boost::scoped_ptr
    
/** Scoped智能指针数组宏定义，boost实现转接
*/
#define RCScopedArrayPtr     boost::scoped_array

#endif //__RCSmartPtr_h_
