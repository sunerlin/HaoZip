/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCOutFile_h_
#define __RCOutFile_h_ 1

#include "base/RCDefs.h"

#ifdef RCZIP_OS_WIN
    #include "filesystem/windows/RCWinOutFile.h"    
#else
    #include "filesystem/linux/RCLinOutFile.h"
#endif


BEGIN_NAMESPACE_RCZIP

/** 写文件操作类
*/
#ifdef RCZIP_OS_WIN
    typedef RCWinOutFile RCOutFile ;
#else
    typedef RCLinOutFile RCOutFile ;
#endif

END_NAMESPACE_RCZIP

#endif //__RCOutFile_h_
