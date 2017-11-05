/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSharedLibrary_h_
#define __RCSharedLibrary_h_ 1

#include "base/RCDefs.h"

#ifdef RCZIP_OS_WIN
    #include "common/RCSharedLibWin32.h"
#else
    #include "common/RCSharedLibLinux.h"
#endif

BEGIN_NAMESPACE_RCZIP

/** 动态链接库加载管理类
*/
#ifdef RCZIP_OS_WIN
    /** Windows平台实现
    */
    typedef RCSharedLibWin32 RCSharedLibrary ;
#else
    /** Linux平台实现 
    */
    typedef RCSharedLibLinux RCSharedLibrary ;
#endif

END_NAMESPACE_RCZIP

#endif //__RCSharedLibrary_h_
