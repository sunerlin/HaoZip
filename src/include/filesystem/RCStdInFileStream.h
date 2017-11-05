/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCStdInFileStream_h_
#define __RCStdInFileStream_h_ 1

#include "base/RCDefs.h"
#ifdef RCZIP_OS_WIN
    #include "filesystem/windows/RCWinStdInFileStream.h"
#else
    #include "filesystem/linux/RCLinStdInFileStream.h"
#endif

BEGIN_NAMESPACE_RCZIP

/** 控制台输入类
*/
#ifdef RCZIP_OS_WIN
    typedef RCWinStdInFileStream RCStdInFileStream ;
#else
    typedef RCLinStdInFileStream RCStdInFileStream ;
#endif

END_NAMESPACE_RCZIP

#endif //__RCStdInFileStream_h_
