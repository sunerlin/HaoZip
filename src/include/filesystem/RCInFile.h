/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCInFile_h_
#define __RCInFile_h_ 1

#include "base/RCDefs.h"

#ifdef RCZIP_OS_WIN
    #include "filesystem/windows/RCWinInFile.h"    
#else
    #include "filesystem/linux/RCLinInFile.h"
#endif

BEGIN_NAMESPACE_RCZIP

/** 读取文件实现类
*/
#ifdef RCZIP_OS_WIN
    typedef RCWinInFile RCInFile ;    
#else
    typedef RCLinInFile RCInFile ;
#endif

END_NAMESPACE_RCZIP

#endif //__RCInFile_h_
